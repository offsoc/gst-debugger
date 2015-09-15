/*
 * tokens.cpp
 *
 *  Created on: Sep 13, 2015
 *      Author: loganek
 */

#include "tokens.h"

#include <stdexcept>
#include <algorithm>

TokenNumber::TokenNumber(const std::string &number)
 : Token(TokenType::NUMBER_LITERAL)
{
	value = atoi(number.c_str());
}

TokenString::TokenString(const std::string &str)
 : Token(TokenType::STRING_LITERAL)
{
	value = str;
}

bool TokenString::is_quote(char c)
{
	return c == '"' || c == '\'';
}

TokenIdentifier::TokenIdentifier(const std::string &identifier)
 : Token(TokenType::IDENTIFIER)
{
	value = identifier;
}

bool TokenIdentifier::is_id_start(char c)
{
	return isalpha(c);
}

TokenOperator::TokenOperator(const std::string &op)
 : Token(TokenType::OPERATOR)
{
	if (op == "==") value = OpType::EQ;
	else if (op == "!=") value = OpType::NEQ;
	else if (op == "||") value = OpType::OR;
	else if (op == "&&") value = OpType::AND;
	else throw std::runtime_error ("invalid operator " + op);
}

bool TokenOperator::is_part_of_op(char c)
{
	auto v = {'=', '!', '&', '|'};
	return std::find(v.begin(), v.end(), c) != v.end();
}

std::string TokenOperator::to_string() const
{
	switch (value)
	{
	case OpType::AND: return "&&";
	case OpType::OR: return "||";
	case OpType::EQ: return "==";
	case OpType::NEQ: return "!=";
	}
	throw std::runtime_error("unknown operator");
}
