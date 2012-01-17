/* $Id$ */
/*
   Copyright (C) 2010 - 2011 by Chris Hopman <cjhopman@gmail.com>
   Part of the Battle for Wesnoth Project http://www.wesnoth.org/

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY.

   See the COPYING file for more details.
*/

#ifndef GUI_DIALOGS_UNIT_LIST_HPP_INCLUDED
#define GUI_DIALOGS_UNIT_LIST_HPP_INCLUDED

#include "gui/dialogs/dialog.hpp"

#include "unit.hpp"
#include <vector>

namespace gui2 {

class tunit_list
	: public tdialog
{
public:
	tunit_list(const std::vector<unit>& units, int selected = 0);

	/***** ***** ***** setters / getters for members ***** ****** *****/

	int get_selected_unit() const { return selected_unit_; }

	void selection_changed(twindow* window, int item);

private:

	/** Inherited from tdialog. */
	twindow* build_window(CVideo& video);

	/** Inherited from tdialog. */
	void pre_show(CVideo& video, twindow& window);

	/** Inherited from tdialog. */
	void post_show(twindow& window);

	/** The index of the selected unit. */
	int selected_unit_;

	/** The listing team. */
	const std::vector<unit>& units_;
};

} // namespace gui2

#endif

