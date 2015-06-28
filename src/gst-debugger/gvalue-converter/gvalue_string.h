/*
 * gvalue_string.h
 *
 *  Created on: Jun 28, 2015
 *      Author: mkolny
 */

#ifndef SRC_GST_DEBUGGER_GVALUE_CONVERTER_GVALUE_STRING_H_
#define SRC_GST_DEBUGGER_GVALUE_CONVERTER_GVALUE_STRING_H_

#include "gvalue_base.h"

class GValueString : public GValueBase
{
public:
	GValueString(GValue* gobj);

	std::string to_string() const override;
};

#endif /* SRC_GST_DEBUGGER_GVALUE_CONVERTER_GVALUE_STRING_H_ */
