/* $Id: mp_create_game.cpp 48450 2011-02-08 20:55:18Z mordante $ */
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

#define GETTEXT_DOMAIN "wesnoth-lib"

#include "gui/dialogs/mp_create_game.hpp"

#include "gui/dialogs/field.hpp"
#include "gui/dialogs/helper.hpp"
#include "gui/dialogs/message.hpp"
#include "gui/widgets/button.hpp"
#include "gui/widgets/integer_selector.hpp"
#include "gui/widgets/listbox.hpp"
#include "gui/widgets/minimap.hpp"
#include "gui/widgets/settings.hpp"
#include "gui/widgets/slider.hpp"
#include "gui/widgets/text_box.hpp"

#include "foreach.hpp"
#include "formula_string_utils.hpp"
#include "game_display.hpp"
#include "game_preferences.hpp"
#include "gettext.hpp"
#include "log.hpp"
#include "map.hpp"
#include "map_exception.hpp"
#include "map_create.hpp"
#include "../../settings.hpp"

#include <boost/bind.hpp>

#include <map>
#include <algorithm>
#include <iterator>
#include <sstream>
#include <memory>

static lg::log_domain log_config("config");
#define ERR_CF LOG_STREAM(err, log_config)

namespace gui2 {

namespace {
std::string default_game_name() {
	utils::string_map i18n_symbols;
	i18n_symbols["login"] = preferences::login();
	return vgettext("$login's game", i18n_symbols);
}
int convert_turns_value(int val) {
	return (val >= 0 && val < ::settings::turns_max) ? val : -1;
}
} // anonymous namespace


tmp_create_game::tmp_create_game(game_display& disp, const config& cfg, mp::ui::result& res, mp_game_settings& parameters) :
	cfg_(cfg),

	countdown_(register_bool("time_limit", true,
		preferences::countdown,
		preferences::set_countdown)),
	use_map_settings_(register_bool("use_map_settings", true,
		preferences::use_map_settings,
		preferences::set_use_map_settings,
		boost::bind(&tmp_create_game::update_map_settings, this))),
	start_time_(register_bool("random_start_time", true,
		preferences::random_start_time,
		preferences::set_random_start_time)),
	fog_(register_bool("fog", true,
		preferences::fog,
		preferences::set_fog)),
	shroud_(register_bool("shroud", true,
		preferences::shroud,
		preferences::set_shroud)),
	allow_observers_(register_bool("allow_observers", true,
		preferences::allow_observers,
		preferences::set_allow_observers)),
	share_view_(register_bool("share_view", true)),
	share_maps_(register_bool("share_maps", true)),

	turns_(register_integer("turn_count", true,
		preferences::turns ,
		boost::bind(preferences::set_turns, boost::bind(convert_turns_value, _1)))),
	gold_(register_integer("village_gold", true,
		preferences::village_gold ,
		preferences::set_village_gold)),
	experience_(register_integer("experience_modifier", true,
		preferences::xp_modifier ,
		preferences::set_xp_modifier)),
	countdown_init_time_(register_integer("countdown_init_time", true,
		preferences::countdown_init_time,
		preferences::set_countdown_init_time)),
	countdown_reservoir_time_(register_integer("countdown_reservoir_time", true,
		preferences::countdown_reservoir_time ,
		preferences::set_countdown_reservoir_time)),
	countdown_turn_bonus_(register_integer("countdown_turn_bonus", true,
		preferences::countdown_turn_bonus ,
		preferences::set_countdown_turn_bonus)),
	countdown_action_bonus_(register_integer("countdown_action_bonus", true,
		preferences::countdown_action_bonus ,
		preferences::set_countdown_action_bonus)),

	game_name_(register_text("game_name", true, default_game_name)),
	game_password_(register_text("game_password", true)),

