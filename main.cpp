/**
 * @file main.cpp
 *
 * Manages the state of the interpreter.
 */
#include <fstream>
#include <iostream>
#include <stdexcept>
#include "very.h"

/**
 * Runs source whose filename is given on the command line.
 */
int main(int argc, char** argv) try {

	--argc, ++argv;
	if (argc != 1)
		throw std::runtime_error("Invalid command line.");

	std::ifstream stream(argv[0]);
	input_stack input(stream);
	token_stack<decltype(input)> tokens(input);
	term_stack<decltype(tokens)> terms(tokens);
	while (!terms.empty()) {
		std::cout << *terms.top() << '\n';
		terms.pop();
	}

} catch (const std::runtime_error& error) {

	std::cerr << error.what() << '\n';
	return 1;

}
