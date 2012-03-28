/*
   Copyright (C) 2011 - 2012 Chris Hopman <cjhopman@gmail.com>
   Part of the Battle for Wesnoth Project http://www.wesnoth.org/

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License version 2
   or at your option any later version.
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY.

   See the COPYING file for more details.
*/

#ifndef GUI_WIDGETS_DROP_BUTTON_HPP_INCLUDED
#define GUI_WIDGETS_DROP_BUTTON_HPP_INCLUDED

#include "gui/widgets/button.hpp"
#include "gui/auxiliary/window_builder.hpp"
#include "gui/dialogs/drop_listbox.hpp"
#include <boost/function.hpp>

namespace gui2 {

class tdrop_button
	: public tbutton
{
	typedef tbutton super;
public:
	typedef boost::function<std::map<std::string, string_map>(std::string)> transform_type;
	typedef boost::function<void(tdrop_button*, int)> callback_selection_changed_type;

	tdrop_button();
	void create_drop_listbox(bool& handled, bool& halt);
	void set_transform(transform_type t) { transform_ = t ? t : default_transform; }
	void set_items(const std::vector<std::string>& vals);
	const std::vector<std::string>& get_items() { return vals_; }
	void set_list_builder(tbuilder_grid_ptr builder) { list_builder_ = builder; }
	void set_linked_groups(const std::vector<std::string> groups) { linked_groups_ = groups; }
	void set_label(const t_string& str);
	void set_callback_selection_changed(callback_selection_changed_type cb) { callback_selection_changed_ = cb; }

	int selected() { return selected_; }
	void set_selected(int v);

	static std::map<std::string, string_map> default_transform(std::string);
private:
	std::vector<std::string> vals_;
	transform_type transform_;
	tbuilder_grid_ptr list_builder_;
	std::vector<std::string> linked_groups_;
	callback_selection_changed_type callback_selection_changed_;
	int selected_;
};


} // namespace gui2

#endif

