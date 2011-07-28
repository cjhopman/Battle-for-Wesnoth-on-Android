/* $Id: touch_game_menu.hpp 48450 2011-02-08 20:55:18Z hopman $ */
/*
   Copyright (C) 2008 - 2011 by Jörg Hinrichs <joerg.hinrichs@alice-dsl.de>
   Part of the Battle for Wesnoth Project http://www.wesnoth.org/

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License version 2
   or at your option any later version.
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY.

   See the COPYING file for more details.
*/

#ifndef GUI_DIALOGS_TOUCH_GAME_MENU_HPP_INCLUDED
#define GUI_DIALOGS_TOUCH_GAME_MENU_HPP_INCLUDED

#include "gui/dialogs/dialog.hpp"
#include "tstring.hpp"
#include <boost/function.hpp>


namespace gui2 {

class ttouch_game_menu : public tdialog
{
public:
	enum action {
		NOTHING = 0,
		LOAD_GAME,
		SAVE_GAME,
		QUIT_GAME,
		PREFERENCES,
		OBJECTIVES,
		UNDO,
		REDO,
		RECALL,
		RECRUIT,
		ENEMY_MOVES,
		UNIT_LIST,
		STATISTICS,
	};
	ttouch_game_menu();

	void change_page(int page, twindow* window);
protected:
	/** Inherited from tdialog. */
	void pre_show(CVideo& video, twindow& window);

	/** Inherited from tdialog. */
	void post_show(twindow& window);

private:
	CVideo* video_;
	/** Inherited from tdialog. */
	twindow* build_window(CVideo& video);
};

}

#endif

