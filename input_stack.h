/**
 * @file input_stack.h
 */
#ifndef INPUT_STACK_H
#define INPUT_STACK_H
#include <deque>
#include <iosfwd>
#include <iterator>

/**
 * Adapts an input stream into a stack.
 */
class input_stack {
	std::istreambuf_iterator<char> source;
	std::deque<uint32_t> buffer;
public:
	input_stack(std::istream&);
	bool empty() const;
	void pop();
	void push(uint32_t);
	uint32_t top() const;
private:
	void read();
};

#endif
