/*
 * lexer.cpp
 *
 *  Created on: Sep 3, 2015
 *      Author: loganek
 */

#include "lexer.h"

#include <algorithm>

enum class State
{
	NUMBER,
	STRING,
	VARIABLE,
	OPERATOR
};

void Lexer::read_number()
{
	int tmp = pos--;
	while (++pos < src_text.length() && isdigit(src_text[pos]));
	tokens.push_back(std::make_shared<TokenNumber>(std::string(src_text.begin() + tmp, src_text.begin() + pos--)));
}

void Lexer::read_operator()
{
	int tmp = pos--;
	while (++pos < src_text.length() && TokenOperator::is_part_of_op(src_text[pos]));
	tokens.push_back(std::make_shared<TokenOperator>(std::string(src_text.begin() + tmp, src_text.begin() + pos--)));
}

void Lexer::read_string()
{
	char quote = src_text[pos];
	bool prev_esc = false;
	std::string val;
	char c = -1;
	while (pos < src_text.length())
	{
		c = src_text[++pos];
		if (prev_esc)
		{
			if (TokenString::is_quote(c) || c == '\\')
			{
				val += c;
				prev_esc = false;
			}
			else
			{
				throw std::runtime_error("unknown escape sequence: '" + std::string("\\") + c + "'");
			}
		}
		else if (c == '\\')
		{
			prev_esc = true;
		}
		else if (c == quote)
		{
			break;
		}
		else
		{
			val += c;
		}
	}

	if (prev_esc)
	{
		throw std::runtime_error("incomplete escape sequence");
	}
	if (c != quote)
	{
		throw std::runtime_error("missing terminating " + std::string{quote} + " character");
	}

	tokens.push_back(std::make_shared<TokenString>(val));
}

void Lexer::read_identifier()
{
	int tmp = pos;

	while (!isspace(src_text[pos]) &&
			!TokenString::is_quote(src_text[pos]) &&
			!TokenOperator::is_part_of_op(src_text[pos]) &&
			pos < src_text.length())
	{
		pos++;
	}

	tokens.push_back(std::make_shared<TokenIdentifier>(std::string(src_text.begin() + tmp, src_text.begin() + pos--)));
}

void Lexer::tokenize(const std::string &src)
{
	src_text = src;

	for (pos = 0; pos < src_text.length(); pos++)
	{
		char c = src_text[pos];

		if (isdigit(c))
			read_number();
		else if (TokenOperator::is_part_of_op(c))
			read_operator();
		else if (TokenString::is_quote(c))
			read_string();
		else if (TokenIdentifier::is_id_start(c))
			read_identifier();
	}
}
