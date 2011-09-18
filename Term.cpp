/**
 * @file Term.cpp
 */
#include "Term.h"
#include "Context.h"
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <utility>
#include <utf8.h>

using namespace std::rel_ops;

/// Map of builtin operations to their names.
std::map<std::string, int32_t> Term::operations {
	{ "_def",    DEF },
	{ "dup",     DUP },
	{ "pop",     POP },
	{ "swap",    SWAP },
	{ "quote",   QUOTE },
	{ "apply",   APPLY },
	{ "compose", COMPOSE },
	{ "write",   WRITE },
	{ "putc",    PUTC },
	{ "+",       ADD },
	{ "-",       SUB },
	{ "*",       MUL },
	{ "/",       DIV },
	{ "%",       MOD },
	{ "lt?",     LT },
	{ "gt?",     GT },
	{ "le?",     LE },
	{ "ge?",     GE },
	{ "eq?",     EQ },
	{ "ne?",     NE },
	{ "cond",    COND }
};

/**
 * Constructs an empty array Term.
 */
Term::Term() : type(SPECIAL), tag(ARRAY) {}

/**
 * Constructs a scalar Term.
 * @param value Scalar value.
 */
Term::Term(int32_t value) : type(SCALAR), tag(value) {}

/**
 * Constructs a Term from the given token string.
 */
Term::Term(const std::string& token) {
	auto operation = operations.find(token);
	if (operation != operations.end()) {
		type = SPECIAL;
		tag = operation->second;
	} else if (::isdigit(token[0]) || token[0] == '-' || token[0] == '+') {
		std::istringstream stream(token);
		type = SCALAR;
		stream >> tag;
	} else if (token[0] == '"') {
		type = SPECIAL;
		tag = ARRAY;
		auto i = token.begin() + 1;
		while (i != token.end())
			values.push_back(std::make_shared<Term>(utf8::unchecked::next(i)));
	} else {
		type = SPECIAL;
		tag = SYMBOL;
		auto i = token.begin();
		while (i != token.end())
			values.push_back(std::make_shared<Term>(utf8::unchecked::next(i)));
	}
}

/**
 * Evaluates a Term.
 * @param context Evaluation context.
 */
void Term::operator()(Context& context) {
	if (is_value()) {
		context.push(shared_from_this());
		return;
	}
	switch (tag) {
	case DEF:
		{
			auto name = context.pop();
			auto body = context.pop();
			context.define_word(name, body);
			break;
		}
	case SYMBOL:
		{
			context.get_word(shared_from_this())->apply(context);
			break;
		}
	case DUP:
		context.push(context.top());
		break;
	case POP:
		context.pop();
		break;
	case SWAP:
		{
			auto a = context.pop();
			auto b = context.pop();
			context.push(a);
			context.push(b);
			break;
		}
	case QUOTE:
		{
			auto result = std::make_shared<Term>();
			result->values.push_back(context.pop());
			context.push(result);
			break;
		}
	case APPLY:
		context.pop()->apply(context);
		break;
	case WRITE:
		std::cout << *context.pop() << '\n';
		break;
	case PUTC:
		{
			auto port = context.pop();
			auto character = context.pop();
			if (character->is_scalar() && port->is_scalar()) {
				utf8::utf32to8(&character->tag, &character->tag + 1,
					std::ostreambuf_iterator<char>
					(context.get_output_port(port->tag)));
			} else {
				std::ostringstream message;
				message << "putc does not understand arrays in:\n"
					<< *character << " " << *port << " putc";
				throw std::runtime_error(message.str());
			}
			break;
		}
#define OPERATOR_TERM(id, symbol)                               \
	case id:                                                    \
		{                                                       \
			auto b = context.pop();                             \
			auto a = context.pop();                             \
			context.push(std::make_shared<Term>(*a symbol *b)); \
			break;                                              \
		}
	OPERATOR_TERM(COMPOSE, +)
	OPERATOR_TERM(ADD, +)
	OPERATOR_TERM(SUB, -)
	OPERATOR_TERM(MUL, *)
	OPERATOR_TERM(DIV, /)
	OPERATOR_TERM(MOD, %)
	OPERATOR_TERM(LT, <)
	OPERATOR_TERM(GT, >)
	OPERATOR_TERM(LE, <=)
	OPERATOR_TERM(GE, >=)
	OPERATOR_TERM(EQ, ==)
	OPERATOR_TERM(NE, !=)
#undef OPERATOR_TERM
	case COND:
		{
			auto else_body = context.pop();
			auto then_body = context.pop();
			auto test = context.pop();
			(*test != Term(0) ? then_body : else_body)->apply(context);
			break;
		}
	}
}

/**
 * Applies a Term to the stack.
 * @param context Evaluation context.
 */
void Term::apply(Context& context) {
	if (is_scalar()) {
		(*this)(context);
	} else {
		for (auto i = values.begin(); i != values.end(); ++i)
			(**i)(context);
	}
}

