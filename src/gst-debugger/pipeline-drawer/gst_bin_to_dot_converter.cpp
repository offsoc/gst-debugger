/*
 * gst_bin_to_dot_converter.cpp
 *
 *  Created on: Jul 8, 2015
 *      Author: loganek
 */

#include "gst_bin_to_dot_converter.h"

std::string GstBinToDotConverter::make_object_name(const GraphObjectPtr& obj)
{
	gchar* str = g_strcanon (g_strdup_printf ("%s_%p", obj->get_name().c_str(), obj.get()),
			G_CSET_A_2_Z G_CSET_a_2_z G_CSET_DIGITS "_", '_');
	std::string ret(str);
	g_free (str);
	return ret;
}

void GstBinToDotConverter::dump_pad(const GraphPadPtr& pad)
{
	const gchar *color_name;

	Gst::PadDirection dir = pad->get_direction();

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

	std::string style_name = "filled,solid";

	if (pad->get_presence() == Gst::PAD_SOMETIMES)
	{
		style_name = "filled,dotted";
	}
	else if (pad->get_presence() == Gst::PAD_REQUEST) {
		style_name = "filled,dashed";
	}

	std::string element = pad->get_parent() ? make_object_name (pad->get_parent()) : std::string();
	str += element + "_" + make_object_name (pad) + " [color=black, fillcolor=\"" + color_name +
			"\", label=\"" + pad->get_name() + "\", height=\"0.2\", style=\"" + style_name + "\"];\n";

}
void GstBinToDotConverter::dump_pad_link(const GraphPadPtr& pad)
{
	std::shared_ptr<GraphPad> peer_pad;
	std::string pad_name, element_name;
	std::string peer_pad_name, peer_element_name;

	if ((peer_pad = pad->get_peer()))
	{
		pad_name = make_object_name (pad);
		if (pad->get_parent())
		{
			element_name = make_object_name (pad->get_parent());
		}

		peer_pad_name = make_object_name (peer_pad);
		if (peer_pad->get_parent())
		{
			peer_element_name = make_object_name (peer_pad->get_parent());
		}

		str += element_name + "_" + pad_name + " -> " + peer_element_name + "_" + peer_pad_name + "\n";
	}
}

void GstBinToDotConverter::dump_element(const GraphElementPtr& bin)
{
	Glib::RefPtr<Gst::Element> element;

	for (auto element : bin->get_children())
	{
		std::string element_name = make_object_name (element);

		str += "subgraph cluster_" + element_name + " {\n";
		str +=  "fontname=\"Bitstream Vera Sans\";\n";
		str += "fontsize=\"8\";\n";
		str += "style=filled;\n";
		str += "color=black;\n\n";
		str += "label=\"" + element->get_name() + "\";\n";

		for (auto pad : element->get_pads())
		{
			dump_pad (pad);
		}

		str += "fillcolor=\"#ffffff\";\n}\n\n";

		for (auto pad : element->get_pads())
		{
			if (!pad->get_peer())
				continue;

			if (pad->get_direction() == Gst::PAD_SRC)
			{
				dump_pad_link (pad);
			}
		}
	}
}

void GstBinToDotConverter::dump_header()
{
	str +=
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
			"\n";
}

void GstBinToDotConverter::dump_top_pads(const GraphElementPtr& bin)
{
	for (auto pad : bin->get_pads())
	{
		dump_pad (pad);

		if (pad->get_direction() == Gst::PAD_SINK)
		{
			dump_pad_link (pad);
		}
	}
}

std::string GstBinToDotConverter::to_dot_data(const GraphElementPtr& bin)
{
	str.clear();

	dump_header();

	dump_top_pads(bin);

	dump_element (bin);
	str += "}\n";

	return str;
}

