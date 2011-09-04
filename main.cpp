#include <fstream>
#include <iostream>
#include <stdexcept>
#include "very.h"

int main(int argc, char** argv) try {

	--argc, ++argv;
	if (argc != 1)
		throw std::runtime_error("Invalid command line.");

	std::ifstream stream(argv[0]);
	run(interpreter(terms(tokens(utf32(buffered(characters(stream)))))));

} catch (const std::runtime_error& error) {

	std::cerr << error.what() << '\n';
	return 1;

}
