#include "term_stack.h"
#include "Term.h"
#include "token_stack.h"

/**
 * Gets the first Term from the source.
 */
term_stack::term_stack(token_stack& stack)
	: source(new token_stack(stack)), buffer(new buffer_type()) {}

/**
 * End-of-range test.
 */
bool term_stack::empty() const {
	return buffer->empty() && source->empty();
}

/**
 * Removes the current Term.
 */
void term_stack::pop() {
	buffer->pop_front();
}

/**
 * Ungets a Term.
 */
void term_stack::push(std::shared_ptr<Term> term) {
	buffer->push_front(term);
}

/**
 * Gets the current Term.
 */
std::shared_ptr<Term> term_stack::top() {
	if (buffer->empty()) read();
	return buffer->front();
}

/**
 * Reads a (possibly nested) Term from the source.
 */
void term_stack::read() {
	if (source->empty()) return;
	std::shared_ptr<Term> term;
	if (source->top() == "(") {
		source->pop();
		term.reset(new Term());
		term_stack stack(*this);
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
