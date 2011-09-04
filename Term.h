#ifndef TERM_H
#define TERM_H
#include <map>
#include <memory>
#include <string>
#include <vector>

class Context;

/**
 * A term in an expression.
 */
class Term : public std::enable_shared_from_this<Term> {
	static std::map<std::string, int32_t> operations;
public:
	enum Extra {
		DEF,
		SYMBOL,
		ARRAY,
		DUP,
		POP,
		SWAP,
		QUOTE,
		APPLY,
		COMPOSE,
		WRITE,
		PUTC,
		ADD,
		SUB,
		MUL,
		DIV,
		MOD,
		LT,
		GT,
		LE,
		GE,
		EQ,
		NE,
		COND
	};
	enum Type { SCALAR, SPECIAL } type;
	int32_t tag;
	std::vector<std::shared_ptr<Term>> values;
	Term();
	Term(int32_t);
	Term(const std::string&);
	void operator()(Context&);
	void apply(Context&);
	Term& operator+=(const Term&);
	Term& operator-=(const Term&);
	Term& operator*=(const Term&);
	Term& operator/=(const Term&);
	Term& operator%=(const Term&);
	friend Term operator+(Term, const Term&);
	friend Term operator-(Term, const Term&);
	friend Term operator*(Term, const Term&);
	friend Term operator/(Term, const Term&);
	friend Term operator%(Term, const Term&);
	friend bool operator<(const Term&, const Term&);
	friend bool operator==(const Term&, const Term&);
	friend std::ostream& operator<<(std::ostream&, const Term&);
private:
	bool is_value() const;
	bool is_scalar() const;
};

#endif
