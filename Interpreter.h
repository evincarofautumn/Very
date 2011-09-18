/**
 * @file Interpreter.h
 */
#ifndef INTERPRETER_H
#define INTERPRETER_H

class Context;
class Parser;

/**
 * Lazily evaluates a Term stack.
 */
class Interpreter {
	Parser& source;
	Context& context;
public:
	Interpreter(Parser&, Context&);
	bool empty() const;
	void pop();
};

#endif
