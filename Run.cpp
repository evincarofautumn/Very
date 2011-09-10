/**
 * @file Run.cpp
 */
#include "Run.h"
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <utf8.h>

/**
 * Constructs a default runtime context with initial constants and ports.
 */
Run::Run() {
	ports.push_back(&std::cin);
	ports.push_back(&std::cout);
	ports.push_back(&std::cerr);
	defs["false"]  = std::make_shared<Term>(0);
	defs["true"]   = std::make_shared<Term>(1);
	defs["stdin"]  = std::make_shared<Term>(0);
	defs["stdout"] = std::make_shared<Term>(1);
	defs["stderr"] = std::make_shared<Term>(2);
	defs["_token"] = std::make_shared<Term>();
}

/**
 * Maps a name to a value.
 */
void Run::def(std::shared_ptr<Term> raw_name, std::shared_ptr<Term> body) {
	std::string name;
	for (auto i = raw_name->values.begin(); i != raw_name->values.end(); ++i)
		utf8::append((*i)->tag, std::back_inserter(name));
	auto existing = defs.find(name);
	if (existing != defs.end()) {
		std::ostringstream message;
		message << "Redefinition of symbol \"" << name << "\".";
		throw std::runtime_error(message.str());
	}
	defs[name] = body;
}

std::shared_ptr<Term> Run::get(std::shared_ptr<Term> raw_name) {
	std::string name;
	for (auto i = raw_name->values.begin(); i != raw_name->values.end(); ++i)
		utf8::append((*i)->tag, std::back_inserter(name));
	auto existing = defs.find(name);
	if (existing == defs.end()) {
		std::ostringstream message;
		message << "Use of undefined symbol \"" << name << "\".";
		throw std::runtime_error(message.str());
	}
	return existing->second;
}

/**
 * Yields the top element of the stack without removing it.
 */
std::shared_ptr<Term> Run::peek() const {
	if (stack.empty())
		return std::make_shared<Term>();
	return stack.top();
}

/**
 * Yields the top element of the stack and removes it.
 */
std::shared_ptr<Term> Run::pop() {
	if (stack.empty())
		return std::make_shared<Term>();
	auto top = stack.top();
	stack.pop();
	return top;
}

/**
 * Gets the input stream associated with the given port number.
 */
std::istream& Run::input_port(uint32_t index) {
	if (index >= ports.size())
		throw std::runtime_error("Invalid port number.");
	if (ports[index].type == OSTREAM)
		throw std::runtime_error("Output port cannot be used for input.");
	return *ports[index].pointer.i;
}

/**
 * Gets the output stream associated with the given port number.
 */
std::ostream& Run::output_port(uint32_t index) {
	if (index >= ports.size())
		throw std::runtime_error("Invalid port number.");
	if (ports[index].type == ISTREAM)
		throw std::runtime_error("Input port cannot be used for output.");
	return *ports[index].pointer.o;
}

/**
 * Pushes the given value to the stack.
 */
void Run::push(std::shared_ptr<Term> value) {
	stack.push(value);
}
