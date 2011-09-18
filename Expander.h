#ifndef EXPANDER_H
#define EXPANDER_H
#include <deque>
#include <memory>

class Context;
class Parser;
class Term;

class Expander {
	std::deque<std::shared_ptr<Term>> buffer;
	Context& context;
	Parser& source;
public:
	Expander(Parser&, Context&);
	bool empty() const;
	void pop();
	std::shared_ptr<Term> top();
private:
	void read();
};

#endif
