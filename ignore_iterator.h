/**
 * @file ignore_iterator.h
 */
#ifndef IGNORE_ITERATOR_H
#define IGNORE_ITERATOR_H

/**
 * An output iterator for discarding values.
 */
class ignore_iterator
	: public std::iterator<std::output_iterator_tag, ignore_iterator> {
public:
	template<class T>
	ignore_iterator& operator=(const T&) { return *this; }
	ignore_iterator& operator*() { return *this; }
	ignore_iterator& operator++() { return *this; }
	ignore_iterator& operator++(int) { return *this; }
};

#endif
