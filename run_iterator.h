/**
 * @file run_iterator.h
 */
#ifndef RUN_ITERATOR_H
#define RUN_ITERATOR_H
#include "Run.h"

/**
 * Lazily evaluates a Term iterator range.
 * @tparam I Input iterator type.
 */
template<class I>
class run_iterator
	: public std::iterator<std::forward_iterator_tag, Run> {
	Run context;
	I here;
	I end;
public:
	run_iterator() {}
	run_iterator(I, I = I());
	run_iterator& operator*();
	run_iterator& operator++();
	run_iterator operator++(int);
	bool operator==(const run_iterator&);
	bool operator!=(const run_iterator&);
};

/**
 * Constructs a run_iterator from a range.
 * @param first Start of range.
 * @param last  End of range.
 */
template<class I>
run_iterator<I>::run_iterator(I first, I last)
	: here(first), end(last) {}

/**
 * Evaluates the current Term in the current runtime context.
 */
template<class I>
run_iterator<I>& run_iterator<I>::operator*() {
	(**here)(context);
	return *this;
}

/**
 * Seeks to the next Term.
 */
template<class I>
run_iterator<I>& run_iterator<I>::operator++() {
	++here;
	return *this;
}

/**
 * Post-increment to satisfy iterator interface.
 */
template<class I>
run_iterator<I> run_iterator<I>::operator++(int) {
	run_iterator temp(*this);
	++*this;
	return temp;
}

/**
 * Compares run_iterator for equality.
 */
template<class I>
bool run_iterator<I>::operator==(const run_iterator<I>& other) {
	return here == other.here;
}

/**
 * Compares run_iterator for inequality.
 */
template<class I>
bool run_iterator<I>::operator!=(const run_iterator<I>& other) {
	return !(*this == other);
}

#endif
