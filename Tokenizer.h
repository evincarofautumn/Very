/**
 * @file Tokenizer.h
 */
#ifndef TOKENIZER_H
#define TOKENIZER_H
#include "Context.h"
#include "Ignorer.h"
#include "Reader.h"
#include <utf8.h>
#include <deque>
#include <string>

class Reader;

/**
 * Converts a UTF-32 character sequence to UTF-8 tokens.
 */
class Tokenizer {
	mutable std::deque<std::string> buffer;
	mutable Reader& source;
	Context& context;
public:
	Tokenizer(Reader&, Context&);
	bool empty() const;
	void pop();
	void push(const std::string&);
	const std::string& top() const;
private:
	bool read() const;
	void ignore_silence() const;
	template<class O> bool accept_string(O) const;
	static bool any(uint32_t);
	template<uint32_t C> static bool is(uint32_t);
	template<uint32_t C> static bool is_not(uint32_t);
	static bool is_word(uint32_t);
	template<class P, class O = Ignorer> bool single(P, O = O()) const;
	template<class P, class O = Ignorer> bool multiple(P, O = O()) const;
};

/**
 * Accepts a string token.
 */
template<class O>
bool Tokenizer::accept_string(O accept) const {
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
inline bool Tokenizer::any(uint32_t) {
	return true;
}

/**
 * Matches a given character.
 */
template<uint32_t C>
bool Tokenizer::is(uint32_t c) {
	return c == C;
}

/**
 * Matches anything but a given character.
 */
template<uint32_t C>
bool Tokenizer::is_not(uint32_t c) {
	return c != C;
}

/**
 * Matches a word (identifier) character.
 */
inline bool Tokenizer::is_word(uint32_t c) {
	return std::string(" \n\r\t\v()#").find(c) == std::string::npos;
}

/**
 * Accepts a single character matching a predicate.
 */
template<class P, class O>
bool Tokenizer::single(P predicate, O output) const {
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
bool Tokenizer::multiple(P predicate, O output) const {
	bool matched = false;
	while (!source.empty() && predicate(source.top())) {
		matched = true;
		utf8::append(source.top(), output);
		source.pop();
	}
	return matched;
}

#endif
