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

#ifndef GUI_DIALOGS_MENU_HPP_INCLUDED
#define GUI_DIALOGS_MENU_HPP_INCLUDED

#include "gui/dialogs/dialog.hpp"

#include "mapgen_dialog.hpp"

#include <boost/function.hpp>
#include <map>
#include <vector>

namespace gui2 {

class twindow;
class tlistbox;
class tmenu : public tdialog
{
public:
	typedef std::map<std::string, string_map> ssmap;
	typedef boost::function<ssmap(std::string)> transform_type;
	tmenu(std::string t, const std::vector<std::string>& v, transform_type tr = default_transform);

	int get_result() { return res_; }
private:
	/** Inherited from tdialog. */
	twindow* build_window(CVideo& video);

	/** Inherited from tdialog. */
	void pre_show(CVideo& video, twindow& window);

	/** Inherited from tdialog. */
	void post_show(twindow& window);

	void select_cb(twindow* window, tlistbox* l);
	void accept_cb(twindow* window);

	std::string title_;
	std::vector<std::string> vals_;
	transform_type transform_;
	int res_;

	static ssmap default_transform(std::string);
};

} // namespace gui2

#endif

