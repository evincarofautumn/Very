/**
 * @file parse_iterator.h
 */
#ifndef PARSE_ITERATOR_H
#define PARSE_ITERATOR_H
#include "Term.h"

/**
 * A forward iterator to convert a token sequence to terms.
 * @tparam I Input iterator type.
 */
template<class I>
class parse_iterator : public
	std::iterator<std::forward_iterator_tag, std::shared_ptr<Term>> {
	mutable std::shared_ptr<I> source;
	I end;
public:
	parse_iterator();
	parse_iterator(I, I = I());
	std::shared_ptr<Term> operator*() const;
	bool operator==(const parse_iterator&) const;
	bool operator!=(const parse_iterator&) const;
	parse_iterator& operator++();
	parse_iterator operator++(int);
};

/**
 * Constructs a past-the-end parse_iterator.
 */
template<class I>
parse_iterator<I>::parse_iterator() : source(new I()) {}

/**
 * Constructs a parse_iterator from an input iterator range.
 * @param first Start of range.
 * @param last  End of range.
 */
template<class I>
parse_iterator<I>::parse_iterator(I first, I last)
	: source(new I(first)), end(last) {}

/**
 * Yields the next Term from the token stream.
 */
template<class I>
std::shared_ptr<Term> parse_iterator<I>::operator*() const {
	std::shared_ptr<Term> term;
	if (**source == "(") {
		term.reset(new Term());
		parse_iterator<I> iterator(*this);
		++*source;
		while (*source != end && **source != ")")
			term->values.push_back(*iterator++);
		if (*source == end)
			throw std::runtime_error("Expected ) before EOF.");
		++*source;
		return term;
	}
	term.reset(new Term(**source));
	++*source;
	return term;
}

/**
 * Compares parse_iterator for equality.
 */
template<class I>
bool parse_iterator<I>::operator==(const parse_iterator<I>& other) const {
	return *source == *other.source;
}

/**
 * Compares parse_iterator for inequality.
 */
template<class I>
bool parse_iterator<I>::operator!=(const parse_iterator<I>& other) const {
	return !(*this == other);
}

/**
 * No-op provided to satisfy iterator interface.
 */
template<class I>
parse_iterator<I>& parse_iterator<I>::operator++() {return *this; }

/**
 * No-op provided to satisfy iterator interface.
 */
template<class I>
parse_iterator<I> parse_iterator<I>::operator++(int) { return *this; }

#endif
