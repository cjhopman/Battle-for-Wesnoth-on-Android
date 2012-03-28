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

#ifndef GUI_DIALOGS_MP_GAME_CONNECT_HPP_INCLUDED
#define GUI_DIALOGS_MP_GAME_CONNECT_HPP_INCLUDED

#include "gui/dialogs/dialog.hpp"
#include "gui/widgets/grid.hpp"
#include "gui/widgets/text.hpp"

#include "gamestatus.hpp"
#include "game_display.hpp"
#include "multiplayer_ui.hpp"
#include "multiplayer_connect.hpp"

#include <list>

class config;

class mp_game_settings;
namespace gui2 {

class twidget;
class ttext_box;

class tconnect_ui;

class tside_ui : public mp::side_ui {
	typedef mp::side_ui super;
public:
	tside_ui(twidget* widget, tconnect_ui* parent);
	tside_ui(const tside_ui& o);

	void set_widget(twidget* widget);
	void setup_adaptors();
private:
	void change_ai_algorithm_cb(int c);
	void change_controller_cb(int c);
	void change_faction_cb(int c);
	void change_leader_cb(int c);
	void change_gender_cb(int c);
	void change_team_cb(int c);
	void change_colour_cb(int c);
	void change_gold_cb();
	void change_income_cb();

	twidget* widget_;
	tconnect_ui* parent_;
};

class tconnect_ui : public mp::connect_ui {
public:
	tconnect_ui(twindow& window, game_display& disp, const config& game_config,
			mp::chat& c, config& gamelist, const mp_game_settings& params);

	virtual bool hidden() const;
	virtual void show_sides(const std::vector<mp::connect_side>& sides);
	virtual mp::side_ui* next_side_ui();
	void full_update() { connect_->full_update(); }
	void set_user_list(const std::vector<std::string>& list, bool silent);
private:
	twindow& window_;
	std::list<tside_ui> sides_;
};

class tmp_game_connect : public tdialog
{
public:
	tmp_game_connect(game_display& disp, const config& game_config, mp::chat& c, config& gamelist,
			const mp_game_settings& parameters, const mp::controller default_controller, bool local_players_only,
			game_state& state);
	~tmp_game_connect();

	mp::ui::result get_result() { return ui_->get_result(); }

private:
	void set_result(mp::ui::result r) { ui_->set_result(r); }

	void load_level(twindow& window);

	/** Inherited from tdialog. */
	twindow* build_window(CVideo& video);

	/** Inherited from tdialog. */
	void pre_show(CVideo& video, twindow& window);

	/** Inherited from tdialog. */
	void post_show(twindow& window);

	void network_handler();

	void ready_handler(twindow* window, bool& handled);
	void cancel_handler(twindow* window, bool& handled);
	bool chat_key_press_handler(twidget*, SDLKey);

	std::auto_ptr<tconnect_ui> ui_;
	std::auto_ptr<mp::connect> connect_;

	game_display& disp_;
	const config& game_config_;
	mp::chat& chat_;
	config& gamelist_;
	mp_game_settings params_;
	mp::controller default_controller_;
	bool local_players_only_;
	game_state& state_;
	int update_timer_;
	bool showing_chat_;
};

} // namespace gui2

#endif

