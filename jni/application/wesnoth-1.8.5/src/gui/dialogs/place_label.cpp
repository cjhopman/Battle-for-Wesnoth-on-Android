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

#include "gui/dialogs/place_label.hpp"

#include "gui/dialogs/helper.hpp"
#include "gui/widgets/settings.hpp"
#include "gui/widgets/text.hpp"
#include "gui/widgets/toggle_button.hpp"
#include "gui/widgets/window.hpp"

#include <boost/bind.hpp>

namespace gui2 {

namespace {
void outside_click_dismiss(twindow* wnd, const tpoint& ev) {
	SDL_Rect r = wnd->get_rect();
	int buf = 10;
	r.x -= buf; r.y -= buf;
	r.w += 2 * buf; r.h += 2 * buf;
	if (!point_in_rect(ev.x, ev.y, r)) {
		wnd->close();
	}
}
}

tplace_label::tplace_label(std::string t, bool team_only, int max_len) :
	text_(t), team_only_(team_only), max_len_(max_len)
{
}

twindow* tplace_label::build_window(CVideo& video) {
	twindow* ret = build(video, "place_label");
	ret->connect_signal<event::SDL_LEFT_BUTTON_UP>(boost::bind(&outside_click_dismiss, ret, _5));
	return ret;
}

void text_cb(std::string& s, std::string v) {
	s = v;
}
void toggle_cb(bool& s, twidget* w) {
	ttoggle_button* tb = dynamic_cast<ttoggle_button*>(w);
	assert(tb);
	s = tb->get_value();
}

void tplace_label::pre_show(CVideo& video, twindow& window) {
	find_widget<ttext_>(&window, "input", true).set_text_changed_callback(boost::bind(text_cb, boost::ref(text_), _2));
	find_widget<ttoggle_button>(&window, "team_only", true).set_callback_state_change(boost::bind(toggle_cb, boost::ref(team_only_), _1));
	find_widget<ttoggle_button>(&window, "team_only", true).set_value(team_only_);
}


void tplace_label::post_show(twindow& window) {
}

}
