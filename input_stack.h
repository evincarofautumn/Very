#ifndef INPUT_STACK_H
#define INPUT_STACK_H
#include <deque>
#include <iosfwd>
#include <iterator>

class input_stack {
	std::istreambuf_iterator<char> source;
	std::deque<uint32_t> buffer;
public:
	input_stack(std::istream&);
	void push(uint32_t);
	void pop();
	uint32_t top() const;
	bool empty() const;
private:
	void read();
};

#endif
