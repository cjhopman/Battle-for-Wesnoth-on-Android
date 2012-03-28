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

#ifndef GUI_DIALOGS_DROP_LISTBOX_HPP_INCLUDED
#define GUI_DIALOGS_DROP_LISTBOX_HPP_INCLUDED

#include "gui/auxiliary/window_builder.hpp"
#include "gui/dialogs/dialog.hpp"
#include "gui/widgets/window.hpp"

#include <boost/function.hpp>

#include <map>
#include <string>
#include <vector>

namespace gui2 {

class twindow;
class tlistbox;

class tdrop_listbox : public tdialog
{
public:
	typedef boost::function<std::map<std::string, string_map>(std::string)> transform_type;
	tdrop_listbox(
			tpoint pos,
			std::vector<std::string> vals,
			std::vector<std::string> groups,
			tbuilder_grid_ptr builder,
			transform_type trans,
			int sel);
	twindow* build_window(CVideo& video);
	void pre_show(CVideo& /*video*/, twindow& /*window*/);
	void post_show(twindow& /*window*/);
private:
	void close_and_select(twindow* window, tlistbox* listbox);
	tpoint pos_;
	std::vector<std::string> vals_;
	std::vector<std::string> linked_groups_;
	tbuilder_grid_ptr list_builder_;
	transform_type transform_;
	int selected_;
	unsigned close_timer_;
};

}

#endif
