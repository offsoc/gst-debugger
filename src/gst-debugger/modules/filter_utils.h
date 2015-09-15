/*
 * filter_utils.h
 *
 *  Created on: Sep 15, 2015
 *      Author: loganek
 */

#ifndef SRC_GST_DEBUGGER_MODULES_FILTER_UTILS_H_
#define SRC_GST_DEBUGGER_MODULES_FILTER_UTILS_H_

#include "filter-parser/parser.h"

#include <gst/gst.h>

#include <memory>

bool filter_structure(const GstStructure *structure, const gchar *field_name, const std::shared_ptr<TokenBase> &value);

void read_tokens_by_type(const std::shared_ptr<Expression> &filter_expression, std::shared_ptr<TokenIdentifier> &ident, std::shared_ptr<TokenBase> &value);

#endif /* SRC_GST_DEBUGGER_MODULES_FILTER_UTILS_H_ */
