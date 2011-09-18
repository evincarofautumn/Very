/**
 * @file Interpreter.h
 */
#ifndef INTERPRETER_H
#define INTERPRETER_H

class Context;
class term_stack;

/**
 * Lazily evaluates a Term stack.
 */
class Interpreter {
	term_stack& source;
	Context& context;
public:
	Interpreter(term_stack&, Context&);
	bool empty() const;
	void pop();
};

#endif
