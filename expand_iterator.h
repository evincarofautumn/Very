#ifndef EXPAND_ITERATOR_H
#define EXPAND_ITERATOR_H

template<class I>
class expand_iterator
	: public std::iterator<std::forward_iterator_tag, typename I::value_type> {
	I here;
	I end;
public:
	expand_iterator() {}
	expand_iterator(I first, I last = I()) : here(first), end(last) {}
	typename I::value_type operator*() const {
		return *here;
	}
	expand_iterator& operator++() {
		++here;
		return *this;
	}
	expand_iterator operator++(int) {
		expand_iterator previous(*this);
		++*this;
		return previous;
	}
	bool operator==(const expand_iterator& other) const {
		return here == other.here;
	}
	bool operator!=(const expand_iterator& other) const {
		return !(*this == other);
	}
};

#endif
