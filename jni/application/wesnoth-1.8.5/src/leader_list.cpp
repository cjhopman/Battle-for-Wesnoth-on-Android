/* $Id: leader_list.cpp 48450 2011-02-08 20:55:18Z mordante $ */
/*
   Copyright (C) 2007 - 2011
   Part of the Battle for Wesnoth Project http://www.wesnoth.org

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License 2
   or at your option any later version.
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY.

   See the COPYING file for more details.
*/

/**
 * @file leader_list.cpp
 * Manage the selection of a leader, and select his/her gender.
 */

#include "global.hpp"

#include "foreach.hpp"
#include "gettext.hpp"
#include "leader_list.hpp"
#include "race.hpp"
#include "wml_separators.hpp"
#include "widgets/combo.hpp"

#include <boost/scope_exit.hpp>

#include <iterator>
#include <algorithm>

const std::string leader_list_manager::random_enemy_picture("units/random-dice.png");

leader_list_manager::leader_list_manager(const std::vector<const config *> &side_list, leader_list_gui gui) :
	leaders_(),
	genders_(),
	leader_ids_(),
	gender_ids_(),
	side_list_(side_list),
	leader_enabled_(false),
	gender_enabled_(false),
	colour_(0),
	gui_(gui)
{
}

void leader_list_manager::update_leader_gui(int index) {
	gui_.update_leader_list(leader_enabled_, leaders_, index);
}

void leader_list_manager::update_leader_list(int side_index)
{
	const config& side = *side_list_[side_index];

	leader_ids_.clear();

	int default_index = 0;
	leader_enabled_ = false;
	gender_enabled_ = false;

	if(!utils::string_bool(side["random_faction"])) {
		leader_enabled_ = true;
		gender_enabled_ = true;

		leader_ids_ = utils::split(side["leader"]);

		std::vector<std::string>::const_iterator itor = find(leader_ids_.begin(), leader_ids_.end(), side["type"]);
		default_index = itor - leader_ids_.begin();
		if (itor == leader_ids_.end()) {
			leader_ids_.push_back(side["type"]);
		}

		leader_ids_.push_back("random");
		populate_leaders();
	}

	update_leader_gui(default_index);
}

void leader_list_manager::update_gender_gui(int index) {
	gui_.update_gender_list(gender_enabled_, genders_, index);
}
void leader_list_manager::update_gender_list(const std::string& leader)
{
	genders_.clear();
	gender_ids_.clear();
	if (leader == "random" || leader == "-" || leader == "?") {
		// Assume random/unknown leader/faction == unknown gender
		gender_ids_.push_back("null");
		genders_.push_back("-");
		gender_enabled_ = false;
	} else {
		const unit_type *utp = unit_types.find(leader);
		if (utp) {
			const unit_type &ut = *utp;
			const std::vector<unit_race::GENDER> genders = ut.genders();
			if (genders.size() > 1) {
				gender_ids_.push_back("random");
				genders_.push_back(IMAGE_PREFIX + random_enemy_picture + COLUMN_SEPARATOR + _("gender^Random"));
			}
			foreach(unit_race::GENDER gender, genders) {
				const unit_type& utg = ut.get_gender_unit_type(gender);

				// Make the internationalized titles for each gender, along with the WML ids
				if (gender == unit_race::FEMALE) {
					gender_ids_.push_back("female");
					genders_.push_back(IMAGE_PREFIX + utg.image() + get_RC_suffix(utg.flag_rgb()) +
							COLUMN_SEPARATOR + _("Female ♀"));
				} else {
					gender_ids_.push_back("male");
					genders_.push_back(IMAGE_PREFIX + utg.image() + get_RC_suffix(utg.flag_rgb()) +
							COLUMN_SEPARATOR + _("Male ♂"));
				}
			}
			gender_enabled_ = true;
		} else {
			gender_ids_.push_back("random");
			genders_.push_back(IMAGE_PREFIX + random_enemy_picture + COLUMN_SEPARATOR + _("Random"));
			gender_enabled_ = false;
		}
	}
	update_gender_gui();
}

void leader_list_manager::populate_leaders() {
	leaders_.clear();
	
	std::string gender = gender_ids_.empty() ? "" : gender_ids_[0];
	foreach(const std::string& leader, leader_ids_) {
		const unit_type *utp = unit_types.find(leader);
		std::string str = "?";
		if (utp) {
			const unit_type& ut = utp->get_gender_unit_type(gender);
			str = IMAGE_PREFIX + ut.image() + get_RC_suffix(ut.flag_rgb()) + COLUMN_SEPARATOR + ut.type_name();
		} else if(leader == "random") {
			str = IMAGE_PREFIX + random_enemy_picture + COLUMN_SEPARATOR + _("Random");
		}
		leaders_.push_back(str);
	}
}

void leader_list_manager::set_leader(const std::string& leader)
{
	std::vector<std::string>::iterator itor = find(leaders_.begin(), leaders_.end(), leader);
	if (itor != leaders_.end()) {
		gui_.set_leader(std::distance(leaders_.begin(), itor));
	}
}

void leader_list_manager::set_gender(const std::string& gender)
{
	std::vector<std::string>::iterator itor = find(genders_.begin(), genders_.end(), gender);
	if (itor != gender_ids_.end()) {
		gui_.set_gender(std::distance(gender_ids_.begin(), itor));
	}
}

std::string leader_list_manager::get_leader() const {
	if (leaders_.empty()) return "random";
	size_t idx = gui_.get_leader();
	return idx >= leader_ids_.size() ? _("?") : leader_ids_[idx];
}

std::string leader_list_manager::get_gender() const {
	size_t idx = gui_.get_gender();
	return idx >= genders_.size() ? "null" : gender_ids_[idx];
}


#ifdef LOW_MEM
std::string leader_list_manager::get_RC_suffix(const std::string& /*unit_colour*/) const {
	return "";
}
#else
std::string leader_list_manager::get_RC_suffix(const std::string& unit_colour) const {
	return "~RC("+unit_colour+">"+lexical_cast<std::string>(colour_+1) +")";
}
#endif

leader_list_gui::leader_list_gui(gui_list_type leader_combo , gui_list_type gender_combo) :
	leader_combo_(leader_combo),
	gender_combo_(gender_combo),
	leader_list_dirty_(false),
	gender_list_dirty_(false)
{
}

void leader_list_gui::set_leader_combo(gui_list_type combo)
{
	leader_combo_ = combo;
	leader_list_dirty_ = true;
}

void leader_list_gui::set_gender_combo(gui_list_type combo)
{
	gender_combo_ = combo;
	gender_list_dirty_ = true;
}

void leader_list_gui::update_leader_list(bool enabled, const std::vector<std::string>& values, int index)
{
	leader_combo_.enable(enabled);
	leader_combo_.set_items(values);
	set_leader(index);
	leader_list_dirty_ = false;
}

void leader_list_gui::update_gender_list(bool enabled, const std::vector<std::string>& values, int index) {
	gender_combo_.enable(enabled);
	gender_combo_.set_items(values);
	set_gender(index);
	gender_list_dirty_ = false;
}

void leader_list_gui::set_leader(int idx) {
	if (int nitems = leader_combo_.items_size()) {
		leader_combo_.set_selected(idx % nitems);
	}
}

void leader_list_gui::set_gender(int idx) {
	if (int nitems = gender_combo_.items_size()) {
		gender_combo_.set_selected(idx % nitems);
	}
}

int leader_list_gui::get_leader() const {
	return leader_combo_.selected();
}

int leader_list_gui::get_gender() const {
	return gender_combo_.selected();
}
