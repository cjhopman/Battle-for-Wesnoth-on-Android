/* $Id: game_save.cpp 48450 2011-02-08 20:55:18Z mordante $ */
/*
   Copyright (C) 2008 - 2011 by Jörg Hinrichs <joerg.hinrichs@alice-dsl.de>
   Part of the Battle for Wesnoth Project http://www.wesnoth.org/

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License version 2
   or at your option any later version.
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY.

   See the COPYING file for more details.
*/

#define GETTEXT_DOMAIN "wesnoth-lib"

#include "gui/dialogs/touch_game_menu.hpp"

#include "gui/dialogs/field.hpp"
#include "gui/widgets/button.hpp"
#include "gui/widgets/label.hpp"
#include "gui/widgets/settings.hpp"

#include "gui/auxiliary/log.hpp"

#include "dialogs.hpp"
#include "sdl_utils.hpp"

#include <boost/bind.hpp>

namespace gui2 {


ttouch_game_menu::ttouch_game_menu() 
{
}

void outside_click_dismiss(twindow* wnd, const tpoint& ev) {
	SDL_Rect r = wnd->get_rect();
	int buf = 10;
	r.x -= buf; r.y -= buf;
	r.w += 2 * buf; r.h += 2 * buf;
	if (!point_in_rect(ev.x, ev.y, r)) {
		wnd->close();
	}
}

twindow* ttouch_game_menu::build_window(CVideo& video)
{
	video_ = &video;
	twindow* ret = build(video, get_id(TOUCH_GAME_MENU));

	ret->connect_signal<event::SDL_LEFT_BUTTON_UP>(boost::bind(&outside_click_dismiss, ret, _5));

	change_page(0, ret);
	return ret;
}

namespace {
	template <int N>
		void change_page_callback(twidget* caller) {
			ttouch_game_menu* dialog = dynamic_cast<ttouch_game_menu*>(caller->dialog());
			assert(dialog);
			twindow* window = dynamic_cast<twindow*>(caller->get_window());
			assert(window);
			dialog->change_page(N, window);
		}

	void change_button(twindow* window, const std::string& bname, ttouch_game_menu::action retval, const std::string& label) {
		tbutton* btn = dynamic_cast<tbutton*>(window->find(bname, false));
		assert(btn);
		btn->set_retval(retval);
		btn->set_label(label);
	}
}

void ttouch_game_menu::change_page(int page, twindow* window) {
	switch (page) {
		case 0: {
			change_button(window, "button0", SAVE_GAME, "Save Game");
			change_button(window, "button1", LOAD_GAME, "Load Game");
			change_button(window, "button2", QUIT_GAME, "Quit Game");
			change_button(window, "button3", OBJECTIVES, "Objectives");
			change_button(window, "button4", RECALL, "Recall Unit");
			change_button(window, "button5", RECRUIT, "Recruit Unit");
			change_button(window, "button6", NOTHING, "Next Page");

			tbutton* btn = dynamic_cast<tbutton*>(window->find("button6", false));
			assert(btn);
			btn->set_callback_mouse_left_click(change_page_callback<1>);
			
			break;
		}
		case 1: {
			change_button(window, "button0", STATISTICS, "Statistics");
			change_button(window, "button1", UNIT_LIST, "Unit List");
			change_button(window, "button2", ENEMY_MOVES, "Enemy Moves");
			change_button(window, "button3", PREFERENCES, "Preferences");
			change_button(window, "button4", UNDO, "Undo");
			change_button(window, "button5", REDO, "Redo");
			change_button(window, "button6", NOTHING, "Previous Page");

			tbutton* btn = dynamic_cast<tbutton*>(window->find("button6", false));
			assert(btn);
			btn->set_callback_mouse_left_click(change_page_callback<0>);
			
			break;
		}
	}
}

void ttouch_game_menu::pre_show(CVideo& /*video*/, twindow& /*window*/)
{
}

void ttouch_game_menu::post_show(twindow& /*window*/)
{
}

} // namespace gui2

