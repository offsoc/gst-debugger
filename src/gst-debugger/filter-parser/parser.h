/*
 * parser.h
 *
 *  Created on: Sep 13, 2015
 *      Author: loganek
 */

#ifndef SRC_GST_DEBUGGER_FILTER_PARSER_PARSER_H_
#define SRC_GST_DEBUGGER_FILTER_PARSER_PARSER_H_

#include "lexer.h"

#include <stack>

struct Expression
{
	const std::shared_ptr<TokenOperator> op;
	const std::shared_ptr<TokenBase> left;
	const std::shared_ptr<TokenBase> right;

	Expression(const std::shared_ptr<TokenOperator> &op,
			const std::shared_ptr<TokenBase> &left,
			const std::shared_ptr<TokenBase> &right)
	: op(op), left(left), right(right)
	{}

	virtual ~Expression() {}
};

class Parser
{
	void check_token(const std::shared_ptr<TokenBase> &token, TokenType expected_type)
	{
		if ((token->get_type() & expected_type) == TokenType::INVALID)
		{
			throw std::runtime_error("expected token type " + std::to_string(static_cast<int>(expected_type)) +
					" (" + token->to_string() + ")");
		}
	}

public:
	virtual ~Parser() {}

	Expression parse(const std::string &str)
	{
		Lexer lexer;
		lexer.tokenize(str);
		auto tokens = lexer.get_tokens();

		if (tokens.size() != 3)
		{
			throw std::runtime_error("invalid number of tokens (3 expected)");
		}

		check_token(tokens[0], TokenType::LITERAL | TokenType::IDENTIFIER);
		check_token(tokens[1], TokenType::OPERATOR);
		check_token(tokens[2], TokenType::LITERAL | TokenType::IDENTIFIER);

		return Expression(std::static_pointer_cast<TokenOperator>(tokens[1]), tokens[0], tokens[2]);
	}
};

#endif /* SRC_GST_DEBUGGER_FILTER_PARSER_PARSER_H_ */
