#ifndef TERM_STACK_H
#define TERM_STACK_H
#include "Term.h"
#include <deque>
#include <memory>

template<class S>
class term_stack {
	std::shared_ptr<S> source;
	typedef std::deque<std::shared_ptr<Term>> buffer_type;
	std::shared_ptr<buffer_type> buffer;
public:
	term_stack(S&);
	bool empty() const;
	void pop();
	void push(std::shared_ptr<Term>);
	std::shared_ptr<Term> top();
private:
	void read();
};

template<class S>
term_stack<S>::term_stack(S& stack)
	: source(new S(stack)), buffer(new buffer_type()) {
	read();
}

template<class S>
bool term_stack<S>::empty() const {
	return buffer->empty();
}

template<class S>
void term_stack<S>::pop() {
	buffer->pop_front();
}

template<class S>
void term_stack<S>::push(std::shared_ptr<Term> term) {
	buffer->push_front(term);
}

template<class S>
std::shared_ptr<Term> term_stack<S>::top() {
	read();
	return buffer->front();
}

template<class S>
void term_stack<S>::read() {
	if (source->empty()) return;
	std::shared_ptr<Term> term;
	if (source->top() == "(") {
		source->pop();
		term.reset(new Term());
		term_stack stack(*this);
		while (!source->empty() && source->top() != ")") {
			term->values.push_back(stack.top());
			stack.pop();
		}
		if (source->empty())
			throw std::runtime_error("Expected ) before EOF.");
		source->pop(); // )
		buffer->push_back(term);
		return;
	}
	term.reset(new Term(source->top()));
	source->pop();
	buffer->push_back(term);
}

#endif
