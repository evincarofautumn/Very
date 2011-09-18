#include "Interpreter.h"
#include "Term.h"
#include "term_stack.h"

/**
 * Constructs a run_iterator from a range.
 * @param stack Source stack.
 */
Interpreter::Interpreter(term_stack& stack, Context& context)
	: source(stack), context(context) {}

/**
 * End-of-range test.
 */
bool Interpreter::empty() const {
	return source.empty();
}

/**
 * Evaluates and removes the top Term.
 */
void Interpreter::pop() {
	(*source.top())(context);
	source.pop();
}
