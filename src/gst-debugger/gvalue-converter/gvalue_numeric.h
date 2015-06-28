/*
 * gvalue_numeric.h
 *
 *  Created on: Jun 28, 2015
 *      Author: mkolny
 */

#ifndef SRC_GST_DEBUGGER_GVALUE_CONVERTER_GVALUE_NUMERIC_H_
#define SRC_GST_DEBUGGER_GVALUE_CONVERTER_GVALUE_NUMERIC_H_

#include "gvalue_base.h"

template<typename T>
class GValueNumeric : public GValueBase
{
public:
	GValueNumeric(GValue* gobj);

	std::string to_string() const override;
};


#endif /* SRC_GST_DEBUGGER_GVALUE_CONVERTER_GVALUE_NUMERIC_H_ */
