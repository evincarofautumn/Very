/**
 * @file Reader.h
 */
#ifndef READER_H
#define READER_H
#include <deque>
#include <iosfwd>
#include <iterator>

/**
 * Adapts an input stream into a stack.
 */
class Reader {
	std::istreambuf_iterator<char> source;
	std::deque<uint32_t> buffer;
public:
	Reader(std::istream&);
	bool empty() const;
	void pop();
	void push(uint32_t);
	uint32_t top() const;
private:
	void read();
};

#endif
