#include "token_stack.h"

/**
 * Ignores any BOM and gets the first token.
 */
token_stack::token_stack(input_stack& stack, Context& context)
	: source(stack), context(context) {
	single(is<0xFEFF>);
}

/**
 * End-of-range test.
 */
bool token_stack::empty() const {
	return buffer.empty() && source.empty();
}

/**
 * Removes the first token.
 */
void token_stack::pop() {
	buffer.pop_front();
	read();
}

/**
 * Ungets a token.
 */
void token_stack::push(const std::string& token) {
	buffer.push_front(token);
}

/**
 * Gets the current token.
 */
const std::string& token_stack::top() {
	if (buffer.empty()) read();
	return buffer.front();
}

/**
 * Reads a token from the source.
 */
void token_stack::read() {

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
		context.define_token(buffer.front().substr(1));
	} else {
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
		return;
	}

	buffer.push_back(token);

}

/**
 * Ignores whitespace and comments.
 */
void token_stack::ignore_silence() {
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
