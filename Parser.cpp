#include "Parser.h"
#include "Term.h"
#include "Tokenizer.h"

/**
 * Gets the first Term from the source.
 */
Parser::Parser(Tokenizer& stack)
	: source(new Tokenizer(stack)), buffer(new buffer_type()) {}

/**
 * End-of-range test.
 */
bool Parser::empty() const {
	return buffer->empty() && source->empty();
}

/**
 * Removes the current Term.
 */
void Parser::pop() {
	buffer->pop_front();
}

/**
 * Ungets a Term.
 */
void Parser::push(std::shared_ptr<Term> term) {
	buffer->push_front(term);
}

/**
 * Gets the current Term.
 */
std::shared_ptr<Term> Parser::top() {
	if (buffer->empty()) read();
	return buffer->front();
}

/**
 * Reads a (possibly nested) Term from the source.
 */
void Parser::read() {
	if (source->empty()) return;
	std::shared_ptr<Term> term;
	if (source->top() == "(") {
		source->pop();
		term.reset(new Term());
		Parser stack(*this);
		while (!source->empty() && source->top() != ")") {
			term->values.push_back(stack.top());
			stack.pop();
		}
		if (source->empty())
			throw std::runtime_error("Expected ) before EOF.");
		source->pop(); // )
		buffer->push_back(term);
		return;
	}
	term.reset(new Term(source->top()));
	source->pop();
	buffer->push_back(term);
}
