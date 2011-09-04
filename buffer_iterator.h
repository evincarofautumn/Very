#ifndef BUFFER_ITERATOR_H
#define BUFFER_ITERATOR_H
#include <iterator>
#include <memory>
#include <vector>

/**
 * Adapts an input iterator to a bidirectional iterator by buffering input.
 */
template<class I>
class buffer_iterator : public
	std::iterator<std::bidirectional_iterator_tag, typename I::value_type> {
	typedef std::vector<typename I::value_type> buffer_type;
	mutable std::shared_ptr<buffer_type> buffer;
	typename buffer_type::size_type index;
	static const typename buffer_type::size_type npos = -1;
	mutable std::shared_ptr<I> source;
	I end;
public:
	buffer_iterator();
	buffer_iterator(I, I = I());
	typename I::value_type operator*() const;
	bool operator==(const buffer_iterator&) const;
	bool operator!=(const buffer_iterator&) const;
	buffer_iterator& operator++();
	buffer_iterator operator++(int);
	buffer_iterator& operator--();
	buffer_iterator operator--(int);
private:
	bool at_end() const;
};

template<class I>
buffer_iterator<I>::buffer_iterator() : index(-1), source(new I()) {}

template<class I>
buffer_iterator<I>::buffer_iterator(I first, I last)
	: buffer(new buffer_type()), index(0), source(new I(first)), end(last) {}

template<class I>
typename I::value_type buffer_iterator<I>::operator*() const {
	while (index >= buffer->size())
		buffer->push_back(*(*source)++);
	return (*buffer)[index];
}

template<class I>
bool buffer_iterator<I>::operator==(const buffer_iterator<I>& other) const {
	return *source == *other.source
		&& ((at_end() && other.at_end()) || index == other.index);
}

template<class I>
bool buffer_iterator<I>::operator!=(const buffer_iterator<I>& other) const {
	return !(*this == other);
}

template<class I>
buffer_iterator<I>& buffer_iterator<I>::operator++() {
	++index;
	return *this;
}

template<class I>
buffer_iterator<I> buffer_iterator<I>::operator++(int) {
	buffer_iterator<I> temp(*this);
	++*this;
	return temp;
}

template<class I>
buffer_iterator<I>& buffer_iterator<I>::operator--() {
	--index;
	return *this;
}

template<class I>
buffer_iterator<I> buffer_iterator<I>::operator--(int) {
	buffer_iterator<I> temp(*this);
	--*this;
	return temp;
}

template<class I>
bool buffer_iterator<I>::at_end() const {
	return index == npos || (index == buffer->size() && *source == end);
}

#endif
