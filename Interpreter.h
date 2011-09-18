/**
 * @file Interpreter.h
 */
#ifndef INTERPRETER_H
#define INTERPRETER_H

class Context;
class Expander;

/**
 * Lazily evaluates a Term stack.
 */
class Interpreter {
	Expander& source;
	Context& context;
public:
	Interpreter(Expander&, Context&);
	bool empty() const;
	void pop();
};

#endif
