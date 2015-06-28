/*
 * gvalue_boolean.h
 *
 *  Created on: Jun 28, 2015
 *      Author: mkolny
 */

#ifndef SRC_GST_DEBUGGER_GVALUE_CONVERTER_GVALUE_BOOLEAN_H_
#define SRC_GST_DEBUGGER_GVALUE_CONVERTER_GVALUE_BOOLEAN_H_

#include "gvalue_base.h"

class GValueBoolean : public GValueBase
{
public:
	GValueBoolean(GValue* gobj);

	std::string to_string() const override;
};

#endif /* SRC_GST_DEBUGGER_GVALUE_CONVERTER_GVALUE_BOOLEAN_H_ */
