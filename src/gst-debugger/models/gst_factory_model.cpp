/*
 * gst_factory_model.cpp
 *
 *  Created on: Aug 13, 2015
 *      Author: loganek
 */

#include "gst_factory_model.h"

#include <cassert>

void FactoryModel::append_template(const Glib::RefPtr<Gst::PadTemplate> &tpl)
{
	auto it = std::find_if(pad_templates.begin(), pad_templates.end(), [tpl](const Glib::RefPtr<Gst::PadTemplate> &p_tpl) {
		return p_tpl->get_name_template() == tpl->get_name_template();
	});

	if (it == pad_templates.end())
	{
		pad_templates.push_back(tpl);
	}
	else
	{
		*it = tpl;
	}
}
