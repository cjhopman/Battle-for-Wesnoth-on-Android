/* $Id: leader_list.hpp 48450 2011-02-08 20:55:18Z mordante $ */
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

/** @file leader_list.hpp */

#ifndef LEADER_LIST_HPP_INCLUDED
#define LEADER_LIST_HPP_INCLUDED

class config;

#include "gui_adaptor.hpp"
#include "unit_types.hpp"
#include <string>
#include <vector>

class leader_list_gui {
public:
	typedef gui_adaptor::list gui_list_type;
	leader_list_gui() { }
	leader_list_gui(gui_list_type leader_combo, gui_list_type gender_combo);

	void set_leader_combo(gui_list_type combo);
	void set_gender_combo(gui_list_type combo);

	void update_leader_list(bool enabled, const std::vector<std::string>& values, int index = -1);
	void update_gender_list(bool enabled, const std::vector<std::string>& values, int index = -1);

	int get_leader() const;
	int get_gender() const;

	void set_leader(int);
	void set_gender(int);

	bool leader_list_dirty() { return leader_list_dirty_; }
	bool gender_list_dirty() { return gender_list_dirty_; }
private:
	gui_list_type leader_combo_;
	gui_list_type gender_combo_;
	bool leader_list_dirty_;
	bool gender_list_dirty_;
};

class leader_list_manager
{
public:
	typedef leader_list_gui::gui_list_type gui_list_type;
	struct gui_accessor {
		gui_accessor(leader_list_manager* p) : parent_(p) { }
		leader_list_gui* get() {
			return &parent_->gui_;
		}
		leader_list_gui* operator->() {
			return get();
		}
		~gui_accessor() {
			if (get()->leader_list_dirty()) {
				parent_->update_leader_gui();
			}
			if (get()->gender_list_dirty()) {
				parent_->update_gender_gui();
			}
		}
		private:
			leader_list_manager* parent_;
	};

	static const std::string random_enemy_picture;
	leader_list_manager(const std::vector<const config *> &side_list, leader_list_gui gui = leader_list_gui());

	void set_colour(int colour) {colour_ = colour;};
	std::string get_RC_suffix(const std::string& unit_colour) const;

	void update_leader_list(int side);
	void update_gender_list(const std::string& leader);

	std::string get_leader() const;
	std::string get_gender() const;

	void set_leader(const std::string& leader);
	void set_gender(const std::string& gender);

	gui_accessor gui() { return gui_accessor(this); }
private:
	void populate_leaders();

	void update_leader_gui(int index = 0);
	void update_gender_gui(int index = 0);

	std::vector<std::string> leaders_;
	std::vector<std::string> genders_;
	std::vector<std::string> leader_ids_;
	std::vector<std::string> gender_ids_;
	std::vector<const config *> side_list_;

	bool leader_enabled_;
	bool gender_enabled_;

	int colour_;

	leader_list_gui gui_;
};


#endif

