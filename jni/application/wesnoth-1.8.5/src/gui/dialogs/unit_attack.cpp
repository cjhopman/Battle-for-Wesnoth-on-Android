/* $Id$ */
/*
   Copyright (C) 2010 - 2011 by Mark de Wever <koraq@xs4all.nl>
   Part of the Battle for Wesnoth Project http://www.wesnoth.org/

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY.

   See the COPYING file for more details.
*/

#define GETTEXT_DOMAIN "wesnoth-lib"

#include "gui/dialogs/unit_attack.hpp"
#include "gui/dialogs/unit_panel.hpp"

#include "gui/widgets/image.hpp"
#ifdef GUI2_EXPERIMENTAL_LISTBOX
#include "gui/widgets/list.hpp"
#else
#include "gui/widgets/listbox.hpp"
#endif
#include "gui/widgets/settings.hpp"
#include "gui/widgets/window.hpp"
#include "gui/widgets/grid.hpp"
#include "gui/widgets/button.hpp"


#include "unit.hpp"
#include "team.hpp"

#include "marked-up_text.hpp"
#include "sdl_utils.hpp"

#include <boost/bind.hpp>

#include <sstream>

namespace gui2 {

/*WIKI
 * @page = GUIWindowDefinitionWML
 * @order = 2_unit_attack
 *
 * == Unit attack ==
 *
 * This shows the dialog for attacking units.
 *
 * @begin{table}{dialog_widgets}
 * attacker_portrait & & image   & o & Shows the portrait of the attacking unit. $
 * attacker_icon     & & image   & o & Shows the icon of the attacking unit. $
 * attacker_name     & & control & o & Shows the name of the attacking unit. $
 *
 * defender_portrait & & image   & o & Shows the portrait of the defending unit. $
 * defender_icon     & & image   & o & Shows the icon of the defending unit. $
 * defender_name     & & control & o & Shows the name of the defending unit. $
 *
 *
 * weapon_list       & & listbox & m & The list with weapons to choos from. $
 * -attacker_weapon  & & control & o & The weapon for the attacker to use. $
 * -defender_weapon  & & control & o & The weapon for the defender to use. $
 *
 * @end{table}
 */

tunit_attack::tunit_attack(
		  const unit_map::iterator& attacker_itor
		, const unit_map::iterator& defender_itor
		, const std::vector<battle_context>& weapons
		, const int best_weapon)
	: selected_weapon_(-1)
	, attacker_itor_(attacker_itor)
	, defender_itor_(defender_itor)
	, weapons_(weapons)
	, best_weapon_(best_weapon)
	, ap_displayer_(weapons, attacker_itor->first, defender_itor->first)
{
}

twindow* tunit_attack::build_window(CVideo& video)
{
	return build(video, get_id(UNIT_ATTACK));
}


template<class T>
static void set_label(
		  tgrid* grid 
		, const std::string& id
		, const std::string& label
		, const bool markup = false)
{
	T* widget = find_widget<T>(grid, id, false, false);
	if(widget) {
		widget->set_label(label);
		widget->set_use_markup(markup);
	}
}

static void set_attacker_info(twindow& w, unit& u)
{
	tgrid* g = find_widget<tgrid>(&w, "attacker", false, false);
	assert(g);
	set_unit_info(g, u);
}

static void set_defender_info(twindow& w, unit& u)
{
	tgrid* g = find_widget<tgrid>(&w, "defender", false, false);
	assert(g);
	set_unit_info(g, u);
}

static void set_weapon_info(twindow& window
		, const std::vector<battle_context>& weapons
		, const int best_weapon)
{
	tlistbox& weapon_list =
			find_widget<tlistbox>(&window, "weapon_list", false);
	window.keyboard_capture(&weapon_list);

	const config empty;
	attack_type no_weapon(empty);

	foreach(const battle_context& weapon, weapons) {
		const battle_context::unit_stats& attacker =
				weapon.get_attacker_stats();

		const battle_context::unit_stats& defender =
				weapon.get_defender_stats();

		const attack_type& attacker_weapon = attack_type(*attacker.weapon);
		const attack_type& defender_weapon = attack_type(
				defender.weapon ? *defender.weapon : no_weapon);

		std::map<std::string, string_map> data;
		string_map item;

		std::stringstream ss;

		data["attacker_weapon"]["label"] = attacker_weapon.name();
		data["attacker_weapon_image"]["label"] = attacker_weapon.icon();

		ss.str("");
		ss << attacker.damage << "-" << attacker.num_blows << " " << attacker_weapon.weapon_specials();
		data["attacker_attacks"]["label"] = ss.str();

		data["defender_weapon"]["label"] = defender_weapon.name();
		data["defender_weapon_image"]["label"] = defender_weapon.icon();

		ss.str("");
		ss << defender.damage << "-" << defender.num_blows << " " << defender_weapon.weapon_specials();
		data["defender_attacks"]["label"] = ss.str();

		data["weapon_range"]["label"] = "- " + attacker_weapon.range() + " -";

		weapon_list.add_row(data);


		ss.str("");
		ss << "<span foreground=\"#" << font::color2hexa(int_to_color(game_config::red_to_green(attacker.chance_to_hit))) << "\">"
			<< attacker.chance_to_hit << "%</span>";
		set_label<tcontrol>(weapon_list.get_row_grid(weapon_list.get_item_count() - 1), "attacker_cth", ss.str(), true);

		ss.str("");
		ss << "<span foreground=\"#" << font::color2hexa(int_to_color(game_config::red_to_green(defender.chance_to_hit))) << "\">"
			<< defender.chance_to_hit << "%</span>";
		set_label<tcontrol>(weapon_list.get_row_grid(weapon_list.get_item_count() - 1), "defender_cth", ss.str(), true);

	}

	assert(best_weapon < static_cast<int>(weapon_list.get_item_count()));
	weapon_list.select_row(best_weapon);
}

void tunit_attack::pre_show(CVideo& /*video*/, twindow& window)
{
	set_attacker_info(window, attacker_itor_->second);
	set_defender_info(window, defender_itor_->second);

	selected_weapon_ = -1;
	set_weapon_info(window, weapons_, best_weapon_);

	if(tbutton* button = find_widget<tbutton>(&window, "damage_calculations", false, false)) {
		button->connect_signal_mouse_left_click(boost::bind(
					&tunit_attack::show_damage_calculations
					, this
					, boost::ref(window)));
	}
}

void tunit_attack::set_selected_weapon(twindow& window) {
	selected_weapon_ = find_widget<tlistbox>(&window, "weapon_list", false)
		.get_selected_row();
}

void tunit_attack::post_show(twindow& window)
{
	if(get_retval() == twindow::OK) {
		set_selected_weapon(window);
	}
}

void tunit_attack::show_damage_calculations(twindow& window) {
	set_selected_weapon(window);
	ap_displayer_.button_pressed(get_selected_weapon());
}

} // namespace gui2

