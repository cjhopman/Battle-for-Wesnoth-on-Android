/*
   Copyright (C) 2012
   Part of the Battle for Wesnoth Project http://www.wesnoth.org

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License version 2
   or at your option any later version.
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY.

   See the COPYING file for more details.
*/

#ifndef MULTIPLAYER_CONNECT_UI_HPP_INCLUDED
#define MULTIPLAYER_CONNECT_UI_HPP_INCLUDED

#include "config.hpp"
#include "gamestatus.hpp"
#include "gui_adaptor.hpp"
#include "leader_list.hpp"
#include "multiplayer_ui.hpp"
#include "network.hpp"

#include "widgets/scrollpane.hpp"
#include "widgets/slider.hpp"
#include "widgets/combo_drag.hpp"

#include <list>
#include <string>

namespace mp {

class connect;
class connect_side;

class connect_ui_gui1;

class side_ui {
public:
	void set_gold(int val);
	void set_income(int val);

	void set_faction_list(const std::vector<std::string>& factions, int selected = 0);
	void set_faction(int index);

	void set_ai_list(const std::vector<std::string>& ais, int selected = 0);
	void set_ai(int index);

	void set_leader_list(const std::vector<std::string>& leaders, int selected = 0);
	void set_leader(int index);

	void set_gender_list(const std::vector<std::string>& genders, int selected = 0);
	void set_gender(int index);

	void set_controller_list(const std::vector<std::string>& controllers, int selected = 0);
	void set_controller(int index);

	void set_team_list(const std::vector<std::string>& teams, int selected = 0);
	void set_team(int index);

	void set_colour_list(const std::vector<std::string>& colours, int selected = 0);
	void set_colour(int index);

	void set_player(const std::string& player);
	void set_player_number(const std::string& player);

	void enable_gold(bool enable);
	void enable_income(bool enable);
	void enable_team(bool enable);
	void enable_colour(bool enable);
	void enable_leader(bool enable);
	void enable_gender(bool enable);
	void enable_faction(bool enable);

	void enable_llm(leader_list_manager& llm, bool enable);

	void hide_ai(bool hide);
	void hide_orig_controller(bool hide);
	void hide_ai_algorithm_combo(bool hide);

	bool faction_enabled() { return combo_faction_.enabled(); }
	bool leader_enabled() { return combo_leader_.enabled(); }
	bool gender_enabled() { return combo_gender_.enabled(); }

	void set_side(connect_side* s) { side_ = s; }

protected:
	gui_adaptor::list combo_controller_;
	gui_adaptor::list combo_ai_algorithm_;
	gui_adaptor::list combo_faction_;
	gui_adaptor::list combo_leader_;
	gui_adaptor::list combo_gender_;
	gui_adaptor::list combo_team_;
	gui_adaptor::list combo_colour_;

	gui_adaptor::value<std::string> player_number_;
	gui_adaptor::value<std::string> orig_controller_;
	gui_adaptor::value<std::string> label_gold_;
	gui_adaptor::value<std::string> label_income_;

	gui_adaptor::value<int> slider_gold_;
	gui_adaptor::value<int> slider_income_;

	connect_side* side_;
};

class side_ui_gui1 : public side_ui {
public:
	side_ui_gui1(connect_ui_gui1* parent);
	side_ui_gui1(const side_ui_gui1& o);

	void add_widgets_to_scrollpane(gui::scrollpane& pane, int pos);
	bool process_event();
	void init();
	void setup_adaptors();
private:
	// Widgets for this side
	gui::combo_drag_ptr w_combo_controller_;

	gui::combo w_combo_ai_algorithm_;
	gui::combo w_combo_faction_;
	gui::combo w_combo_leader_;
	gui::combo w_combo_gender_;
	gui::combo w_combo_team_;
	gui::combo w_combo_colour_;

	gui::label w_player_number_;
	gui::label w_orig_controller_;
	gui::label w_label_gold_;
	gui::label w_label_income_;

	gui::slider w_slider_gold_;
	gui::slider w_slider_income_;
};

class connect_ui : public ui {
	friend class side_ui_gui1;
public:
	connect_ui(game_display& disp, const config& game_config,
		chat& c, config& gamelist, const mp_game_settings& params);

	void hide_gold_title(bool hide);
	void hide_income_title(bool hide);
	void enable_launch(bool enable);
	void set_waiting_label(const std::string& str);

	void set_connect(connect* c) { connect_ = c; }
	game_display& disp() { return disp_; }

	void append_to_title(const std::string& name);

	void process_network_data(const config& data, const network::connection sock);
	void process_network_error(network::error& error);
	bool accept_connections();
	void process_network_connection(const network::connection sock);

	virtual void show_sides(const std::vector<connect_side>&) { }
	virtual side_ui* next_side_ui() { return NULL; }

	virtual bool hidden() const { return ui::hidden(); }
	virtual void set_result(result res) { ui::set_result(res); }
	virtual result get_result() { return ui::get_result(); }
	virtual void gamelist_updated(bool silent) { return ui::gamelist_updated(silent); }
	virtual void set_user_list(const std::vector<std::string>& list, bool silent) { ui::set_user_list(list, silent); }
	virtual void set_user_menu_items(const std::vector<std::string>& list) { ui::set_user_menu_items(list); }

protected:
	gui_adaptor::value<std::string> waiting_label_;
	gui_adaptor::value<std::string> type_title_label_;
	gui_adaptor::value<std::string> faction_title_label_;
	gui_adaptor::value<std::string> team_title_label_;
	gui_adaptor::value<std::string> colour_title_label_;
	gui_adaptor::value<std::string> gold_title_label_;
	gui_adaptor::value<std::string> income_title_label_;
	gui_adaptor::value<std::string> launch_;
	gui_adaptor::value<std::string> cancel_;
	gui_adaptor::value<std::string> title_;

	connect* connect_;

	game_display& disp_;

	ui::result res_;
};

class connect_ui_gui1 : public connect_ui {
	friend class side_ui_gui1;
public:
	using ui::disp;
	connect_ui_gui1(game_display& disp, const config& game_config,
		chat& c, config& gamelist, const mp_game_settings& params);

	const config& game_config() { return game_config(); }

	void hide_children(bool hide=true);
	gui::scrollpane& scrollpane() { return scroll_pane_; }

	void show_sides(const std::vector<connect_side>& sides);

	void setup_adaptors();
	void layout_children(const SDL_Rect& rect);
	void process_event();

	side_ui* next_side_ui();
private:
	// Widgets
	gui::label w_waiting_label_;
	gui::label w_type_title_label_;
	gui::label w_faction_title_label_;
	gui::label w_team_title_label_;
	gui::label w_colour_title_label_;
	gui::label w_gold_title_label_;
	gui::label w_income_title_label_;

	gui::button w_launch_;
	gui::button w_cancel_;

	gui::scrollpane scroll_pane_;
	gui::drop_group_manager_ptr combo_control_group_;

	std::list<side_ui_gui1> sides_;
};


} // namespace mp

#endif
