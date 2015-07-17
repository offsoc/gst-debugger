/*
 * gst_bin_to_dot_converter.cpp
 *
 *  Created on: Jul 8, 2015
 *      Author: loganek
 */

#include "gst_bin_to_dot_converter.h"

const gchar spaces[] = {
		"                                "    /* 32 */
		"                                "        /* 64 */
		"                                "        /* 96 */
		"                                "        /* 128 */
};

std::string GstBinToDotConverter::debug_dump_make_object_name (const std::shared_ptr<GraphObject>& obj)
{
	gchar* str = g_strcanon (g_strdup_printf ("%s_%p", obj->get_name().c_str(), obj.get()),
			G_CSET_A_2_Z G_CSET_a_2_z G_CSET_DIGITS "_", '_');
	std::string ret(str);
	g_free (str);
	return ret;
}

void GstBinToDotConverter::debug_dump_pad (const std::shared_ptr<GraphPad>& pad, const gchar * color_name, const std::string& element_name, const gint indent)
{
	const gchar *style_name = "filled,solid";

	if (pad->get_presence() == Gst::PAD_SOMETIMES) {
		style_name = "filled,dotted";
	} else if (pad->get_presence() == Gst::PAD_REQUEST) {
		style_name = "filled,dashed";
	}

	const gchar *spc = &spaces[MAX (sizeof (spaces) - (1 + indent * 2), 0)];
	g_string_append_printf (str,
			"%s  %s_%s [color=black, fillcolor=\"%s\", label=\"%s\", height=\"0.2\", style=\"%s\"];\n",
			spc, element_name.c_str(), debug_dump_make_object_name (pad).c_str(), color_name, pad->get_name().c_str(),
			style_name);
}

void GstBinToDotConverter::debug_dump_element_pad (const std::shared_ptr<GraphPad>& pad, const std::shared_ptr<GraphElement>& element, const gint indent)
{
	const gchar *color_name;

	Gst::PadDirection dir = pad->get_direction();
	std::string element_name = element ? debug_dump_make_object_name (element) : std::string();

	if (pad->is_ghost())
	{
		color_name = (dir == Gst::PAD_SRC) ? "#ffdddd" :
				((dir == Gst::PAD_SINK) ? "#ddddff" : "#ffffff");
	}
	else
	{
		color_name = (dir == Gst::PAD_SRC) ? "#ff7777" :
				((dir == Gst::PAD_SINK) ? "#7777ff" : "#cccccc");
	}

	debug_dump_pad (pad, color_name, element_name, indent);
}
void GstBinToDotConverter::debug_dump_element_pad_link (const std::shared_ptr<GraphPad>& pad, const gint indent)
{
	std::shared_ptr<GraphPad> peer_pad;
	std::string pad_name, element_name;
	std::string peer_pad_name, peer_element_name;
	const gchar *spc = &spaces[MAX (sizeof (spaces) - (1 + indent * 2), 0)];

	if ((peer_pad = pad->get_peer())) {
		pad_name = debug_dump_make_object_name (pad);
		if (pad->get_parent())
		{
			element_name = debug_dump_make_object_name (pad->get_parent());
		}

		peer_pad_name = debug_dump_make_object_name (peer_pad);
		if (peer_pad->get_parent())
		{
			peer_element_name = debug_dump_make_object_name (peer_pad->get_parent());
		}

		g_string_append_printf (str, "%s%s_%s -> %s_%s\n", spc,
				element_name.c_str(), pad_name.c_str(), peer_element_name.c_str(), peer_pad_name.c_str());
	}
}


void GstBinToDotConverter::debug_dump_element_pads (const std::vector<std::shared_ptr<GraphPad>>& pads, const std::shared_ptr<GraphElement>& element, const gint indent)
{
	for (auto pad : pads)
	{
		debug_dump_element_pad (pad, element, indent);
	}
}

void GstBinToDotConverter::debug_dump_element (const std::shared_ptr<GraphElement>& bin, const gint indent)
{
	Glib::RefPtr<Gst::Element> element;
	const gchar *spc = &spaces[MAX (sizeof (spaces) - (1 + indent * 2), 0)];

	for (auto element : bin->children)
	{
		std::string element_name = debug_dump_make_object_name (element);

		g_string_append_printf (str, "%ssubgraph cluster_%s {\n", spc,
				element_name.c_str());
		g_string_append_printf (str, "%s  fontname=\"Bitstream Vera Sans\";\n",
				spc);
		g_string_append_printf (str, "%s  fontsize=\"8\";\n", spc);
		g_string_append_printf (str, "%s  style=filled;\n", spc);
		g_string_append_printf (str, "%s  color=black;\n\n", spc);
		g_string_append_printf (str, "%s  label=\"%s\";\n", spc,
				element->get_name().c_str());

		debug_dump_element_pads (element->pads, element, indent);

		g_string_append_printf (str, "%s  fillcolor=\"#ffffff\";\n", spc);
		g_string_append_printf (str, "%s}\n\n", spc);

		for (auto pad : element->pads)
		{
			if (!pad->get_peer())
				continue;

			if (pad->get_direction() == Gst::PAD_SRC)
			{
				debug_dump_element_pad_link (pad, indent);
			}
		}
	}
}

void GstBinToDotConverter::debug_dump_header ()
{
	g_string_append_printf (str,
			"digraph pipeline {\n"
			"  rankdir=LR;\n"
			"  fontname=\"sans\";\n"
			"  fontsize=\"10\";\n"
			"  labelloc=t;\n"
			"  nodesep=.1;\n"
			"  ranksep=.2;\n"
			"  \n"
			"  node [style=filled, shape=box, fontsize=\"9\", fontname=\"sans\", margin=\"0.0,0.0\"];\n"
			"  edge [labelfontsize=\"6\", fontsize=\"9\", fontname=\"monospace\"];\n"
			"  \n"
			"\n");
}

void GstBinToDotConverter::debug_dump_top_pads(const std::shared_ptr<GraphElement>& bin)
{
	for (auto pad : bin->pads)
	{
		debug_dump_element_pad (pad, bin, 1);

		if (pad->get_direction() == Gst::PAD_SINK)
		{
			debug_dump_element_pad_link (pad, 1);
		}
	}
}

std::string GstBinToDotConverter::to_dot_data (const std::shared_ptr<GraphElement>& bin)
{
	str = g_string_new (NULL);

	debug_dump_header ();

	debug_dump_top_pads(bin);

	debug_dump_element (bin, 1);
	g_string_append_printf (str, "}\n");

	gchar *tmp = g_string_free (str, FALSE);
	std::string ret = tmp;
	g_free(tmp);

	return ret;
}