/**
 * Adds Terms.
 */
Term& Term::operator+=(const Term& other) {
	if (is_scalar()) {
		if (other.is_scalar()) {
			tag += other.tag;
		} else {
			values.push_back(std::make_shared<Term>(tag));
			tag = 0;
			std::copy(other.values.begin(), other.values.end(),
				std::back_inserter(values));
		}
	} else {
		if (other.is_scalar()) {
			values.push_back(std::make_shared<Term>(other.tag));
		} else {
			std::copy(other.values.begin(), other.values.end(),
				std::back_inserter(values));
		}
	}
	return *this;
}

/**
 * Subtracts Terms.
 */
Term& Term::operator-=(const Term& other) {
	if (is_scalar() && other.is_scalar())
		tag -= other.tag;
	else
		throw std::runtime_error("- cannot be applied to sequences.");
	return *this;
}

/**
 * Multiplies Terms.
 */
Term& Term::operator*=(const Term& other) {
	if (is_scalar()) {
		if (other.is_scalar()) {
			tag *= other.tag;
		} else {
			if (tag < 0)
				throw std::runtime_error("* can't repeat negative times.");
			for (int32_t i = 0; i < tag; ++i)
				std::copy(other.values.begin(), other.values.end(),
					std::back_inserter(values));
			tag = 0;
		}
	} else {
		if (other.is_scalar()) {
			if (other.tag < 0)
				throw std::runtime_error("* can't repeat negative times.");
			auto size = values.size();
			for (int32_t i = 0; i < other.tag - 1; ++i)
				std::copy(values.begin(), values.begin() + size,
					std::back_inserter(values));
		} else {
			throw std::runtime_error
				("* cannot be applied to two sequences.");
		}
	}
	return *this;
}

/**
 * Divides Terms.
 */
Term& Term::operator/=(const Term& other) {
	if (is_scalar() && other.is_scalar())
		tag /= other.tag;
	else
		throw std::runtime_error("/ cannot be applied to sequences.");
	return *this;
}

/**
 * Modulates Terms.
 */
Term& Term::operator%=(const Term& other) {
	if (is_scalar() && other.is_scalar())
		tag %= other.tag;
	else
		throw std::runtime_error("% cannot be applied to sequences.");
	return *this;
}

Term operator+(Term a, const Term& b) { return a += b; }
Term operator-(Term a, const Term& b) { return a -= b; }
Term operator*(Term a, const Term& b) { return a *= b; }
Term operator/(Term a, const Term& b) { return a /= b; }
Term operator%(Term a, const Term& b) { return a %= b; }

/**
 * Sorts Terms: scalars directly, arrays recursively (and lexicographically).
 */
bool operator<(const Term& a, const Term& b) {
	if (a.is_scalar() && b.is_scalar()) {
		return a.tag < b.tag;
	} else if (!a.is_scalar() && !b.is_scalar()) {
		auto i = a.values.begin(), j = b.values.begin();
		while (i != a.values.end()) {
			if (j == b.values.end() || **j < **i)
				return false;
			if (**i < **j)
				return true;
			i++;
			j++;
		}
		return (j != b.values.end());
	} else {
		return a.is_scalar() && !b.is_scalar();
	}
}

/**
 * Tests equality of Terms: scalars directly, arrays recursively.
 */
bool operator==(const Term& a, const Term& b) {
	if (a.is_scalar()) {
		if (b.is_scalar())
			return a.tag == b.tag;
		return false;
	} else if (b.is_scalar()) {
		return false;
	} else {
		if (a.values.size() != b.values.size())
			return false;
		for (auto i = a.values.begin(), j = b.values.begin();
			i != a.values.end(); ++i, ++j)
			if (**i != **j)
				return false;
		return true;
	}
}

/**
 * Writes a Term to a stream.
 */
std::ostream& operator<<(std::ostream& stream, const Term& term) {
	if (term.is_value()) {
		if (term.is_scalar()) {
			return stream << term.tag;
		} else {
			stream << "( ";
			for (auto i = term.values.begin(); i != term.values.end(); ++i)
				stream << **i << " ";
			return stream << ")";
		}
	} else {
		for (auto i = Term::operations.begin();
			i != Term::operations.end(); ++i)
			if (term.tag == i->second)
				return stream << i->first;
		std::ostreambuf_iterator<char> write(stream);
		for (auto i = term.values.begin(); i != term.values.end(); ++i)
			utf8::append((*i)->tag, write);
	}
	return stream;
}

/**
 * Tests whether a Term represents a value.
 */
bool Term::is_value() const {
	return type == SCALAR
		|| (type == SPECIAL && tag == ARRAY);
}

/**
 * Tests whether a Term is a scalar value.
 */
bool Term::is_scalar() const {
	return type == SCALAR;
}
