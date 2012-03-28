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

/**
 * @file multiplayer_connect.cpp
 * Prepare to join a multiplayer-game.
 */

#include "global.hpp"

#include "ai/configuration.hpp"
#include "dialogs.hpp"
#include "foreach.hpp"
#include "game_display.hpp"
#include "gettext.hpp"
#include "log.hpp"
#include "map.hpp"
#include "multiplayer_connect.hpp"
#include "multiplayer_connect_ui.hpp"
#include "savegame.hpp"
#include "statistics.hpp"
#include "unit_id.hpp"
#include "wml_separators.hpp"
#include "formula_string_utils.hpp"
#include "tod_manager.hpp"

#include <boost/lambda/bind.hpp>
#include <boost/lambda/lambda.hpp>
#include <boost/lambda/algorithm.hpp>
using namespace boost::lambda;
using namespace gui_adaptor;

static lg::log_domain log_config("config");
#define LOG_CF LOG_STREAM(info, log_config)
#define WRN_CF LOG_STREAM(warn, log_config)
#define ERR_CF LOG_STREAM(err, log_config)

namespace {
	std::string income_string(int income) {
		std::stringstream buf;
		if(income < 0) {
			buf << _("(") << income << _(")");
		} else if(income > 0) {
			buf << _("+") << income;
		} else {
			buf << _("Normal");
		}
		return buf.str();
	}
} // anonymous namespace

namespace mp {

void side_ui::set_gold(int val) {
	slider_gold_.set_value(val);
	label_gold_.set_text(lexical_cast_default<std::string>(val, "0"));
}
void side_ui::set_income(int val) {
	slider_income_.set_value(val);
	label_income_.set_text(income_string(val));
}

void side_ui::set_team_list(const std::vector<std::string>& teams, int index) {
	combo_team_.set_items(teams);
	set_team(index);
}
void side_ui::set_team(int index) {
	combo_team_.set_selected(index);
}

void side_ui::set_colour_list(const std::vector<std::string>& colours, int index) {
	combo_colour_.set_items(colours);
	set_colour(index);
}
void side_ui::set_colour(int index) {
	combo_colour_.set_selected(index);
}

void side_ui::set_controller_list(const std::vector<std::string>& controllers, int index) {
	combo_controller_.set_items(controllers);
	set_controller(index);
}
void side_ui::set_controller(int index) {
	combo_controller_.set_selected(index);
}

void side_ui::set_leader_list(const std::vector<std::string>& leaders, int index) {
	combo_leader_.set_items(leaders);
	set_leader(index);
}
void side_ui::set_leader(int index) {
	combo_leader_.set_selected(index);
}

void side_ui::set_gender_list(const std::vector<std::string>& genders, int index) {
	combo_gender_.set_items(genders);
	set_gender(index);
}
void side_ui::set_gender(int index) {
	combo_gender_.set_selected(index);
}

void side_ui::set_ai_list(const std::vector<std::string>& ais, int index) {
	combo_ai_algorithm_.set_items(ais);
	set_ai(index);
}
void side_ui::set_ai(int index) {
	combo_ai_algorithm_.set_selected(index);
}

void side_ui::set_faction_list(const std::vector<std::string>& factions, int index) {
	combo_faction_.set_items(factions);
	set_faction(index);
}
void side_ui::set_faction(int index) {
	combo_faction_.set_selected(index);
}

void side_ui::set_player(const std::string& player) {
	orig_controller_.set_text(player);
}
void side_ui::set_player_number(const std::string& player) {
	player_number_.set_text(player);
}

void side_ui::enable_gold(bool enable) {
	slider_gold_.enable(enable);
	label_gold_.enable(enable);
}
void side_ui::enable_income(bool enable) {
	slider_income_.enable(enable);
	label_income_.enable(enable);
}
void side_ui::enable_team(bool enable) {
	combo_team_.enable(enable);
}
void side_ui::enable_colour(bool enable) {
	combo_colour_.enable(enable);
}
void side_ui::enable_faction(bool enable) {
	combo_faction_.enable(enable);
}
void side_ui::enable_leader(bool enable) {
	combo_leader_.enable(enable);
}
void side_ui::enable_gender(bool enable) {
	combo_gender_.enable(enable);
}

void side_ui::enable_llm(leader_list_manager& llm, bool enable) {
	llm.gui()->set_leader_combo(enable ? combo_leader_ : gui_adaptor::list());
	llm.gui()->set_gender_combo(enable ? combo_gender_ : gui_adaptor::list());
}

void side_ui::hide_ai(bool hide) {
	combo_ai_algorithm_.hide(hide);
}
void side_ui::hide_orig_controller(bool hide) {
	orig_controller_.hide(hide);
}
void side_ui::hide_ai_algorithm_combo(bool hide) {
	bool ai = side_->should_show_ai_combo();
	combo_ai_algorithm_.hide(hide || !ai);
	orig_controller_.hide(hide || ai);
}

side_ui_gui1::side_ui_gui1(connect_ui_gui1* parent) :
	w_combo_controller_(new gui::combo_drag(parent->disp(), std::vector<std::string>(), parent->combo_control_group_)),

