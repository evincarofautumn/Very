/**
 * @file run_stack.h
 */
#ifndef RUN_STACK_H
#define RUN_STACK_H
#include "Run.h"

/**
 * Lazily evaluates a Term stack.
 * @tparam S Source stack type.
 */
template<class S>
class run_stack {
	S& source;
	Run context;
public:
	run_stack(S&);
	bool empty() const;
	void pop();
};

/**
 * Constructs a run_iterator from a range.
 * @param stack Source stack.
 */
template<class S>
run_stack<S>::run_stack(S& stack) : source(stack) {}

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
