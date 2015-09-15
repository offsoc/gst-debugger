/*
 * lexer.h
 *
 *  Created on: Sep 3, 2015
 *      Author: loganek
 */

#ifndef SRC_GST_DEBUGGER_FILTER_PARSER_LEXER_H_
#define SRC_GST_DEBUGGER_FILTER_PARSER_LEXER_H_

#include "tokens.h"

#include <vector>
#include <memory>

class Lexer
{
	std::vector<std::shared_ptr<TokenBase>> tokens;
	int pos;
	std::string src_text;

	void read_number();
	void read_operator();
	void read_string();
	void read_identifier();

public:
	virtual ~Lexer() {}
	void tokenize(const std::string &src);

	decltype(tokens) get_tokens() const { return tokens; }
};

#endif /* SRC_GST_DEBUGGER_FILTER_PARSER_LEXER_H_ */
