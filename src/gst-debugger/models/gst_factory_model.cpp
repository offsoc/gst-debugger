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

void GstFactoryContainer::update_factory(const FactoryModel &factory)
{
	auto it = get_factory_it(factory.get_name());
	if (it == factories.end())
	{
		factories.push_back(factory);
	}
	else
	{
		*it = factory;
	}
}

std::vector<FactoryModel>::iterator GstFactoryContainer::get_factory_it(const std::string &factory_name)
{
	std::vector<FactoryModel>::iterator it = std::find_if(factories.begin(), factories.end(), [factory_name] (const FactoryModel& t) {
		return t.get_name() == factory_name;
	});

	return it;
}

bool GstFactoryContainer::has_factory(const std::string &factory_name)
{
	return get_factory_it(factory_name) != factories.end();
}

FactoryModel GstFactoryContainer::get_factory(const std::string &factory_name)
{
	auto factory_it = get_factory_it(factory_name);
	assert(factory_it != factories.end());
	return *factory_it;
}
