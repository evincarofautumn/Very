/**
 * @file read_iterator.h
 */
#ifndef READ_ITERATOR_H
#define READ_ITERATOR_H
#include "Read.h"
#include <utf8.h>
#include <deque>
#include <iterator>
#include <string>

/**
 * A forward iterator to convert a UTF-32 character sequence to UTF-8 tokens.
 * @tparam B Bidirectional iterator type.
 */
template <class B>
class read_iterator :
	public std::iterator<std::forward_iterator_tag, std::string> {
	Read context;
	B here;
	B end;
	std::deque<std::string> tokens;
public:
	read_iterator();
	read_iterator(B, B = B());
	std::string operator*() const;
	bool operator==(const read_iterator&) const;
	bool operator!=(const read_iterator&) const;
	read_iterator& operator++();
	read_iterator operator++(int);
private:
	bool at_end() const;
	static bool any(uint32_t);
	static bool is_word(uint32_t);
	template<class P> void ignore(P);
	template<class P, class O = ignore_iterator> bool single(P, O = O());
	template<class P, class O = ignore_iterator> bool multiple(P, O = O());
	template<uint32_t C> static bool is(uint32_t);
	template<uint32_t C> static bool is_not(uint32_t);
};

/**
 * Constructs a past-the-end read_iterator.
 */
template<class B>
read_iterator<B>::read_iterator() {}

/**
 * Ignores any BOM and seeks to the first token.
 */
template<class B>
read_iterator<B>::read_iterator(B first, B last)
	: here(first), end(last) {
	single(is<0xFEFF>);
	++*this;
}

/**
 * Returns the current token.
 */
template<class B>
std::string read_iterator<B>::operator*() const {
	return tokens.front();
}

/**
 * Compares read_iterator for equality. The token must be tested to
 * differentiate between pre- and post-EOF iterators.
 */
template<class B>
bool read_iterator<B>::operator==(const read_iterator<B>& other) const {
	return here == other.here && tokens.empty() == other.tokens.empty();
}

/**
 * Compares read_iterator for inequality.
 */
template<class B>
bool read_iterator<B>::operator!=(const read_iterator<B>& other) const {
	return !(*this == other);
}

/**
 * Moves to the next token.
 */
template<class B>
read_iterator<B>& read_iterator<B>::operator++() {

	if (at_end()) {
		if (!tokens.empty()) tokens.pop_front();
		return *this;
	}

	std::string token;
	auto accept = std::back_inserter(token);

	// Ignore whitespace and comments.
	bool matched = true;
	while (matched) {
		matched = multiple(::isspace);
		if ((matched = single(is<'#'>))) {
			if (single(is<'('>)) {
				int depth = 1;
				while (depth) {
					if (single(is<'('>))
						++depth;
					else if (single(is<')'>))
						--depth;
					else
						single(any);
				}
			} else {
				multiple(is_not<'\n'>);
			}
		}
	}

	if (at_end()) {
		if (!tokens.empty()) tokens.pop_front();
		return *this;
	}

	// Accept strings.
	if (single(is<'"'>, accept)) {

		if (at_end())
			throw std::runtime_error
				("Expected closing quote before EOF.");

		while (true) {

			if (single(is<'\\'>)) {

				throw std::runtime_error
					("Expected escape before EOF.");

				// Accept escape specifiers.
				single(is<'\\'>, accept)
					|| single(is<'"'>, accept);

			} else if (single(is<'"'>)) {

				break;

			} else {

				if (at_end())
					throw std::runtime_error
						("Expected character before EOF.");

				single(any, accept);

			}

		}

		if (!tokens.empty()) tokens.pop_front();
		tokens.push_back(token);
		return *this;

	}

	if (!(single(is<'('>, accept) || single(is<')'>, accept))) {
		multiple(is_word, accept);
		if (token == "_token") {
			if (tokens.empty())
				throw std::runtime_error("Expected token definition.");
			if (tokens.back()[0] != '"')
				throw std::runtime_error("Expected quoted token definition.");
			if (tokens.back().size() == 1)
				throw std::runtime_error("Invalid token definition.");
			context.def(tokens.back().substr(1));
		} else {
			while (!token.empty()) {
				std::string maximum;
				std::string normal;
				while (!token.empty()) {
					maximum.clear();
					for (auto i = context.begin(); i != context.end(); ++i)
						if (std::equal(i->begin(), i->end(), token.begin())) {
							if (maximum.size() < i->size())
								maximum = *i;
						}
					if (maximum.empty()) {
						auto first = token.begin();
						utf8::append(utf8::next(first, token.end()),
							std::back_inserter(normal));
						token = std::string(first, token.end());
					} else {
						break;
					}
				}
				if (!normal.empty()) {
					tokens.push_back(normal);
					normal.clear();
				}
				if (!maximum.empty()) {
					tokens.push_back(maximum);
					auto first = token.begin();
					utf8::advance(first,
						utf8::distance(maximum.begin(), maximum.end()),
						token.end());
					token = std::string(first, token.end());
				}
			}
			if (!tokens.empty()) tokens.pop_front();
			return *this;
		}
	}

	if (!tokens.empty()) tokens.pop_front();
	tokens.push_back(token);
	return *this;

}

/**
 * Post-increment to satisfy iterator interface.
 */
template<class B>
read_iterator<B> read_iterator<B>::operator++(int) {
	read_iterator<B> temp(*this);
	++*this;
	return temp;
}

/**
 * At-the-end test.
 */
template<class B>
bool read_iterator<B>::at_end() const {
	return here == end;
}

/**
 * Outputs a single character if it matches a predicate.
 * @param  predicate Predicate functor.
 * @param  output    Output iterator.
 * @return Whether a character matched.
 */
template<class B>
template<class P, class O>
bool read_iterator<B>::single(P predicate, O output) {
	if (!at_end() && predicate(*here)) {
		utf8::append(*here++, output);
		return true;
	}
	return false;
}

/**
 * Outputs multiple characters while they match a predicate.
 * @param  predicate Predicate functor.
 * @param  output    Output iterator.
 * @return Whether at least one character matched.
 */
template<class B>
template<class P, class O>
bool read_iterator<B>::multiple(P predicate, O output) {
	bool matched = false;
	while (!at_end() && predicate(*here)) {
		matched = true;
		utf8::append(*here++, output);
	}
	return matched;
}

/**
 * Predicate function for matching valid identifier characters.
 */
template<class B>
bool read_iterator<B>::is_word(uint32_t c) {
	return std::string(" \n\r\t\v()#").find(c) == std::string::npos;
}

/**
 * Predicate function for matching a literal character.
 */
template<class B>
template<uint32_t C>
bool read_iterator<B>::is(uint32_t c) {
	return c == C;
}

/**
 * Predicate function for matching anything other than the given character.
 */
template<class B>
template<uint32_t C>
bool read_iterator<B>::is_not(uint32_t c) {
	return c != C;
}

/**
 * Predicate function to match any character.
 * @return @c true
 */
template<class B>
bool read_iterator<B>::any(uint32_t) {
	return true;
}

#endif
