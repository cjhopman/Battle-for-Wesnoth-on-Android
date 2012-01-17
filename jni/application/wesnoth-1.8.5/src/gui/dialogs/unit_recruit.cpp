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

#define GETTEXT_DOMAIN "wesnoth-lib"

#include "gui/dialogs/unit_recruit.hpp"
#include "gui/dialogs/unit_panel.hpp"

#include "gui/widgets/image.hpp"
#ifdef GUI2_EXPERIMENTAL_LISTBOX
#include "gui/widgets/list.hpp"
#else
#include "gui/widgets/listbox.hpp"
#endif
#include "gui/widgets/settings.hpp"
#include "gui/widgets/window.hpp"
#include "gui/widgets/grid.hpp"
#include "gui/widgets/button.hpp"

#include "gui/widgets/multi_page.hpp"

#include "gui/auxiliary/log.hpp"

#include "team.hpp"
#include "unit_types.hpp"

#include <boost/bind.hpp>

#include <sstream>

namespace gui2 {

tunit_recruit::tunit_recruit(
		const team& t,
		const std::vector<const unit_type*>& ty)
	: selected_recruit_(0)
	, team_(t)
	, types_(ty)
{
}

twindow* tunit_recruit::build_window(CVideo& video)
{
	return build(video, get_id(UNIT_RECRUIT));
}

namespace {
void selection_changed_callback(twidget* caller) {
	tunit_recruit* dialog = dynamic_cast<tunit_recruit*>(caller->dialog());
	assert(dialog);
	twindow* window = dynamic_cast<twindow*>(caller->get_window());
	assert(window);
	dialog->selection_changed(window, dynamic_cast<tlistbox*>(caller)->get_selected_row());
}
}

void tunit_recruit::selection_changed(twindow* window, int item) {
	selected_recruit_ = item;

	find_widget<tmulti_page>(window, "unit_page", false).select_page(item);
}

void tunit_recruit::pre_show(CVideo& /*video*/, twindow& window)
{
	tlistbox& recruit_list =
			find_widget<tlistbox>(&window, "unit_list", false);

	recruit_list.set_callback_value_change(selection_changed_callback);

	tmulti_page& multi_page = find_widget<tmulti_page>(
			&window, "unit_page", false);

	foreach(const unit_type* type, types_) {
		std::map<std::string, string_map> data = get_unit_info(type, &team_);

		recruit_list.add_row(data);
		multi_page.add_page(data);
	}

	recruit_list.select_row(0);
	selection_changed(&window, 0);
}

void tunit_recruit::post_show(twindow& window)
{
	if(get_retval() == twindow::OK) {
		selected_recruit_ = find_widget<tlistbox>(&window, "unit_list", false)
			.get_selected_row();
	}
}

} // namespace gui2

