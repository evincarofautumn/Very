/**
 * @file context_iterator.h
 */
#ifndef CONTEXT_ITERATOR_H
#define CONTEXT_ITERATOR_H
#include "Context.h"

/**
 * Lazily evaluates a Term iterator range.
 * @tparam I Input iterator type.
 */
template<class I>
class context_iterator
	: public std::iterator<std::forward_iterator_tag, Context> {
	Context context;
	I here;
	I end;
public:
	context_iterator() {}
	context_iterator(I, I = I());
	context_iterator& operator*();
	context_iterator& operator++();
	context_iterator operator++(int);
	bool operator==(const context_iterator&);
	bool operator!=(const context_iterator&);
};

/**
 * Constructs a context_iterator from a range.
 * @param first Start of range.
 * @param last  End of range.
 */
template<class I>
context_iterator<I>::context_iterator(I first, I last)
	: here(first), end(last) {}

/**
 * Evaluates the current Term in the current Context.
 */
template<class I>
context_iterator<I>& context_iterator<I>::operator*() {
	(**here)(context);
	return *this;
}

/**
 * Seeks to the next Term.
 */
template<class I>
context_iterator<I>& context_iterator<I>::operator++() {
	++here;
	return *this;
}

/**
 * Post-increment to satisfy iterator interface.
 */
template<class I>
context_iterator<I> context_iterator<I>::operator++(int) {
	context_iterator temp(*this);
	++*this;
	return temp;
}

/**
 * Compares context_iterator for equality.
 */
template<class I>
bool context_iterator<I>::operator==(const context_iterator<I>& other) {
	return here == other.here;
}

/**
 * Compares context_iterator for inequality.
 */
template<class I>
bool context_iterator<I>::operator!=(const context_iterator<I>& other) {
	return !(*this == other);
}

#endif
