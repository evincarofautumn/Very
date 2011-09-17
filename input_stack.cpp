#include "input_stack.h"
#include <utf8.h>

input_stack::input_stack(std::istream& stream) : source(stream) {
	read();
}

void input_stack::push(uint32_t c) {
	buffer.push_front(c);
}

void input_stack::pop() {
	buffer.pop_front();
	read();
}

uint32_t input_stack::top() const {
	return buffer.front();
}

bool input_stack::empty() const {
	return buffer.empty();
}

void input_stack::read() {
	std::istreambuf_iterator<char> end;
	if (source == end) return;
	buffer.push_back(utf8::next(source, end));
}
