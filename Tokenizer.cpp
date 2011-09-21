#include "Tokenizer.h"

/**
 * Ignores any BOM and gets the first token.
 */
Tokenizer::Tokenizer(Reader& stack, Context& context)
	: source(stack), context(context) {
	single(is<0xFEFF>);
}

/**
 * End-of-range test. Must test for a failed call to read() because the last
 * token can occur well before the last character.
 */
bool Tokenizer::empty() const {
	return buffer.empty() && (source.empty() || !read());
}

/**
 * Removes the first token.
 */
void Tokenizer::pop() {
	if (buffer.empty()) read();
	buffer.pop_front();
}

/**
 * Ungets a token.
 */
void Tokenizer::push(const std::string& token) {
	buffer.push_front(token);
}

/**
 * Gets the current token.
 */
const std::string& Tokenizer::top() const {
	if (buffer.empty()) read();
	return buffer.front();
}

/**
 * Reads a token from the source.
 */
bool Tokenizer::read() const {

	if (source.empty()) return false;

	std::string token;
	auto accept = std::back_inserter(token);

	ignore_silence();
	if (source.empty()) return false;

	if (accept_string(accept)
		|| single(is<'('>, accept)
		|| single(is<')'>, accept)) {
		buffer.push_back(token);
		return true;
	}

	multiple(is_word, accept);

	while (!token.empty()) {
		std::string maximum;
		std::string normal;
		while (!token.empty()) {
			maximum.clear();
			for (auto i = context.tokens_begin();
				i != context.tokens_end(); ++i)
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

	return !buffer.empty();

}

/**
 * Ignores whitespace and comments.
 */
void Tokenizer::ignore_silence() const {
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
