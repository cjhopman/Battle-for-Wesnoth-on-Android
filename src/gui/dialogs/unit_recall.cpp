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

#include "gui/dialogs/unit_recall.hpp"
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

#include <boost/bind.hpp>

#include <sstream>

namespace gui2 {

tunit_recall::tunit_recall(
		const team& t)
	: selected_unit_(0)
	, team_(t)
{
}

twindow* tunit_recall::build_window(CVideo& video)
{
	return build(video, get_id(UNIT_RECALL));
}

namespace {
void selection_changed_callback(twidget* caller) {
	tunit_recall* dialog = dynamic_cast<tunit_recall*>(caller->dialog());
	assert(dialog);
	twindow* window = dynamic_cast<twindow*>(caller->get_window());
	assert(window);
	dialog->selection_changed(window, dynamic_cast<tlistbox*>(caller)->get_selected_row());
}
}

void tunit_recall::selection_changed(twindow* window, int item) {
	selected_unit_ = item;

	find_widget<tmulti_page>(window, "unit_page", false).select_page(item);
}

void tunit_recall::pre_show(CVideo& /*video*/, twindow& window)
{
	tlistbox& recall_list =
			find_widget<tlistbox>(&window, "unit_list", false);

	recall_list.set_callback_value_change(selection_changed_callback);

	tmulti_page& multi_page = find_widget<tmulti_page>(
			&window, "unit_page", false);

	foreach(const unit& u, team_.recall_list()) {
		std::map<std::string, string_map> data = get_unit_info(u);

		recall_list.add_row(data);
		multi_page.add_page(data);

		tgrid& unit = multi_page.page_grid(multi_page.get_page_count() - 1);
	}

	recall_list.select_row(0);
	selection_changed(&window, 0);
}

void tunit_recall::post_show(twindow& window)
{
	if(get_retval() == twindow::OK) {
		selected_unit_ = find_widget<tlistbox>(&window, "unit_list", false)
			.get_selected_row();
	}
}

} // namespace gui2

