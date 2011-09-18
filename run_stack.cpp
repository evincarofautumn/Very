#include "run_stack.h"
#include "Term.h"
#include "term_stack.h"

/**
 * Constructs a run_iterator from a range.
 * @param stack Source stack.
 */
run_stack::run_stack(term_stack& stack, Context& context)
	: source(stack), context(context) {}

/**
 * End-of-range test.
 */
bool run_stack::empty() const {
	return source.empty();
}

/**
 * Evaluates and removes the top Term.
 */
void run_stack::pop() {
	(*source.top())(context);
	source.pop();
}
