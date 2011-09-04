#ifndef TERM_ITERATOR_H
#define TERM_ITERATOR_H
#include "Term.h"

/**
 * A forward iterator to convert a token sequence to terms.
 */
template<class I>
class term_iterator : public
	std::iterator<std::forward_iterator_tag, std::shared_ptr<Term>> {
	mutable std::shared_ptr<I> source;
	I end;
public:
	term_iterator();
	term_iterator(I, I = I());
	std::shared_ptr<Term> operator*() const;
	bool operator==(const term_iterator&);
	bool operator!=(const term_iterator&);
	term_iterator& operator++();
	term_iterator operator++(int);
};

template<class I>
term_iterator<I>::term_iterator() : source(new I()) {}

template<class I>
term_iterator<I>::term_iterator(I first, I last)
	: source(new I(first)), end(last) {}

template<class I>
std::shared_ptr<Term> term_iterator<I>::operator*() const {
	std::shared_ptr<Term> term;
	if (**source == "(") {
		term.reset(new Term());
		term_iterator<I> iterator(*this);
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

template<class I>
bool term_iterator<I>::operator==(const term_iterator<I>& other) {
	return *source == *other.source;
}

template<class I>
bool term_iterator<I>::operator!=(const term_iterator<I>& other) {
	return !(*this == other);
}

template<class I>
term_iterator<I>& term_iterator<I>::operator++() {return *this; }

template<class I>
term_iterator<I> term_iterator<I>::operator++(int) { return *this; }

#endif
