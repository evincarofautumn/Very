/**
 * @file run_stack.h
 */
#ifndef RUN_STACK_H
#define RUN_STACK_H

class Context;
class term_stack;

/**
 * Lazily evaluates a Term stack.
 */
class run_stack {
	term_stack& source;
	Context& context;
public:
	run_stack(term_stack&, Context&);
	bool empty() const;
	void pop();
};

#endif
