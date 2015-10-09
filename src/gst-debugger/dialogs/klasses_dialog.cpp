/*
 * klasses_dialog.cpp
 *
 *  Created on: Oct 1, 2015
 *      Author: loganek
 */

#include "klasses_dialog.h"
#include "ui_utils.h"

#include "controller/controller.h"

#include <glibmm/i18n.h>

#include <map>

KlassesDialog::KlassesDialog(BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& builder)
: RemoteDataDialog(cobject, builder)
{
	tree_model = Gtk::TreeStore::create(klasses_columns);
	data_tree_view->set_model(tree_model);
	data_tree_view->append_column(_("Property"), klasses_columns.m_col_name);
	data_tree_view->append_column(_("Value"), klasses_columns.m_col_value);

	set_title("Remote Factories");
}

void KlassesDialog::set_controller(const std::shared_ptr<Controller> &controller)
{
	IBaseView::set_controller(controller);
	controller->on_klass_list_changed.connect(sigc::mem_fun(*this, &KlassesDialog::reload_list));
	reload_list("", true);
}

#define APPEND_SUB_ROW(PARENT, NAME, VALUE) \
	do { \
		auto c = *(tree_model->append(PARENT.children())); \
		c[klasses_columns.m_col_name] = NAME; \
		c[klasses_columns.m_col_value] = VALUE; \
	} while (false)

std::string KlassesDialog::g_param_flags_to_string(GParamFlags v)
{
#define xstr(s) str(s)
#define str(s) #s
#define PARAM_FLAG(NAME) { NAME, xstr(NAME) }
	std::map<int, std::string> values = {
			PARAM_FLAG(G_PARAM_READABLE),
			PARAM_FLAG(G_PARAM_WRITABLE),
			PARAM_FLAG(G_PARAM_CONSTRUCT),
			PARAM_FLAG(G_PARAM_CONSTRUCT_ONLY),
			PARAM_FLAG(G_PARAM_LAX_VALIDATION),
			PARAM_FLAG(G_PARAM_STATIC_NAME),
			PARAM_FLAG(G_PARAM_PRIVATE),
			PARAM_FLAG(G_PARAM_STATIC_NICK),
			PARAM_FLAG(G_PARAM_STATIC_BLURB),
			PARAM_FLAG(G_PARAM_EXPLICIT_NOTIFY),
			PARAM_FLAG(G_PARAM_DEPRECATED)
	};

	std::string str;
	bool first = false;
	if (values.empty()) str = _("none");
	for (auto value : values)
	{
		if (value.first & v)
		{
			if (first) str.append(", ");
			else first = true;
			str.append(value.second);
		}
	}

	return str;
}

void KlassesDialog::reload_list(const Glib::ustring &klass_name, bool add)
{
	// todo if (add)

	tree_model->clear();

	for (auto klass : controller->get_klasses())
	{
		auto row = *(tree_model->append());
		row[klasses_columns.m_col_name] = klass.get_name();
		row[klasses_columns.m_col_value] = "";

		auto childrow = *(tree_model->append(row.children()));
		childrow[klasses_columns.m_col_name] = _("Properties");

		for (auto property : klass.get_properties())
		{
			auto cr = *(tree_model->append(childrow.children()));

			cr[klasses_columns.m_col_name] = property.get_name();

			APPEND_SUB_ROW(cr, _("Nick"), property.get_nick());
			APPEND_SUB_ROW(cr, _("Blurb"), property.get_blurb());
			APPEND_SUB_ROW(cr, _("Flags"), g_param_flags_to_string(property.get_flags()));
			APPEND_SUB_ROW(cr, _("Default value"), property.get_default_value()->to_string());
		}
	}
}
#undef APPEND_SUB_ROW

