#include "Expander.h"
#include "Context.h"
#include "Parser.h"

Expander::Expander(Parser& stack, Context& context)
	: context(context), source(stack) {}

bool Expander::empty() const {
	return buffer.empty() && source.empty();
}

void Expander::pop() {
	buffer.pop_front();
}

std::shared_ptr<Term> Expander::top() {
	if (buffer.empty()) read();
	return buffer.front();
}

void Expander::read() {
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