	res_(res),
	parameters_(parameters),
	disp_(disp),
	window_(NULL)
{
}

twindow* tmp_create_game::build_window(CVideo& video)
{
	return (window_ = build(video, get_id(MP_CREATE_GAME)));
}

void tmp_create_game::pre_show(CVideo& /*video*/, twindow& window)
{
	window.set_escape_disabled(true);
	window.set_enter_disabled(true);

	find_widget<tminimap>(window_, "minimap", false).set_config(&cfg_);

	tlistbox& map_list = find_widget<tlistbox>(window_, "map_list", false);
	map_list.set_callback_value_change(boost::bind(&tmp_create_game::update_map, this));

	// Load option (might turn it into a button later).
	std::map<std::string, string_map> data;
	data["name"]["label"] = _("Load Game");
	map_list.add_row(data);

	if (tslider* turns = find_widget<tslider>(window_, "turn_count", false, false)) {
		turns->set_step_size(std::max(1, ::settings::turns_step));
		turns->set_maximum_value(::settings::turns_max);
		turns->set_minimum_value(::settings::turns_min);
	}

	// User maps
/*	FIXME implement user maps
	std::vector<std::string> maps;
	get_files_in_dir(get_user_data_dir() + "/editor/maps", &maps, NULL, FILE_NAME_ONLY);

	foreach(const std::string& map, maps) {
		std::map<std::string, t_string> item;
		item.insert(std::make_pair("label", map));
		list->add_row(item);
	}
*/

	if (tbutton* random_regenerate =
			find_widget<tbutton>(window_, "random_regenerate", false, false)) {
		random_regenerate->connect_click_handler(boost::bind(&tmp_create_game::random_regenerate_cb, this));
	}
	if (tbutton* random_regenerate =
			find_widget<tbutton>(window_, "random_settings", false, false)) {
		random_regenerate->connect_click_handler(boost::bind(&tmp_create_game::random_settings_cb, this));
	}
	// Standard maps
	foreach (const config &map, cfg_.child_range("multiplayer"))
	{
		if (utils::string_bool(map["allow_new_game"], true)) {
			std::map<std::string, string_map> data;
			data["name"]["label"] = ellipsize(map["name"], 28);
			map_list.add_row(data);
		}
	}

	if (size_t(preferences::map()) < map_list.get_item_count()) {
		map_list.select_row(preferences::map());
		map_list.show_selected_row();
	}

	if (tlistbox* era_list = 
			find_widget<tlistbox>(window_, "era_list", false, false)) {
		foreach(const config& era, cfg_.child_range("era")) {
			std::map<std::string, string_map> data;
			data["name"]["label"] = ellipsize(era["name"], 20);
			era_list->add_row(data);
		}
	}

	update_map();

}

void tmp_create_game::post_show(twindow& window)
{
	if(get_retval() == twindow::OK) {
		res_ = mp::ui::CREATE;

		parameters_.name = game_name_->get_widget_value(window);

		int era_idx = get_value_or_default<tlistbox>(&window, "era_list", 0);
		config::const_child_iterator it = cfg_.child_range("era").first;
		std::advance(it, era_idx);
		parameters_.mp_era = (*it)["id"];

		parameters_.use_map_settings = use_map_settings_->get_widget_value(window);
		parameters_.allow_observers = allow_observers_->get_widget_value(window);
		parameters_.mp_countdown_init_time = countdown_init_time_->get_widget_value(window);
		parameters_.mp_countdown_turn_bonus = countdown_turn_bonus_->get_widget_value(window);
		parameters_.mp_countdown_reservoir_time = countdown_reservoir_time_->get_widget_value(window);
		parameters_.mp_countdown_action_bonus = countdown_action_bonus_->get_widget_value(window);
		parameters_.village_gold = gold_->get_widget_value(window);
		parameters_.xp_modifier = experience_->get_widget_value(window);
		parameters_.random_start_time = start_time_->get_widget_value(window);
		parameters_.fog_game = fog_->get_widget_value(window);
		parameters_.shroud_game = shroud_->get_widget_value(window);
		parameters_.share_view = share_view_->get_widget_value(window);
		parameters_.share_maps = share_maps_->get_widget_value(window);
		parameters_.password = game_password_->get_widget_value(window);

		parameters_.num_turns = convert_turns_value(turns_->get_widget_value(window));

		int map_idx = get_value_or_default<tlistbox>(window_, "map_list", 0);
		preferences::set_map(map_idx);
	} else {
		res_ = mp::ui::QUIT;
	}
}

void tmp_create_game::regenerate_map() {
	parameters_.scenario_data = generator_->create_scenario(std::vector<std::string>());
	if (!parameters_.scenario_data["error_message"].empty())
		gui2::show_message(disp_.video(), "map generation error", parameters_.scenario_data["error_message"]);
	parameters_.scenario_data["modify_placing"] = "true";
}

void tmp_create_game::update_map()
{
	int map_idx = get_value_or_default<tlistbox>(window_, "map_list", 0);

	twidget* regen = find_widget<twidget>(window_, "random_regenerate", false, false);
	twidget* settings = find_widget<twidget>(window_, "random_settings", false, false);
	if (regen) regen->set_visible(twidget::HIDDEN);
	if (settings) settings->set_visible(twidget::HIDDEN);

	if (map_idx == 0) {
		// Load Game
		parameters_.saved_game = true;
		parameters_.scenario_data.clear();
	} else {
		parameters_.saved_game = false;
		config::const_child_itors children = cfg_.child_range("multiplayer");
		std::advance(children.first, map_idx - 1);
		const config &level = *children.first;
		parameters_.scenario_data = level;

		if (level["map_generation"].empty()) {
			generator_.reset(NULL);
		} else {
			generator_.reset(create_map_generator(level["map_generation"], level.child("generator")));
			if (regen) regen->set_visible(twidget::VISIBLE);
			if (settings) settings->set_visible(generator_->allow_user_config() ? twidget::VISIBLE : twidget::HIDDEN);
			regenerate_map();
		}
	}

	update_map_settings();
}

void tmp_create_game::random_settings_cb() {
	generator_->user_config(disp_);
	regenerate_map();
	update_map_settings();
}

void tmp_create_game::random_regenerate_cb() {
	regenerate_map();
	update_map_settings();
}

void tmp_create_game::update_map_settings()
{
	const bool enable_settings = !parameters_.saved_game && !use_map_settings_->get_widget_value(*window_);

	fog_->widget_set_enabled(*window_, enable_settings, false);
	shroud_->widget_set_enabled(*window_, enable_settings, false);
	start_time_->widget_set_enabled(*window_, enable_settings, false);

	turns_->widget_set_enabled(*window_, enable_settings, false);
	gold_->widget_set_enabled(*window_, enable_settings, false);
	experience_->widget_set_enabled(*window_, enable_settings, false);

	tminimap& minimap = find_widget<tminimap>(window_, "minimap", false);

	if (parameters_.saved_game) {
		minimap.set_map_data("");
		if (tcontrol* w = find_widget<tcontrol>(window_, "map_players", false, false)) {
			w->set_label("Players: ");
		}

		if (tcontrol* w = find_widget<tcontrol>(window_, "map_size", false, false)) {
			w->set_label("Size: ");
		}
		return;
	}

	minimap.set_map_data(parameters_.scenario_data["map_data"]);

	const bool use_map_settings = use_map_settings_->get_widget_value(*window_);
	const config& scenario = parameters_.scenario_data;

	std::auto_ptr<gamemap> map;
	try {
		map.reset(new gamemap(cfg_, scenario["map_data"]));
	} catch(incorrect_map_format_exception& e) {
		ERR_CF << "map could not be loaded: " << e.msg_ << "\n";
		return;
	} catch(twml_exception& e) {
		ERR_CF <<  "map could not be loaded: " << e.dev_message << '\n';
		return;
	}

	const int map_positions = map.get() != NULL ? map->num_valid_starting_positions() : 0;

	for (int pos = parameters_.scenario_data.child_count("side"); pos < map_positions; ++pos) {
		config& side = parameters_.scenario_data.add_child("side");
		side["side"] = lexical_cast<std::string>(pos+1);
		side["team_name"] = lexical_cast<std::string>(pos+1);
		side["canrecruit"] = "yes";
		side["controller"] = "human";
	}

	config::const_child_itors sides = scenario.child_range("side");
	using namespace boost;
	int nsides = 0;
	foreach(const config& c, sides) {
		nsides += utils::string_bool(c["allow_player"], true);
	}

	if (tcontrol* w = find_widget<tcontrol>(window_, "map_players", false, false)) {
		std::stringstream players;
		players << _("Players: ") << nsides;
		w->set_label(players.str());
	}

	if (tcontrol* w = find_widget<tcontrol>(window_, "map_size", false, false)) {
		std::stringstream map_size;
		map_size << _("Size: ") << map.get()->w() << "x" << map.get()->h();
		w->set_label(map_size.str());
	}

	if(use_map_settings) {
		if (sides.first != sides.second) {
			const config& side = *sides.first;
			fog_->set_widget_value(*window_, ::settings::use_fog(side["fog"]));
			shroud_->set_widget_value(*window_, ::settings::use_shroud(side["shroud"]));
			start_time_->set_widget_value(*window_, ::settings::use_random_start_time(scenario["random_start_time"]));

			turns_->set_widget_value(*window_, ::settings::get_turns(scenario["turns"]));
			gold_->set_widget_value(*window_, ::settings::get_village_gold(side["village_gold"]));
			experience_->set_widget_value(*window_, ::settings::get_xp_modifier(scenario["experience_modifier"]));
		}
	}
}

} // namespace gui2
