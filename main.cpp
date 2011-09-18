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

	Context context;
	std::ifstream stream(argv[0]);
	Reader reader(stream);
	Tokenizer tokenizer(reader, context);
	term_stack parser(tokenizer);
	Interpreter interpreter(parser, context);
	force(interpreter);

} catch (const std::runtime_error& error) {

	std::cerr << error.what() << '\n';
	return 1;

}
