/*
 * expression.h
 *
 *  Created on: Sep 3, 2015
 *      Author: loganek
 */

#ifndef SRC_GST_DEBUGGER_FILTER_PARSER_EXPRESSION_H_
#define SRC_GST_DEBUGGER_FILTER_PARSER_EXPRESSION_H_

#include <stdexcept>

enum class ExprOperator
{
	EQ,		// ==
	NEQ,	// !=
	GE,		// >=
	LE,		// <=
	G,		// >
	L		// <
};

template<typename T>
class Expression
{
	T left;
	T right;
	ExprOperator op;

public:
	virtual ~Expression() {}

	bool evaluate() const;
};

template<typename T>
bool Expression<T>::evaluate() const
{
#define APPLY_OPERATOR(OP_ENUM, OP_REAL) \
	case ExprOperator::OP_ENUM: return left OP_REAL right;
	switch (op)
	{
	APPLY_OPERATOR(EQ, ==)
	APPLY_OPERATOR(NEQ, !=)
	APPLY_OPERATOR(GE, >=)
	APPLY_OPERATOR(LE, <=)
	APPLY_OPERATOR(G, >)
	APPLY_OPERATOR(L, <)
	}

	throw std::runtime_error("unknown operator");
}


#endif /* SRC_GST_DEBUGGER_FILTER_PARSER_EXPRESSION_H_ */
