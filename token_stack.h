/**
 * @file token_stack.h
 */
#ifndef TOKEN_STACK_H
#define TOKEN_STACK_H
#include "Read.h"
#include "ignore_iterator.h"
#include <utf8.h>
#include <deque>
#include <string>

/**
 * Converts a UTF-32 character sequence to UTF-8 tokens.
 * @tparam S Source stack type.
 */
template <class S>
class token_stack {
	S&                      source;
	std::deque<std::string> buffer;
	Read                    context;
public:
	token_stack(S&);
	bool empty() const;
	void pop();
	void push(const std::string&);
	const std::string& top() const;
private:
	void read();
	void ignore_silence();
	template<class O> bool accept_string(O);
	static bool any(uint32_t);
	template<uint32_t C> static bool is(uint32_t);
	template<uint32_t C> static bool is_not(uint32_t);
	static bool is_word(uint32_t);
	template<class P, class O = ignore_iterator> bool single(P, O = O());
	template<class P, class O = ignore_iterator> bool multiple(P, O = O());
};

/**
 * Ignores any BOM and gets the first token.
 */
template<class S>
token_stack<S>::token_stack(S& stack) : source(stack) {
	single(is<0xFEFF>);
	read();
}

/**
 * End-of-range test.
 */
template<class S>
bool token_stack<S>::empty() const {
	return buffer.empty();
}

/**
 * Removes the first token.
 */
template<class S>
void token_stack<S>::pop() {
	buffer.pop_front();
	read();
}

/**
 * Ungets a token.
 */
template<class S>
void token_stack<S>::push(const std::string& token) {
	buffer.push_front(token);
}

/**
 * Gets the current token.
 */
template<class S>
const std::string& token_stack<S>::top() const {
	return buffer.front();
}

/**
 * Reads a token from the source.
 */
template<class S>
void token_stack<S>::read() {

	if (source.empty()) return;

	std::string token;
	auto accept = std::back_inserter(token);

	ignore_silence();
	if (source.empty()) return;

	if (accept_string(accept)
		|| single(is<'('>, accept)
		|| single(is<')'>, accept)) {
		buffer.push_back(token);
		return;
	}

	multiple(is_word, accept);

	if (token == "_token") {
		if (buffer.empty())
			throw std::runtime_error("Expected token definition.");
		if (buffer.front()[0] != '"')
			throw std::runtime_error("Expected quoted token definition.");
		if (buffer.front().size() == 1)
			throw std::runtime_error("Invalid token definition.");
		context.def(buffer.front().substr(1));
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
				buffer.push_back(normal);
				normal.clear();
			}
			if (!maximum.empty()) {
				buffer.push_back(maximum);
				auto first = token.begin();
				utf8::advance(first,
					utf8::distance(maximum.begin(), maximum.end()),
					token.end());
				token = std::string(first, token.end());
			}
		}
		return;
	}

	buffer.push_back(token);

}

/**
 * Ignores whitespace and comments.
 */
template<class S>
void token_stack<S>::ignore_silence() {
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
}

/**
 * Accepts a string token.
 */
template<class S>
template<class O>
bool token_stack<S>::accept_string(O accept) {
	if (!single(is<'"'>, accept)) return false;
	if (source.empty())
		throw std::runtime_error
			("Expected closing quote before EOF.");
	while (true) {
		if (single(is<'\\'>)) {
			throw std::runtime_error("Expected escape before EOF.");
			single(is<'\\'>, accept) || single(is<'"'>, accept);
		} else if (single(is<'"'>)) {
			break;
		} else {
			if (source.empty())
				throw std::runtime_error
					("Expected character before EOF.");
			single(any, accept);
		}
	}
	return true;
}

/**
 * Matches any character.
 */
template<class S>
bool token_stack<S>::any(uint32_t) {
	return true;
}

/**
 * Matches a given character.
 */
template<class S>
template<uint32_t C>
bool token_stack<S>::is(uint32_t c) {
	return c == C;
}

/**
 * Matches anything but a given character.
 */
template<class S>
template<uint32_t C>
bool token_stack<S>::is_not(uint32_t c) {
	return c != C;
}

/**
 * Matches a word (identifier) character.
 */
template<class S>
bool token_stack<S>::is_word(uint32_t c) {
	return std::string(" \n\r\t\v()#").find(c) == std::string::npos;
}

/**
 * Accepts a single character matching a predicate.
 */
template<class S>
template<class P, class O>
bool token_stack<S>::single(P predicate, O output) {
	if (!source.empty() && predicate(source.top())) {
		utf8::append(source.top(), output);
		source.pop();
		return true;
	}
	return false;
}

/**
 * Accepts multiple characters matching a predicate.
 */
template<class S>
template<class P, class O>
bool token_stack<S>::multiple(P predicate, O output) {
	bool matched = false;
	while (!source.empty() && predicate(source.top())) {
		matched = true;
		utf8::append(source.top(), output);
		source.pop();
	}
	return matched;
}

#endif
