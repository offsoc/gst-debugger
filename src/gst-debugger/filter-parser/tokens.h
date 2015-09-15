/*
 * tokens.h
 *
 *  Created on: Sep 13, 2015
 *      Author: loganek
 */

#ifndef SRC_GST_DEBUGGER_FILTER_PARSER_TOKENS_H_
#define SRC_GST_DEBUGGER_FILTER_PARSER_TOKENS_H_

#include <string>

enum class TokenType : int
{
	INVALID = 0x00,
	IDENTIFIER = 0x01,
	OPERATOR = 0x02,
	LITERAL = 0x04,
	NUMBER_LITERAL = 0x05,
	STRING_LITERAL = 0x06,
};

inline TokenType operator|(const TokenType &t1, const TokenType &t2)
{
	return static_cast<TokenType>(static_cast<int>(t1) | static_cast<int>(t2));
}

inline TokenType operator&(const TokenType &t1, const TokenType &t2)
{
	return static_cast<TokenType>(static_cast<int>(t1) & static_cast<int>(t2));
}

class TokenBase
{
	TokenType type;

public:
	virtual ~TokenBase() {}
	TokenBase(TokenType type) : type(type) {}

	virtual std::string to_string() const = 0;

	TokenType get_type() const { return type; }
};

template<typename T>
class Token : public TokenBase
{
protected:
	T value;

public:
	virtual ~Token() {}
	Token(TokenType type) : TokenBase(type) {}

	T get_value() const { return value; }
};

class TokenNumber : public Token<long long>
{
public:
	TokenNumber(const std::string &number);

	std::string to_string() const override
	{
		return std::to_string(value);
	}

};

class TokenString : public Token<std::string>
{
public:
	TokenString(const std::string &str);

	std::string to_string() const override { return value; }

	static bool is_quote(char c);
};

class TokenIdentifier : public Token<std::string>
{
public:
	TokenIdentifier(const std::string &identifier);

	std::string to_string() const override { return value; }

	static bool is_id_start(char c);
};

enum class OpType
{
	EQ,
	NEQ,
	OR,
	AND
};

class TokenOperator : public Token<OpType>
{
public:
	TokenOperator(const std::string &op);

	std::string to_string() const override;

	static bool is_part_of_op(char c);
};

#endif /* SRC_GST_DEBUGGER_FILTER_PARSER_TOKENS_H_ */
