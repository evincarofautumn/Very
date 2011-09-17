/**
 * @file input_stack.cpp
 */
#include "input_stack.h"
#include <utf8.h>

/**
 * Gets the first character from the stream.
 */
input_stack::input_stack(std::istream& stream) : source(stream) {
	read();
}

/**
 * End-of-range test.
 */
bool input_stack::empty() const {
	return buffer.empty();
}

/**
 * Removes the current character.
 */
void input_stack::pop() {
	buffer.pop_front();
	read();
}

/**
 * Ungets a character.
 */
void input_stack::push(uint32_t c) {
	buffer.push_front(c);
}

/**
 * Gets the current character.
 */
uint32_t input_stack::top() const {
	return buffer.front();
}

/**
 * Reads and converts a character from the input stream.
 */
void input_stack::read() {
	std::istreambuf_iterator<char> end;
	if (source == end) return;
	buffer.push_back(utf8::next(source, end));
}
