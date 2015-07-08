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

std::string GstBinToDotConverter::debug_dump_make_object_name (const Glib::RefPtr<Gst::Object>& obj)
{
	gchar* str = g_strcanon (g_strdup_printf ("%s_%p", obj->get_name().c_str(), obj->gobj()),
			G_CSET_A_2_Z G_CSET_a_2_z G_CSET_DIGITS "_", '_');
	std::string ret(str);
	g_free (str);
	return ret;
}

void GstBinToDotConverter::debug_dump_pad (const Glib::RefPtr<Gst::Pad>& pad, const gchar * color_name, const std::string& element_name, const gint indent)
{
	Glib::RefPtr<Gst::PadTemplate> pad_templ;
	const gchar *style_name = "filled,solid";

	if ((pad_templ = pad->get_pad_template())) {
		Gst::PadPresence presence = pad_templ->get_presence();
		if (presence == Gst::PAD_SOMETIMES) {
			style_name = "filled,dotted";
		} else if (presence == Gst::PAD_REQUEST) {
			style_name = "filled,dashed";
		}
	}
	const gchar *spc = &spaces[MAX (sizeof (spaces) - (1 + indent * 2), 0)];
	g_string_append_printf (str,
			"%s  %s_%s [color=black, fillcolor=\"%s\", label=\"%s\", height=\"0.2\", style=\"%s\"];\n",
			spc, element_name.c_str(), debug_dump_make_object_name (pad).c_str(), color_name, pad->get_name().c_str(),
			style_name);
}


void GstBinToDotConverter::debug_dump_element_pad (const Glib::RefPtr<Gst::Pad>& pad, const Glib::RefPtr<Gst::Element>& element, const gint indent)
{
	Glib::RefPtr<Gst::Element> target_element;
	Glib::RefPtr<Gst::Pad> target_pad, tmp_pad;
	std::string target_element_name;
	const gchar *color_name;

	Gst::PadDirection dir = pad->get_direction();
	auto element_name = debug_dump_make_object_name (element);
	if (pad->is_ghost_pad())
	{
		color_name = (dir == Gst::PAD_SRC) ? "#ffdddd" :
				((dir == Gst::PAD_SINK) ? "#ddddff" : "#ffffff");
		if ((tmp_pad = Glib::RefPtr<Gst::GhostPad>::cast_static (pad)->get_target())) {
			if ((target_pad = tmp_pad->get_peer())) {
				std::string pad_name, target_pad_name;
				const gchar *spc = &spaces[MAX (sizeof (spaces) - (1 + indent * 2), 0)];

				if ((target_element = target_pad->get_parent_element())) {
					target_element_name =
							debug_dump_make_object_name (target_element);
				} else {
					target_element_name = "";
				}
				debug_dump_pad (target_pad, color_name, target_element_name.c_str(), indent);
				/* src ghostpad relationship */
				pad_name = debug_dump_make_object_name (pad);
				target_pad_name = debug_dump_make_object_name (target_pad);
				if (dir == Gst::PAD_SRC) {
					g_string_append_printf (str,
							"%s%s_%s -> %s_%s [style=dashed, minlen=0]\n", spc,
							target_element_name.c_str(), target_pad_name.c_str(), element_name.c_str(), pad_name.c_str());
				} else {
					g_string_append_printf (str,
							"%s%s_%s -> %s_%s [style=dashed, minlen=0]\n", spc,
							element_name.c_str(), pad_name.c_str(), target_element_name.c_str(), target_pad_name.c_str());
				}
			}
		}
	} else {
		color_name = (dir == Gst::PAD_SRC) ? "#ffaaaa" :
				((dir == Gst::PAD_SINK) ? "#aaaaff" : "#cccccc");
	}
	/* pads */
	debug_dump_pad (pad, color_name, element_name, indent);
}
void GstBinToDotConverter::debug_dump_element_pad_link (const Glib::RefPtr<Gst::Pad>& pad, Glib::RefPtr<Gst::Element>& element, const gint indent)
{
	Glib::RefPtr<Gst::Element> peer_element;
	Glib::RefPtr<Gst::Pad> peer_pad;
	std::string pad_name, element_name;
	std::string peer_pad_name, peer_element_name;
	const gchar *spc = &spaces[MAX (sizeof (spaces) - (1 + indent * 2), 0)];

	if ((peer_pad = pad->get_peer())) {
		pad_name = debug_dump_make_object_name (pad);
		if (element) {
			element_name = debug_dump_make_object_name (element);
		} else {
			element_name = "";
		}
		peer_pad_name = debug_dump_make_object_name (peer_pad);
		if ((peer_element = peer_pad->get_parent_element())) {
			peer_element_name =
					debug_dump_make_object_name (peer_element);
		} else {
			peer_element_name = "";
		}

		g_string_append_printf (str, "%s%s_%s -> %s_%s\n", spc,
				element_name.c_str(), pad_name.c_str(), peer_element_name.c_str(), peer_pad_name.c_str());
	}
}


void GstBinToDotConverter::debug_dump_element_pads (Gst::Iterator<Gst::Pad> pad_iter,Glib::RefPtr<Gst::Element>& element, const gint indent)
{
	Glib::RefPtr<Gst::Pad> pad;

	while (pad_iter.next())
	{
		debug_dump_element_pad (*pad_iter, element, indent);
	}
}

void GstBinToDotConverter::debug_dump_element (const Glib::RefPtr<Gst::Bin>& bin, const gint indent)
{
	Glib::RefPtr<Gst::Element> element;
	const gchar *spc = &spaces[MAX (sizeof (spaces) - (1 + indent * 2), 0)];

	Gst::Iterator<Gst::Element> element_iter = bin->iterate_elements();
	while (element_iter.next())
	{
		element = *element_iter;
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

		debug_dump_element_pads (element->iterate_sink_pads(), element, indent);
		debug_dump_element_pads (element->iterate_src_pads(), element, indent);

		g_string_append_printf (str, "%s  fillcolor=\"#ffffff\";\n", spc);
		g_string_append_printf (str, "%s}\n\n", spc);

		auto pad_iter = element->iterate_pads();
		while (pad_iter.next() && pad_iter->is_linked()) {
			if (pad_iter->get_direction() == Gst::PAD_SRC)
				debug_dump_element_pad_link (*pad_iter, element, indent);
			else
			{
				auto peer_pad = pad_iter->get_peer();

				if (!peer_pad)
					continue;
				if (!peer_pad->is_ghost_pad() && peer_pad->is_proxy_pad())
				{
					Glib::RefPtr<Gst::Element> e;
					debug_dump_element_pad_link (peer_pad, e, indent);
				}
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

std::string GstBinToDotConverter::to_dot_data (const Glib::RefPtr<Gst::Bin>& bin)
{
	str = g_string_new (NULL);

	debug_dump_header ();
	debug_dump_element (bin, 1);
	g_string_append_printf (str, "}\n");

	gchar *tmp = g_string_free (str, FALSE);
	std::string ret = tmp;
	g_free(tmp);

	return ret;
}

