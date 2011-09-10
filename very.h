/**
 * @file very.h
 *
 * Typedefs for composing the necessary iterators to run a program.
 */
#ifndef VERY_H
#define VERY_H
#include "buffer_iterator.h"
#include "expand_iterator.h"
#include "ignore_iterator.h"
#include "parse_iterator.h"
#include "read_iterator.h"
#include "run_iterator.h"
#include <algorithm>
#include <utf8.h>

typedef std::istreambuf_iterator  <char>      loader;
typedef buffer_iterator           <loader>    bufferer;
typedef utf8::unchecked::iterator <bufferer>  converter;
typedef read_iterator             <converter> reader;
typedef parse_iterator            <reader>    parser;
typedef expand_iterator           <parser>    expander;
typedef run_iterator              <expander>  interpreter;

/**
 * Runs through an iterator range, sending output to an ignore iterator.
 * Used to force lazy computation expressed through iterator composition.
 * @tparam I     Input iterator type.
 * @param  begin Start of range.
 * @param  end   End of range.
 */
template<class I>
void force(I begin, I end = I()) {
	std::copy(begin, end, ignore_iterator());
}

#endif