	w_combo_ai_algorithm_(parent->disp(), std::vector<std::string>()),
	w_combo_faction_(parent->disp(), std::vector<std::string>()),
	w_combo_leader_(parent->disp(), std::vector<std::string>()),
	w_combo_gender_(parent->disp(), std::vector<std::string>()),
	w_combo_team_(parent->disp(), std::vector<std::string>()),
	w_combo_colour_(parent->disp(), std::vector<std::string>()),

	w_player_number_(parent->video(), "",
	               font::SIZE_LARGE, font::LOBBY_COLOUR),
	w_orig_controller_(parent->video(), "", font::SIZE_SMALL),
	w_label_gold_(parent->video(), "100", font::SIZE_SMALL, font::LOBBY_COLOUR),
	w_label_income_(parent->video(), _("Normal"), font::SIZE_SMALL, font::LOBBY_COLOUR),

	w_slider_gold_(parent->video()),
	w_slider_income_(parent->video())
{
	init();
	setup_adaptors();
}

side_ui_gui1::side_ui_gui1(const side_ui_gui1& o) :
	side_ui(),
	w_combo_controller_(o.w_combo_controller_),

	w_combo_ai_algorithm_(o.w_combo_ai_algorithm_),
	w_combo_faction_(o.w_combo_faction_),
	w_combo_leader_(o.w_combo_leader_),
	w_combo_gender_(o.w_combo_gender_),
	w_combo_team_(o.w_combo_team_),
	w_combo_colour_(o.w_combo_colour_),

	w_player_number_(o.w_player_number_),
	w_orig_controller_(o.w_orig_controller_),
	w_label_gold_(o.w_label_gold_),
	w_label_income_(o.w_label_income_),

