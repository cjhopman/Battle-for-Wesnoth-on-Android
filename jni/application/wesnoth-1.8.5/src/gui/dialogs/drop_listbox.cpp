/*
   Copyright (C) 2008 - 2011 by Chris Hopman <cjhopman@gmail.com>
   Part of the Battle for Wesnoth Project http://www.wesnoth.org/

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License version 2
   or at your option any later version.
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY.

   See the COPYING file for more details.
*/

#define GETTEXT_DOMAIN "wesnoth-lib"

#include "gui/dialogs/drop_listbox.hpp"

#include "gui/auxiliary/log.hpp"
#include "gui/auxiliary/timer.hpp"
#include "gui/dialogs/dialog.hpp"
#include "gui/dialogs/helper.hpp"
#include "gui/widgets/listbox.hpp"
#include "gui/widgets/window.hpp"

#include "marked-up_text.hpp"
#include "wml_separators.hpp"

#include <boost/bind.hpp>

#include <sstream>

namespace gui2 {

namespace {
void outside_click_dismiss(twindow* wnd, const tpoint& ev) {
	SDL_Rect r = wnd->get_rect();
	int buf = 10;
	r.x -= buf; r.y -= buf;
	r.w += 2 * buf; r.h += 2 * buf;
	if (!point_in_rect(ev.x, ev.y, r)) {
		wnd->set_retval(twindow::CANCEL);
	}
}
std::map<std::string, string_map> default_transform(std::string s) {
	std::map<std::string, string_map> data;
	std::stringstream ss(s);
	std::string v;
	int i = 0;
	while (getline(ss, v, COLUMN_SEPARATOR)) {
		std::stringstream out;
		if (!v.empty()) {
			std::string k = "label_";
			if (v[0] == '<') {
				int e = v.find('>');
				out << "<span foreground=\"#" << font::color2hexa(font::markup2color(v.substr(1, e - 1))) << "\">";
				out << ellipsize(v.substr(e + 1), 50);
				out << "</span>";
			} else if (v[0] == '&') {
				k = "image_";
				out << v.substr(1);
			} else {
				out << v;
			}
			k += lexical_cast<std::string>(i);
			data[k]["label"] = out.str();
		}
		++i;
	}
	return data;
}
} // anonymous namespace

tdrop_listbox::tdrop_listbox(
		tpoint pos,
		std::vector<std::string> vals,
		std::vector<std::string> groups,
		tbuilder_grid_ptr builder,
		transform_type trans,
		int sel) :
	pos_(pos),
	vals_(vals),
	linked_groups_(groups),
	list_builder_(builder),
	transform_(trans ? trans : default_transform),
	selected_(sel),
	close_timer_(0)
{
}

twindow* tdrop_listbox::build_window(CVideo& video)
{
	twindow* ret = build(video, "drop_list");
	ret->connect_signal<event::SDL_LEFT_BUTTON_UP>(boost::bind(&outside_click_dismiss, ret, _5));
	return ret;
}

void tdrop_listbox::close_and_select(twindow* window, tlistbox* listbox) {
	if (close_timer_) return;
	window->set_retval(listbox->get_selected_row(), false);
	close_timer_ = add_timer(150
			, boost::bind(&twindow::close, window)
			, false);
}

void tdrop_listbox::pre_show(CVideo& /*video*/, twindow& window)
{
	window.set_click_dismiss(false);
	std::string x = lexical_cast<std::string>(pos_.x);
	std::string y = lexical_cast<std::string>(pos_.y);

	window.set_position_formula(
	"(if (best_w > 0, min(screen_width - best_w, " + x + "), 0))",
	"(if (best_h > 0, min(screen_height - best_h, " + y + "), 0))"
			);
	foreach(std::string s, linked_groups_) {
		window.init_linked_size_group(s, true, true);
	}

	tlistbox& listbox = find_widget<tlistbox>(&window, "drop_list_widget", false);
	if (list_builder_) listbox.set_list_builder(list_builder_);
	foreach(std::string s, vals_) {
		listbox.add_row(transform_(s));
	}
	if (size_t(selected_) < vals_.size()) {
		listbox.select_row(selected_);
	}
	listbox.set_callback_value_change(boost::bind(&tdrop_listbox::close_and_select, this, &window, _1));
}

void tdrop_listbox::post_show(twindow& /*window*/)
{
}

} // gui2 namespace
