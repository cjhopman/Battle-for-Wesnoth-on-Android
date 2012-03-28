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

#include "gui/dialogs/menu.hpp"

#include "gui/dialogs/helper.hpp"
#include "gui/widgets/button.hpp"
#include "gui/widgets/label.hpp"
#include "gui/widgets/listbox.hpp"
#include "gui/widgets/settings.hpp"
#include "gui/widgets/slider.hpp"
#include "gui/widgets/window.hpp"

#include <boost/bind.hpp>

namespace gui2 {

tmenu::ssmap tmenu::default_transform(std::string s) {
	ssmap data;
	data["label"]["label"] = s;
	return data;
}

tmenu::tmenu(std::string t, const std::vector<std::string>& v, transform_type tr) :
	title_(t),
	vals_(v),
	transform_(tr),
	res_(0)
{
}

twindow* tmenu::build_window(CVideo& video) {
	return build(video, "menu");
}

void tmenu::pre_show(CVideo& video, twindow& window) {
	window.set_enter_disabled(true);
	window.set_escape_disabled(true);

	tlabel& title = find_widget<tlabel>(&window, "title", false);
	title.set_label(title_);

	tlistbox& listbox = find_widget<tlistbox>(&window, "menu_list", false);
	foreach(std::string s, vals_) {
		listbox.add_row(transform_(s));
	}
	listbox.set_callback_value_change(boost::bind(&tmenu::select_cb, this, &window, _1));
	listbox.select_row(0);

	tbutton& accept = find_widget<tbutton>(&window, "accept", false);
	accept.connect_click_handler(boost::bind(&tmenu::accept_cb, this, &window));
}

void tmenu::post_show(twindow& window) {
}

void tmenu::accept_cb(twindow* window) {
	window->close();
}

void tmenu::select_cb(twindow* window, tlistbox* l) {
	res_ = l->get_selected_row();
	tbutton& accept = find_widget<tbutton>(window, "accept", false);
	accept.set_active(size_t(res_) < l->get_item_count() ? true : false);
}

}
