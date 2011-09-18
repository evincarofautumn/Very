#include "Context.h"
#include <algorithm>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <utf8.h>

/**
 * Constructs a default context with initial constants and ports.
 */
Context::Context() {
	ports.push_back(&std::cin);
	ports.push_back(&std::cout);
	ports.push_back(&std::cerr);
	words["false"]  = std::make_shared<Term>(0);
	words["true"]   = std::make_shared<Term>(1);
	words["stdin"]  = std::make_shared<Term>(0);
	words["stdout"] = std::make_shared<Term>(1);
	words["stderr"] = std::make_shared<Term>(2);
}

/**
 * Maps a name to a value.
 */
void Context::define_word
	(std::shared_ptr<Term> raw_name, std::shared_ptr<Term> body) {
	std::string name;
	for (auto i = raw_name->values.begin(); i != raw_name->values.end(); ++i)
		utf8::append((*i)->tag, std::back_inserter(name));
	auto existing = words.find(name);
	if (existing != words.end()) {
		std::ostringstream message;
		message << "Redefinition of symbol \"" << name << "\".";
		throw std::runtime_error(message.str());
	}
	words[name] = body;
}

/**
 * Makes a character sequence function as a token.
 */
void Context::define_token(std::shared_ptr<Term> raw_name) {
	std::string name;
	for (auto i = raw_name->values.begin(); i != raw_name->values.end(); ++i)
		utf8::append((*i)->tag, std::back_inserter(name));
	auto existing = std::find(tokens.begin(), tokens.end(), name);
	if (existing != tokens.end()) {
		std::ostringstream message;
		message << "Redefinition of token \"" << name << "\".";
		throw std::runtime_error(message.str());
	}
	tokens.push_back(name);
	std::sort(tokens.begin(), tokens.end());
}

/**
 * Gets the input stream associated with the given port number.
 */
std::istream& Context::get_input_port(uint32_t index) {
	if (index >= ports.size())
		throw std::runtime_error("Invalid port number.");
	if (ports[index].type == OSTREAM)
		throw std::runtime_error("Output port cannot be used for input.");
	return *ports[index].pointer.i;
}

/**
 * Gets the output stream associated with the given port number.
 */
std::ostream& Context::get_output_port(uint32_t index) {
	if (index >= ports.size())
		throw std::runtime_error("Invalid port number.");
	if (ports[index].type == ISTREAM)
		throw std::runtime_error("Input port cannot be used for output.");
	return *ports[index].pointer.o;
}

/**
 * Gets the word with the given name.
 */
std::shared_ptr<Term> Context::get_word(std::shared_ptr<Term> raw_name) {
	std::string name;
	for (auto i = raw_name->values.begin(); i != raw_name->values.end(); ++i)
		utf8::append((*i)->tag, std::back_inserter(name));
	auto existing = words.find(name);
	if (existing == words.end()) {
		std::ostringstream message;
		message << "Use of undefined symbol \"" << name << "\".";
		throw std::runtime_error(message.str());
	}
	return existing->second;
}

/**
 * Yields the top element of the stack and removes it.
 */
std::shared_ptr<Term> Context::pop() {
	if (terms.empty())
		return std::make_shared<Term>();
	auto top = terms.back();
	terms.pop_back();
	return top;
}

/**
 * Pushes the given value to the stack.
 */
void Context::push(std::shared_ptr<Term> value) {
	terms.push_back(value);
}

/**
 * Yields the top element of the stack without removing it.
 */
std::shared_ptr<Term> Context::top() const {
	if (terms.empty())
		return std::make_shared<Term>();
	return terms.back();
}

/**
 * Gets a begin iterator to the declared tokens.
 */
std::vector<std::string>::const_iterator Context::tokens_begin() const {
	return tokens.begin();
}

/**
 * Gets an end iterator to the declared tokens.
 */
std::vector<std::string>::const_iterator Context::tokens_end() const {
	return tokens.end();
}
