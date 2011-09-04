/**
 * @file buffer_iterator.h
 */
#ifndef BUFFER_ITERATOR_H
#define BUFFER_ITERATOR_H
#include <iterator>
#include <memory>
#include <vector>

/**
 * Adapts an input iterator to a bidirectional iterator by buffering input.
 * @tparam I Input iterator type.
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

/**
 * Constructs a past-the-end buffer_iterator.
 */
template<class I>
buffer_iterator<I>::buffer_iterator() : index(-1), source(new I()) {}

/**
 * Constructs a buffer_iterator from a range.
 * @param first Start of range.
 * @param last  End of range.
 */
template<class I>
buffer_iterator<I>::buffer_iterator(I first, I last)
	: buffer(new buffer_type()), index(0), source(new I(first)), end(last) {}

/**
 * Returns the current element, filling the buffer as needed.
 */
template<class I>
typename I::value_type buffer_iterator<I>::operator*() const {
	while (index >= buffer->size())
		buffer->push_back(*(*source)++);
	return (*buffer)[index];
}

/**
 * Compares buffer_iterator for equality.
 */
template<class I>
bool buffer_iterator<I>::operator==(const buffer_iterator<I>& other) const {
	return *source == *other.source
		&& ((at_end() && other.at_end()) || index == other.index);
}

/**
 * Compares buffer_iterator for inequality.
 */
template<class I>
bool buffer_iterator<I>::operator!=(const buffer_iterator<I>& other) const {
	return !(*this == other);
}

/**
 * Seeks to the next element.
 */
template<class I>
buffer_iterator<I>& buffer_iterator<I>::operator++() {
	++index;
	return *this;
}

/**
 * Post-increment to satisfy iterator interface.
 */
template<class I>
buffer_iterator<I> buffer_iterator<I>::operator++(int) {
	buffer_iterator<I> temp(*this);
	++*this;
	return temp;
}

/**
 * Seeks to the previous element.
 */
template<class I>
buffer_iterator<I>& buffer_iterator<I>::operator--() {
	--index;
	return *this;
}

/**
 * Post-decrement to satisfy iterator interface.
 */
template<class I>
buffer_iterator<I> buffer_iterator<I>::operator--(int) {
	buffer_iterator<I> temp(*this);
	--*this;
	return temp;
}

/**
 * At-the-end test. Returns true if the buffer index is @c npos or the input
 * source has been exhausted.
 */
template<class I>
bool buffer_iterator<I>::at_end() const {
	return index == npos || (index == buffer->size() && *source == end);
}

#endif
