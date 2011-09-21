#include "Expander.h"
#include "Context.h"
#include "Parser.h"

Expander::Expander(Parser& stack, Context& context)
	: source(stack), context(context) {}

bool Expander::empty() const {
	return buffer.empty() && source.empty();
}

void Expander::pop() {
	if (buffer.empty()) read();
	buffer.pop_front();
}

std::shared_ptr<Term> Expander::top() const {
	if (buffer.empty()) read();
	return buffer.front();
}

void Expander::read() const {
	if (source.empty()) return;
	if (*source.top() == Term("_token")) {
		context.define_token(buffer.back());
		buffer.pop_back();
		buffer.push_back(source.top());
		source.pop();
	}
	if (source.empty()) return;
	buffer.push_back(source.top());
	source.pop();
}
