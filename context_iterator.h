#ifndef CONTEXT_ITERATOR_H
#define CONTEXT_ITERATOR_H
#include "Context.h"

template<class I>
class context_iterator
	: public std::iterator<std::forward_iterator_tag, Context> {
	Context context;
	I here;
	I end;
public:
	context_iterator() {}
	context_iterator(I first, I last = I()) : here(first), end(last) {}
	context_iterator& operator*() {
		(**here)(context);
		return *this;
	}
	context_iterator& operator++() {
		++here;
		return *this;
	}
	context_iterator operator++(int) {
		context_iterator temp(*this);
		++*this;
		return temp;
	}
	bool operator==(const context_iterator& other) {
		return here == other.here;
	}
	bool operator!=(const context_iterator& other) {
		return !(*this == other);
	}
};

#endif
