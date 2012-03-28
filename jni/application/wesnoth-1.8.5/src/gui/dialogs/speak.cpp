/*
   Copyright (C) by Chris Hopman <cjhopman@gmail.com>
   Part of the Battle for Wesnoth Project http://www.wesnoth.org/

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License version 2
   or at your option any later version.
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY.

   See the COPYING file for more details.
*/

#define GETTEXT_DOMAIN "wesnoth-lib"

#include "gui/dialogs/speak.hpp"

#include "gui/dialogs/helper.hpp"
#include "gui/widgets/button.hpp"
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

tspeak::tspeak() :
	message_("")
{
}

twindow* tspeak::build_window(CVideo& video) {
	twindow* ret = build(video, "speak");
	ret->connect_signal<event::SDL_LEFT_BUTTON_UP>(boost::bind(&outside_click_dismiss, ret, _5));
	return ret;
}

void tspeak::pre_show(CVideo& /*video*/, twindow& window) {
	find_widget<tbutton>(&window, "click_dismiss", false).set_visible(twidget::INVISIBLE);
}

void tspeak::post_show(twindow& window) {
	message_ = find_widget<ttext_box>(&window, "input", false).get_value();
}

}
