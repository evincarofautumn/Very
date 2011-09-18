/**
 * @file token_stack.h
 */
#ifndef TOKEN_STACK_H
#define TOKEN_STACK_H
#include "Context.h"
#include "ignore_iterator.h"
#include "input_stack.h"
#include <utf8.h>
#include <deque>
#include <string>

class input_stack;

/**
 * Converts a UTF-32 character sequence to UTF-8 tokens.
 */
class token_stack {
	input_stack&            source;
	std::deque<std::string> buffer;
	Context&                context;
public:
	token_stack(input_stack&, Context&);
	bool empty() const;
	void pop();
	void push(const std::string&);
	const std::string& top();
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
 * Accepts a string token.
 */
template<class O>
bool token_stack::accept_string(O accept) {
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
inline bool token_stack::any(uint32_t) {
	return true;
}

/**
 * Matches a given character.
 */
template<uint32_t C>
bool token_stack::is(uint32_t c) {
	return c == C;
}

/**
 * Matches anything but a given character.
 */
template<uint32_t C>
bool token_stack::is_not(uint32_t c) {
	return c != C;
}

/**
 * Matches a word (identifier) character.
 */
inline bool token_stack::is_word(uint32_t c) {
	return std::string(" \n\r\t\v()#").find(c) == std::string::npos;
}

/**
 * Accepts a single character matching a predicate.
 */
template<class P, class O>
bool token_stack::single(P predicate, O output) {
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
template<class P, class O>
bool token_stack::multiple(P predicate, O output) {
	bool matched = false;
	while (!source.empty() && predicate(source.top())) {
		matched = true;
		utf8::append(source.top(), output);
		source.pop();
	}
	return matched;
}

#endif
