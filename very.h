#ifndef VERY_H
#define VERY_H
#include "buffer_iterator.h"
#include "ignore_iterator.h"
#include "lex_iterator.h"
#include "term_iterator.h"
#include "context_iterator.h"
#include <algorithm>
#include <utf8.h>

typedef std::istreambuf_iterator<char>      characters;
typedef buffer_iterator<characters>         buffered;
typedef utf8::unchecked::iterator<buffered> utf32;
typedef lex_iterator<utf32>                 tokens;
typedef term_iterator<tokens>               terms;
typedef context_iterator<terms>             interpreter;

template<class I>
void run(I begin, I end = I()) {
	std::copy(begin, end, ignore_iterator());
}

#endif
