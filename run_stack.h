/**
 * @file run_stack.h
 */
#ifndef RUN_STACK_H
#define RUN_STACK_H
#include "Run.h"

class Context;

/**
 * Lazily evaluates a Term stack.
 * @tparam S Source stack type.
 */
template<class S>
class run_stack {
	S&       source;
	Context& context;
public:
	run_stack(S&, Context&);
	bool empty() const;
	void pop();
};

/**
 * Constructs a run_iterator from a range.
 * @param stack Source stack.
 */
template<class S>
run_stack<S>::run_stack(S& stack, Context& context)
	: source(stack), context(context) {}

/**
 * End-of-range test.
 */
template<class S>
bool run_stack<S>::empty() const {
	return source.empty();
}

/**
 * Evaluates and removes the top Term.
 */
template<class S>
void run_stack<S>::pop() {
	(*source.top())(context);
	source.pop();
}

#endif
