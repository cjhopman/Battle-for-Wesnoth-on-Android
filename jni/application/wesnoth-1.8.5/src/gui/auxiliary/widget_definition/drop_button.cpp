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

#define GETTEXT_DOMAIN "wesnoth-lib"

#include "gui/auxiliary/widget_definition/drop_button.hpp"

#include "gui/auxiliary/log.hpp"

namespace gui2 {

tdrop_button_definition::tdrop_button_definition(const config& cfg) :
	tbutton_definition(cfg)
{
	DBG_GUI_P << "Parsing drop_button " << id << '\n';

	load_resolutions<tresolution>(cfg);
}

} // namespace gui2

