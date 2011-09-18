/**
 * @file term_stack.h
 */
#ifndef TERM_STACK_H
#define TERM_STACK_H
#include <deque>
#include <memory>

class Term;
class Tokenizer;

/**
 * Parses a token sequence into terms.
 */
class term_stack {
	std::shared_ptr<Tokenizer> source;
	typedef std::deque<std::shared_ptr<Term>> buffer_type;
	std::shared_ptr<buffer_type> buffer;
public:
	term_stack(Tokenizer&);
	bool empty() const;
	void pop();
	void push(std::shared_ptr<Term>);
	std::shared_ptr<Term> top();
private:
	void read();
};

#endif
