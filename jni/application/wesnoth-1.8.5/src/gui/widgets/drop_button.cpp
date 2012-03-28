/*
   Copyright (C) 2011 - 2012 Chris Hopman <cjhopman@gmail.com>
   Part of the Battle for Wesnoth Project http://www.wesnoth.org/

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License version 2
   or at your option any later version.
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY.

   See the COPYING file for more details.
*/

#define GETTEXT_DOMAIN "wesnoth-lib"

#include "gui/widgets/drop_button.hpp"

#include "gui/auxiliary/log.hpp"
#include "gui/dialogs/dialog.hpp"
#include "gui/dialogs/helper.hpp"
#include "gui/widgets/listbox.hpp"
#include "gui/widgets/window.hpp"

#include "marked-up_text.hpp"

#include <boost/bind.hpp>

#define LOG_SCOPE_HEADER get_control_type() + " [" + id() + "] " + __func__
#define LOG_HEADER LOG_SCOPE_HEADER + ':'

namespace gui2 {


std::string convert_and_ellipsize(std::string s, int len) {
	if (s.find('<') == 0) {
		int e = s.find('>');
		std::stringstream ss;
		ss << "<span foreground=\"#" << font::color2hexa(font::markup2color(s.substr(1, e - 1))) << "\">";
		ss << ellipsize(s.substr(e + 1), len);
		ss << "</span>";
		return ss.str();
	} else {
		return ellipsize(s, len);
	}
}

std::map<std::string, string_map> tdrop_button::default_transform(std::string s) {
	std::map<std::string, string_map> ret;
	ret["label"]["label"] = convert_and_ellipsize(s, 40);
	ret["label"]["use_markup"] = "true";
	ret["drop_button"]["label"] = convert_and_ellipsize(s, 13);
	return ret;
}

tdrop_button::tdrop_button() :
	tbutton(),
	transform_(default_transform),
	selected_(0)
{
	connect_click_handler(boost::bind(&tdrop_button::create_drop_listbox, this, _3, _4));
}

void tdrop_button::set_selected(int v) {
	if (size_t(v) < vals_.size()) {
		selected_ = v;
		set_label(vals_[v]);
	}
}

void tdrop_button::set_label(const t_string& str) {
	set_use_markup(true);
	std::string s = transform_(str)["drop_button"]["label"];
	super::set_label(s);
}

void tdrop_button::create_drop_listbox(
		bool& handled,
		bool& halt) {
	handled = halt = true;
	CVideo& video = get_window()->video();
	tdrop_listbox dlg(get_origin(), vals_, linked_groups_, list_builder_, transform_, selected_);
	dlg.show(video);
	int ret = dlg.get_retval();
	if (ret >= 0) {
		set_selected(ret);
		if (callback_selection_changed_) {
			callback_selection_changed_(this, ret);
		}
	}
}

void tdrop_button::set_items(const std::vector<std::string>& vals) {
	vals_ = vals; 
	set_selected(selected_);
}

} // namespace gui2
