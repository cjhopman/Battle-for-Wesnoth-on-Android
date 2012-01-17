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

#ifndef GUI_DIALOGS_UNIT_ATTACK_HPP_INCLUDED
#define GUI_DIALOGS_UNIT_ATTACK_HPP_INCLUDED

#include "gui/dialogs/dialog.hpp"

class config;

namespace gui2 {

class tmulti_page;
class twindow;

class tpreferences 
	: public tdialog
{
public:
	explicit tpreferences(
		const config* cfg
			);

	/***** ***** ***** setters / getters for members ***** ****** *****/

private:

	/** Inherited from tdialog. */
	twindow* build_window(CVideo& video);

	/** Inherited from tdialog. */
	void pre_show(CVideo& video, twindow& window);

	/** Inherited from tdialog. */
	void post_show(twindow& window);

	void page_changed(twidget* widget);

	tmulti_page* options_panel_;
	twindow* window_;
	const config* game_cfg_;
};

} // namespace gui2

#endif

