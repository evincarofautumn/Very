/**
 * @file Parser.h
 */
#ifndef PARSER_H
#define PARSER_H
#include <deque>
#include <memory>

class Term;
class Tokenizer;

/**
 * Parses a token sequence into terms.
 */
class Parser {
	std::shared_ptr<Tokenizer> source;
	typedef std::deque<std::shared_ptr<Term>> buffer_type;
	std::shared_ptr<buffer_type> buffer;
public:
	Parser(Tokenizer&);
	bool empty() const;
	void pop();
	void push(std::shared_ptr<Term>);
	std::shared_ptr<Term> top();
private:
	void read();
};

#endif
