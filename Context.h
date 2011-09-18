#ifndef CONTEXT_H
#define CONTEXT_H
#include "Term.h"
#include <map>
#include <memory>
#include <stack>
#include <vector>

class Context {

	std::vector<std::string>                     tokens;
	std::stack<std::shared_ptr<Term>>            terms;
	std::map<std::string, std::shared_ptr<Term>> words;

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

	void define_word(std::shared_ptr<Term>, std::shared_ptr<Term>);
	void define_token(const std::string&);

	std::istream& get_input_port(uint32_t);
	std::ostream& get_output_port(uint32_t);
	std::shared_ptr<Term> get_word(std::shared_ptr<Term>);

	std::shared_ptr<Term> peek() const;
	std::shared_ptr<Term> pop();
	void push(std::shared_ptr<Term>);

	std::vector<std::string>::const_iterator tokens_begin() const;
	std::vector<std::string>::const_iterator tokens_end() const;

};

#endif
