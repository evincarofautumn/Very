#ifndef CONTEXT_H
#define CONTEXT_H
#include <iosfwd>
#include <memory>
#include <stack>
#include "Term.h"

/**
 * An execution context.
 */
class Context {
	std::stack<std::shared_ptr<Term>> stack;
	std::map<std::string, std::shared_ptr<Term>> defs;
	enum stream_type {
		ISTREAM,
		OSTREAM,
		IOSTREAM
	};
	union stream_pointer {
		std::istream*  i;
		std::ostream*  o;
		std::iostream* io;
	};
	struct stream {
		stream(std::istream* s) : type(ISTREAM) { pointer.i = s; }
		stream(std::ostream* s) : type(OSTREAM) { pointer.o = s; }
		stream(std::iostream* s) : type(IOSTREAM) { pointer.io = s; }
		stream_type type;
		stream_pointer pointer;
	};
	std::vector<stream> ports;
public:
	Context();
	void def(std::shared_ptr<Term>, std::shared_ptr<Term>);
	std::shared_ptr<Term> get(std::shared_ptr<Term>);
	std::shared_ptr<Term> peek() const;
	std::shared_ptr<Term> pop();
	std::istream& input_port(uint32_t);
	std::ostream& output_port(uint32_t);
	void push(std::shared_ptr<Term>);
};

#endif
