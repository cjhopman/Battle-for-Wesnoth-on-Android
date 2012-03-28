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

#ifndef GUI_DIALOGS_MAPGEN_SETTINGS_HPP_INCLUDED
#define GUI_DIALOGS_MAPGEN_SETTINGS_HPP_INCLUDED

#include "gui/dialogs/dialog.hpp"

#include "mapgen_dialog.hpp"

class config;

namespace gui2 {
class tmapgen_settings : public tdialog
{
	typedef default_map_generator::slider_settings slider_settings;
public:
	struct settings {
		slider_settings width, height, island_size, iterations, hill_size, nvillages, castle_size, nplayers;
		bool link_castles;
	};

	tmapgen_settings(settings& s);
private:
	/** Inherited from tdialog. */
	twindow* build_window(CVideo& video);

	/** Inherited from tdialog. */
	void pre_show(CVideo& video, twindow& window);

	/** Inherited from tdialog. */
	void post_show(twindow& window);

	void setup_slider(twindow* window, std::string id, slider_settings& s);

	settings& s_;
};

} // namespace gui2

#endif

