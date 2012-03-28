/*
   Copyright (C) 2011 Chris Hopman <cjhopman@gmail.com>
   Part of the Battle for Wesnoth Project http://www.wesnoth.org/

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License version 2
   or at your option any later version.
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY.

   See the COPYING file for more details.
*/

#ifndef GUI_AUXILIARY_WINDOW_BUILDER_DROP_BUTTON_HPP_INCLUDED
#define GUI_AUXILIARY_WINDOW_BUILDER_DROP_BUTTON_HPP_INCLUDED

#include "gui/auxiliary/window_builder/button.hpp"

#include <vector>

namespace gui2 {

namespace implementation {

struct tbuilder_drop_button
	: public tbuilder_button
{
private:
	tbuilder_drop_button();
public:
	tbuilder_drop_button(const config& cfg);

	twidget* build () const;
private:
	tbuilder_grid_ptr list_builder_;
	std::vector<std::string> linked_groups_;
};

} // namespace implementation

} // namespace gui2

#endif


