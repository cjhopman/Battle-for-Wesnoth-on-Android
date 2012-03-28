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

#ifndef GUI_DIALOGS_MP_GAME_WAIT_HPP_INCLUDED
#define GUI_DIALOGS_MP_GAME_WAIT_HPP_INCLUDED

#include "multiplayer_wait.hpp"
#include "gui/dialogs/dialog.hpp"
#include "gui/widgets/image.hpp"
#include "gui/widgets/label.hpp"
#include "gui/widgets/listbox.hpp"
#include "gui/widgets/window.hpp"

#include "leader_list.hpp"

#include <vector>

class config;
class mp_game_settings;

namespace gui2 {

class tmp_game_wait : public tdialog, private mp::wait {
public:
	tmp_game_wait(game_display& disp, const config& game_config, mp::chat& c, config& gamelist);
	~tmp_game_wait();

	using mp::wait::join_game;
	using mp::wait::get_result;
	using mp::wait::start_game;
	using mp::wait::get_state;
private:
	/** Inherited from tdialog. */
	twindow* build_window(CVideo& video);

	virtual void process_network_data(const config& data, const network::connection sock);

	/** Inherited from tdialog. */
	void pre_show(CVideo& video, twindow& window);

	/** Inherited from tdialog. */
	void post_show(twindow& window);

	void cancel_handler(bool& handled);
	bool chat_key_press_handler(twidget* w, SDLKey k);
	mp::chat& chat_;
	int update_timer_;
	twindow* window_;
	bool showing_chat_;
};


class tmp_leader_preview : public tdialog {
public:
	tmp_leader_preview(const std::vector<const config *> &side_list, int color);

	int get_selected_faction();
	std::string get_selected_leader();
	std::string get_selected_gender();

private:
	/** Inherited from tdialog. */
	twindow* build_window(CVideo& video);

	/** Inherited from tdialog. */
	void pre_show(CVideo& video, twindow& window);

	/** Inherited from tdialog. */
	void post_show(twindow& window);

	void faction_change_handler(tlistbox* list);
	void leader_change_handler(int val);
	void gender_change_handler(int val);

	void update_data();

	int faction_;
	std::vector<const config *> side_list_;
	const int color_;
	leader_list_manager llm_;
	timage* leader_image_;
	tlabel* recruits_label_;
};

} // namespace gui2

#endif

