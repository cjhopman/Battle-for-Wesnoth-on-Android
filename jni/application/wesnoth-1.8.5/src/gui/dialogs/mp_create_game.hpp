/* $Id: mp_create_game.hpp 48450 2011-02-08 20:55:18Z mordante $ */
/*
   Copyright (C) 2008 - 2011 by Mark de Wever <koraq@xs4all.nl>
   Part of the Battle for Wesnoth Project http://www.wesnoth.org/

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License version 2
   or at your option any later version.
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY.

   See the COPYING file for more details.
*/

#ifndef GUI_DIALOGS_MP_CREATE_GAME_HPP_INCLUDED
#define GUI_DIALOGS_MP_CREATE_GAME_HPP_INCLUDED

#include "gui/dialogs/dialog.hpp"

#include "gui/widgets/settings.hpp"

#include "mapgen.hpp"
#include "mp_game_settings.hpp"
#include "multiplayer_ui.hpp"

class config;

namespace gui2 {

class twidget;
class ttext_box;

class tmp_create_game : public tdialog
{
public:
	tmp_create_game(game_display& disp, const config& cfg, mp::ui::result& res, mp_game_settings& parameters);

private:
	/** Inherited from tdialog. */
	twindow* build_window(CVideo& video);

	/** Inherited from tdialog. */
	void pre_show(CVideo& video, twindow& window);

	/** Inherited from tdialog. */
	void post_show(twindow& window);

	const config& cfg_;

	/**
	 * All fields are also in the normal field vector, but they need to be
	 * manually controled as well so add the pointers here as well.
	 */

	tfield_bool
		*const countdown_,
		*const use_map_settings_,
		*const start_time_,
		*const fog_,
		*const shroud_,
		*const allow_observers_,
		*const share_view_,
		*const share_maps_;

	tfield_integer
		*const turns_,
		*const gold_,
		*const experience_,
		*const countdown_init_time_,
		*const countdown_reservoir_time_,
		*const countdown_turn_bonus_,
		*const countdown_action_bonus_;

	tfield_text
		*const game_name_,
		*const game_password_;

	mp::ui::result& res_;
	mp_game_settings& parameters_;
	std::auto_ptr<map_generator> generator_;

	game_display& disp_;
	twindow* window_;

	void random_settings_cb();
	void random_regenerate_cb();
public:
	// another map selected
	void update_map();
	// use_map_settings toggled (also called in other cases.)
	void update_map_settings();
	void regenerate_map();
};

} // namespace gui2

#endif

