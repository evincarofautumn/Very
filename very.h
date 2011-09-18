/**
 * @file very.h
 *
 * Typedefs for composing the necessary stacks to run a program.
 */
#ifndef VERY_H
#define VERY_H
#include "Reader.h"
#include "Tokenizer.h"
#include "term_stack.h"
#include "Interpreter.h"

/**
 * Forces lazy computation expressed through stack composition.
 * @tparam S     Stack type.
 * @param  stack Stack itself.
 */
template<class S>
void force(S& stack) {
	while (!stack.empty()) stack.pop();
}

#endif
