/*
   Copyright (C) 2012 by Chris Hopman <cjhopman@gmail.com>
   Part of the Battle for Wesnoth Project http://www.wesnoth.org/

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License version 2
   or at your option any later version.
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY.

   See the COPYING file for more details.
*/

#ifndef GUI_DIALOGS_SPEAK_HPP_INCLUDED
#define GUI_DIALOGS_SPEAK_HPP_INCLUDED

#include "gui/dialogs/dialog.hpp"

namespace gui2 {

class twindow;
class tspeak : public tdialog
{
public:
	tspeak();

	std::string get_message() { return message_; }
private:
	/** Inherited from tdialog. */
	twindow* build_window(CVideo& video);

	/** Inherited from tdialog. */
	void pre_show(CVideo& video, twindow& window);

	/** Inherited from tdialog. */
	void post_show(twindow& window);

	std::string message_;
};

} // namespace gui2

#endif

