#ifndef EXPANDER_H
#define EXPANDER_H
#include <deque>
#include <memory>

class Context;
class Parser;
class Term;

class Expander {
	mutable std::deque<std::shared_ptr<Term>> buffer;
	mutable Parser& source;
	Context& context;
public:
	Expander(Parser&, Context&);
	bool empty() const;
	void pop();
	std::shared_ptr<Term> top() const;
private:
	void read() const;
};

#endif
