/**
 * @file Ignorer.h
 */
#ifndef IGNORER_H
#define IGNORER_H

/**
 * An output iterator for discarding values.
 */
class Ignorer
	: public std::iterator<std::output_iterator_tag, Ignorer> {
public:
	template<class T>
	Ignorer& operator=(const T&) { return *this; }
	Ignorer& operator*() { return *this; }
	Ignorer& operator++() { return *this; }
	Ignorer& operator++(int) { return *this; }
};

#endif
