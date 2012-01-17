/* $Id: title_screen.hpp 48450 2011-02-08 20:55:18Z mordante $ */
/*
   Copyright (C) 2008 - 2011 by Mark de Wever <koraq@xs4all.nl>
   Part of the Battle for Wesnoth Project http://www.wesnoth.org/

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License version 2
   or at your option any later version.
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY.

   See the COPYING file for more details.
*/

#ifndef GUI_DIALOGS_TITLE_SCREEN_HPP_INCLUDED
#define GUI_DIALOGS_TITLE_SCREEN_HPP_INCLUDED

#include "gui/dialogs/dialog.hpp"

#include "config.hpp"
#include "loadscreen.hpp"


namespace gui2 {

class ttitle_screen : public tdialog
{
public:
	ttitle_screen(loadscreen::global_loadscreen_manager& ls);

	CVideo* video() { return video_; }

private:
	/** Used in show in order to show child windows. */
	CVideo* video_;

	/** Inherited from tdialog. */
	twindow* build_window(CVideo& video);

	/** Inherited from tdialog. */
	void pre_show(CVideo& video, twindow& window);

	/** Inherited from tdialog. */
	void post_show(twindow& window);

	/** Hold the tips of the day. */
	config tips_;

	/**
	 * Updates the tip of day widget.
	 *
	 * @param window              The window being shown.
	 * @param previous            Show the previous tip, else shows the next
	 *                            one.
	 */
	void update_tip(twindow& window, const bool previous);

	/** Helper to forward the call to update_tip. */
	static void next_tip(twidget* caller);

	/** Helper to forward the call to update_tip. */
	static void previous_tip(twidget* caller);

	loadscreen::global_loadscreen_manager& loadscreen_manager_;
};

} // namespace gui2

#endif
