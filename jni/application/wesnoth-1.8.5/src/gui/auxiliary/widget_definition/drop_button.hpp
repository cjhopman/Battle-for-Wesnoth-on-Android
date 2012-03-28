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

#ifndef GUI_AUXILIARY_WIDGET_DEFINITION_DROP_BUTTON_HPP_INCLUDED
#define GUI_AUXILIARY_WIDGET_DEFINITION_DROP_BUTTON_HPP_INCLUDED

#include "gui/auxiliary/widget_definition.hpp"
#include "gui/auxiliary/widget_definition/button.hpp"

namespace gui2 {

struct tdrop_button_definition
	: public tbutton_definition
{
	explicit tdrop_button_definition(const config& cfg);
	typedef tbutton_definition::tresolution tresolution;
};

} // namespace gui2

#endif

