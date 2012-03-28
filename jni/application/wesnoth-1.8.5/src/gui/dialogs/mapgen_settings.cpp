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

#include "gui/dialogs/mapgen_settings.hpp"

#include "gui/widgets/settings.hpp"
#include "gui/widgets/slider.hpp"
#include "gui/dialogs/helper.hpp"

#include <boost/bind.hpp>

namespace gui2 {

tmapgen_settings::tmapgen_settings(settings& s) : s_(s)
{
}

twindow* tmapgen_settings::build_window(CVideo& video) {
	return build(video, get_id(MAPGEN_SETTINGS));
}

void slider_cb(int& v, twidget* w) {
	tslider* sl = dynamic_cast<tslider*>(w);
	assert(sl);
	v = sl->get_value();
}

void tmapgen_settings::setup_slider(twindow* window, std::string id, slider_settings& s) {
	if (tslider* sl = find_widget<tslider>(window, id, false, false)) {
		sl->set_maximum_value(s.max);
		sl->set_minimum_value(s.min);
		sl->set_value(s.value);
		sl->set_callback_positioner_released(boost::bind(slider_cb, boost::ref(s.value), _1));
	}
}

void tmapgen_settings::pre_show(CVideo& video, twindow& window) {
	setup_slider(&window, "width_slider", s_.width);
	setup_slider(&window, "height_slider", s_.height);
	setup_slider(&window, "island_size_slider", s_.island_size);
	setup_slider(&window, "iterations_slider", s_.iterations);
	setup_slider(&window, "hill_size_slider", s_.hill_size);
	setup_slider(&window, "nvillages_slider", s_.nvillages);
	setup_slider(&window, "castle_size_slider", s_.castle_size);
	setup_slider(&window, "nplayers_slider", s_.nplayers);
}

void tmapgen_settings::post_show(twindow& window) {

}

}