	w_slider_gold_(o.w_slider_gold_),
	w_slider_income_(o.w_slider_income_)
{
	set_side(o.side_);
	setup_adaptors();
}

void side_ui_gui1::setup_adaptors() {
	combo_controller_.set_widget(w_combo_controller_.get());
	combo_ai_algorithm_.set_widget(&w_combo_ai_algorithm_);
	combo_faction_.set_widget(&w_combo_faction_);
	combo_leader_.set_widget(&w_combo_leader_);
	combo_gender_.set_widget(&w_combo_gender_);
	combo_team_.set_widget(&w_combo_team_);
	combo_colour_.set_widget(&w_combo_colour_);

	player_number_.set_widget(&w_player_number_);
	orig_controller_.set_widget(&w_orig_controller_);
	label_gold_.set_widget(&w_label_gold_);
	label_income_.set_widget(&w_label_income_);

	slider_gold_.set_widget(&w_slider_gold_);
	slider_income_.set_widget(&w_slider_income_);
}

void side_ui_gui1::init() {
	w_slider_gold_.set_min(20);
	w_slider_gold_.set_max(800);
	w_slider_gold_.set_increment(25);
	w_slider_gold_.set_measurements(80, 16);

	w_slider_income_.set_min(-2);
	w_slider_income_.set_max(18);
	w_slider_income_.set_increment(1);
	w_slider_income_.set_measurements(50, 16);
}

void side_ui_gui1::add_widgets_to_scrollpane(gui::scrollpane& pane, int pos)
{
	pane.add_widget(&w_player_number_,     0, 5 + pos);
	pane.add_widget(w_combo_controller_.get(), 20, 5 + pos);
	pane.add_widget(&w_orig_controller_,  20 + (w_combo_controller_->width() - w_orig_controller_.width()) / 2,
									    35 + pos + (w_combo_leader_.height() - w_orig_controller_.height()) / 2);
	pane.add_widget(&w_combo_ai_algorithm_, 20, 35 + pos);
	pane.add_widget(&w_combo_faction_, 135, 5 + pos);
	pane.add_widget(&w_combo_leader_,  135, 35 + pos);
	pane.add_widget(&w_combo_gender_,  250, 35 + pos);
	pane.add_widget(&w_combo_team_,    250, 5 + pos);
	pane.add_widget(&w_combo_colour_,  365, 5 + pos);
	pane.add_widget(&w_slider_gold_,   475, 5 + pos);
	pane.add_widget(&w_label_gold_,    475, 35 + pos);
	pane.add_widget(&w_slider_income_, 475 + w_slider_gold_.width(),  5 + pos);
	pane.add_widget(&w_label_income_,  475 + w_slider_gold_.width(), 35 + pos);
}

bool side_ui_gui1::process_event()
{
	int drop_target = w_combo_controller_->get_drop_target();
	if (drop_target > -1) {
		side_->swap_player(drop_target);
	} else if (w_combo_controller_->changed() && w_combo_controller_->selected() >= 0) {
		side_->set_controller(w_combo_controller_->selected());
	}

	w_combo_controller_->hide(false);

	if (w_combo_colour_.changed() && w_combo_colour_.selected() >= 0) {
		side_->set_colour(w_combo_colour_.selected());
	}
	if (w_combo_faction_.changed() && w_combo_faction_.selected() >= 0) {
		side_->set_faction(w_combo_faction_.selected());
	}
	if (w_combo_ai_algorithm_.changed() && w_combo_ai_algorithm_.selected() >= 0) {
		side_->set_ai(w_combo_ai_algorithm_.selected());
	}
	if (w_combo_leader_.changed() && w_combo_leader_.selected() >= 0) {
		side_->update_gender_list();
	}
	if (w_combo_gender_.changed() && w_combo_gender_.selected() >= 0) {
		side_->change();
	}
	if (w_combo_team_.changed() && w_combo_team_.selected() >= 0) {
		side_->set_team(w_combo_team_.selected());
	}
	if (w_slider_gold_.value_change()) {
		int g = w_slider_gold_.value();
		w_label_gold_.set_text(lexical_cast_default<std::string>(g, "0"));
		side_->set_gold(g);
	}
	if (w_slider_income_.value_change()) {
		int i = w_slider_income_.value();
		w_label_income_.set_text(income_string(i));
		side_->set_income(i);
	}
	return side_->changed();
}

connect_ui::connect_ui(game_display& disp, const config& game_config,
	chat& c, config& gamelist, const mp_game_settings& params) : 
	mp::ui(disp, _("Game Lobby: ") + params.name, game_config, c, gamelist),
	disp_(disp)
{
}

void connect_ui::hide_gold_title(bool hide) {
	gold_title_label_.hide(hide);
}
void connect_ui::hide_income_title(bool hide) {
	income_title_label_.hide(hide);
}

void connect_ui::enable_launch(bool enable) {
	launch_.enable(enable);
}

void connect_ui::set_waiting_label(const std::string& str) {
	waiting_label_.set_text(str);
}

void connect_ui::append_to_title(const std::string& str) {
	title_.set(title_.get() + str);
}

connect_ui_gui1::connect_ui_gui1(game_display& disp, const config& game_config,
		chat& c, config& gamelist, const mp_game_settings& params) :
	connect_ui(disp, game_config, c, gamelist, params),
	w_waiting_label_(video(), "", font::SIZE_SMALL, font::LOBBY_COLOUR),
	w_type_title_label_(video(), _("Player/Type"), font::SIZE_SMALL, font::LOBBY_COLOUR),
	w_faction_title_label_(video(), _("Faction"), font::SIZE_SMALL, font::LOBBY_COLOUR),
	w_team_title_label_(video(), _("Team/Gender"), font::SIZE_SMALL, font::LOBBY_COLOUR),
	w_colour_title_label_(video(), _("Color"), font::SIZE_SMALL, font::LOBBY_COLOUR),
	w_gold_title_label_(video(), _("Gold"), font::SIZE_SMALL, font::LOBBY_COLOUR),
	w_income_title_label_(video(), _("Income"), font::SIZE_SMALL, font::LOBBY_COLOUR),
	w_launch_(video(), _("I'm Ready")),
	w_cancel_(video(), _("Cancel")),
	scroll_pane_(video()),
	combo_control_group_(new gui::drop_group_manager()),
	sides_()
{
	setup_adaptors();
}

void connect_ui_gui1::setup_adaptors() {
	waiting_label_.set_widget(&w_waiting_label_);
	type_title_label_.set_widget(&w_type_title_label_);
	faction_title_label_.set_widget(&w_faction_title_label_);
	team_title_label_.set_widget(&w_team_title_label_);
	colour_title_label_.set_widget(&w_colour_title_label_);
	gold_title_label_.set_widget(&w_gold_title_label_);
	income_title_label_.set_widget(&w_income_title_label_);
	launch_.set_widget(&w_launch_);
	cancel_.set_widget(&w_cancel_);
	connect_ui::title_.set_widget(&title());
}

side_ui* connect_ui_gui1::next_side_ui() {
	sides_.push_back(side_ui_gui1(this));
	return &sides_.back();
}

void connect_ui_gui1::process_event() {
	if (w_cancel_.pressed()) {
		if(network::nconnections() > 0) {
			config cfg;
			cfg.add_child("leave_game");
			network::send_data(cfg, 0, true);
		}

		set_result(QUIT);
		return;
	}

	if (w_launch_.pressed()) {
		connect_->try_launch();
	}

	bool changed = false;
	foreach(side_ui_gui1& s, sides_) {
		changed |= s.process_event();
	}

	if (changed) {
		connect_->full_update();
	}
}

void connect_ui_gui1::hide_children(bool hide)
{
	ui::hide_children(hide);

	if (!connect_->params().saved_game) {
		gold_title_label_.hide(hide);
		income_title_label_.hide(hide);
	}

	waiting_label_.hide(hide);
	faction_title_label_.hide(hide);
	team_title_label_.hide(hide);
	colour_title_label_.hide(hide);
	launch_.hide(hide);
	cancel_.hide(hide);

	// Hiding the scrollpane automatically hides its contents
	scroll_pane_.hide(hide);

	foreach(side_ui_gui1& s, sides_) {
		s.hide_ai_algorithm_combo(hide);
	}
}

void connect_ui::process_network_data(const config& data, const network::connection sock) {
	ui::process_network_data(data, sock);
	connect_->process_network_data(data, sock);
}

void connect_ui::process_network_error(network::error& error) {
	connect_->process_network_error(error);
}

bool connect_ui::accept_connections() {
	return connect_->accept_connections();
}

void connect_ui::process_network_connection(const network::connection sock) {
	ui::process_network_connection(sock);
	connect_->process_network_connection(sock);
}

void connect_ui_gui1::layout_children(const SDL_Rect& rect)
{
	ui::layout_children(rect);

	SDL_Rect ca = client_area();

	gui::button* left_button = &w_launch_;
	gui::button* right_button = &w_cancel_;
#ifdef OK_BUTTON_ON_RIGHT
	std::swap(left_button,right_button);
#endif
	size_t left = ca.x;
	size_t right = ca.x + ca.w;
	size_t top = ca.y;
	size_t bottom = ca.y + ca.h;

	// Buttons
	right_button->set_location(right  - right_button->width(),
	                           bottom - right_button->height());
	left_button->set_location(right  - right_button->width() - left_button->width() - gui::ButtonHPadding,
	                          bottom - left_button->height());

	w_waiting_label_.set_location(left + 8, bottom-left_button->height() + 4);
	w_type_title_label_.set_location(left+30, top+35);
	w_faction_title_label_.set_location((left+145), top+35);
	w_team_title_label_.set_location((left+260), top+35);
	w_colour_title_label_.set_location((left+375), top+35);
	w_gold_title_label_.set_location((left+493), top+35);
	w_income_title_label_.set_location((left+560), top+35);

	SDL_Rect scroll_pane_rect;
	scroll_pane_rect.x = ca.x;
	scroll_pane_rect.y = ca.y + 50;
	scroll_pane_rect.w = ca.w;
	scroll_pane_rect.h = w_launch_.location().y - scroll_pane_rect.y - gui::ButtonVPadding;

	scroll_pane_.set_location(scroll_pane_rect);
}

void connect_ui_gui1::show_sides(const std::vector<connect_side>& sides) {
	int i = 0, offset = 0;
	foreach(side_ui_gui1& sui, sides_) {
		if(sides[i++].allow_player()) {
			const int spos = 60 * offset++;
			sui.add_widgets_to_scrollpane(scrollpane(), spos);
		}
	}
}

} // namespace mp
