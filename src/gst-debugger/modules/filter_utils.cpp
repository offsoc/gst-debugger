/*
 * filter_utils.cpp
 *
 *  Created on: Sep 15, 2015
 *      Author: loganek
 */

#include "filter_utils.h"

#include <cstring>

bool filter_structure(const GstStructure *structure, const gchar *field_name, const std::shared_ptr<TokenBase> &value)
{
	if (structure == nullptr)
		return true;

	if (!gst_structure_has_field(structure, field_name))
		return false;

	GType field_type = gst_structure_get_field_type(structure, field_name);

	if (field_type == G_TYPE_STRING)
		return strcmp(gst_structure_get_string(structure, field_name), std::static_pointer_cast<TokenString>(value)->get_value().c_str()) == 0;
#define MAKE_FIELD_FILTER(GTYPE, CTYPE) \
	if (field_type == GTYPE) { \
		g ## CTYPE v; gst_structure_get_ ## CTYPE(structure, field_name, &v); \
		return (long long)v == std::static_pointer_cast<TokenNumber>(value)->get_value(); \
	}

	MAKE_FIELD_FILTER(G_TYPE_INT, int)
	MAKE_FIELD_FILTER(G_TYPE_UINT, uint)
	MAKE_FIELD_FILTER(G_TYPE_INT64, int64)
	MAKE_FIELD_FILTER(G_TYPE_UINT64, uint64)
	MAKE_FIELD_FILTER(G_TYPE_BOOLEAN, boolean)

	return true;
}

void read_tokens_by_type(const std::shared_ptr<Expression> &filter_expression, std::shared_ptr<TokenIdentifier> &ident, std::shared_ptr<TokenBase> &value)
{
	if (filter_expression->left->get_type() == TokenType::IDENTIFIER)
	{
		ident = std::static_pointer_cast<TokenIdentifier>(filter_expression->left);
		value = filter_expression->right;
	}
	else
	{
		ident = std::static_pointer_cast<TokenIdentifier>(filter_expression->right);
		value = filter_expression->left;
	}
}
