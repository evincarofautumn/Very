/**
 * @file Reader.cpp
 */
#include "Reader.h"
#include <utf8.h>

/**
 * Gets the first character from the stream.
 */
Reader::Reader(std::istream& stream) : source(stream) {
	read();
}

/**
 * End-of-range test.
 */
bool Reader::empty() const {
	return buffer.empty();
}

/**
 * Removes the current character.
 */
void Reader::pop() {
	buffer.pop_front();
	read();
}

/**
 * Ungets a character.
 */
void Reader::push(uint32_t c) {
	buffer.push_front(c);
}

/**
 * Gets the current character.
 */
uint32_t Reader::top() const {
	return buffer.front();
}

/**
 * Reads and converts a character from the input stream.
 */
void Reader::read() {
	std::istreambuf_iterator<char> end;
	if (source == end) return;
	buffer.push_back(utf8::next(source, end));
}
