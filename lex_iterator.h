#ifndef LEX_ITERATOR_H
#define LEX_ITERATOR_H
#include <utf8.h>
#include <iterator>
#include <string>

/**
 * A forward iterator to convert a UTF-32 character sequence to UTF-8 tokens.
 */
template <class B>
class lex_iterator :
	public std::iterator<std::forward_iterator_tag, std::string> {
	B here;
	B end;
	std::string token;
public:
	lex_iterator();
	lex_iterator(B, B = B());
	std::string operator*() const;
	bool operator==(const lex_iterator& other) const;
	bool operator!=(const lex_iterator& other) const;
	lex_iterator& operator++();
	lex_iterator operator++(int);
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
 * Constructs a past-the-end lex_iterator.
 */
template<class B>
lex_iterator<B>::lex_iterator() {}

/**
 * Ignores any BOM and seeks to the first token.
 */
template<class B>
lex_iterator<B>::lex_iterator(B first, B last)
	: here(first), end(last) {
	single(is<0xFEFF>);
	++*this;
}

/**
 * Returns the current token.
 */
template<class B>
std::string lex_iterator<B>::operator*() const {
	return token;
}

/**
 * Compares two lex_iterators for equality. The token must be tested to
 * differentiate between pre- and post-EOF iterators.
 */
template<class B>
bool lex_iterator<B>::operator==(const lex_iterator<B>& other) const {
	return here == other.here && token.empty() == other.token.empty();
}

template<class B>
bool lex_iterator<B>::operator!=(const lex_iterator<B>& other) const {
	return !(*this == other);
}

/**
 * Moves to the next token.
 */
template<class B>
lex_iterator<B>& lex_iterator<B>::operator++() {

	token.clear();
	if (at_end()) return *this;

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
	if (at_end()) return *this;

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

		return *this;

	}

	single(is<'('>, accept)
		|| single(is<')'>, accept)
		|| multiple(is_word, accept);

	return *this;

}

template<class B>
lex_iterator<B> lex_iterator<B>::operator++(int) {
	lex_iterator<B> temp(*this);
	++*this;
	return temp;
}

template<class B>
bool lex_iterator<B>::at_end() const {
	return here == end;
}

template<class B>
template<class P, class O>
bool lex_iterator<B>::single(P predicate, O output) {
	if (!at_end() && predicate(*here)) {
		utf8::append(*here++, output);
		return true;
	}
	return false;
}

template<class B>
template<class P, class O>
bool lex_iterator<B>::multiple(P predicate, O output) {
	bool matched = false;
	while (!at_end() && predicate(*here)) {
		matched = true;
		utf8::append(*here++, output);
	}
	return matched;
}

template<class B>
bool lex_iterator<B>::is_word(uint32_t c) {
	return std::string(" \n\r\t\v()#").find(c) == std::string::npos;
}

template<class B>
template<uint32_t C>
bool lex_iterator<B>::is(uint32_t c) {
	return c == C;
}

template<class B>
template<uint32_t C>
bool lex_iterator<B>::is_not(uint32_t c) {
	return c != C;
}

template<class B>
bool lex_iterator<B>::any(uint32_t) {
	return true;
}

#endif
