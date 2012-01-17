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

#include "gui/dialogs/unit_list.hpp"
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
#include "marked-up_text.hpp"

#include <boost/bind.hpp>

#include <sstream>

namespace gui2 {

tunit_list::tunit_list(
		const std::vector<unit>& units,
		int selected
	)
	: selected_unit_(selected)
	, units_(units)
{
}

twindow* tunit_list::build_window(CVideo& video)
{
	return build(video, get_id(UNIT_LIST));
}

namespace {
void selection_changed_callback(twidget* caller) {
	tunit_list* dialog = dynamic_cast<tunit_list*>(caller->dialog());
	assert(dialog);
	twindow* window = dynamic_cast<twindow*>(caller->get_window());
	assert(window);
	dialog->selection_changed(window, dynamic_cast<tlistbox*>(caller)->get_selected_row());
}
}

void tunit_list::selection_changed(twindow* window, int item) {
	selected_unit_ = item;

	find_widget<tmulti_page>(window, "unit_page", false).select_page(item);
}

void tunit_list::pre_show(CVideo& /*video*/, twindow& window)
{
	tlistbox& unit_list =
			find_widget<tlistbox>(&window, "units_list", false);

	unit_list.set_callback_value_change(selection_changed_callback);

	tmulti_page& multi_page = find_widget<tmulti_page>(
			&window, "unit_page", false);

	foreach(const unit& u, units_) {
		std::map<std::string, string_map> data = get_unit_info(u);
		markup_unit_moves(data, u);

		multi_page.add_page(data);

		if (u.can_recruit()) {
			data["name"]["use_markup"] = "true";
			data["type"]["use_markup"] = "true";

			SDL_Color highlight_color =  {205, 173, 0, 0};
			std::stringstream ss;
			ss << "<span foreground=\"#" << font::color2hexa(highlight_color) << "\">" << data["name"]["label"] << "</span>";
			data["name"]["label"] = ss.str();

			ss.str("");
			ss << "<span foreground=\"#" << font::color2hexa(highlight_color) << "\">" << data["type"]["label"] << "</span>";
			data["type"]["label"] = ss.str();
		}
		unit_list.add_row(data);
	}

	if (selected_unit_ >= 0) {
		unit_list.select_row(selected_unit_);
		selection_changed(&window, selected_unit_);
	}
}

void tunit_list::post_show(twindow& window)
{
	if(get_retval() == twindow::OK) {
		selected_unit_ = find_widget<tlistbox>(&window, "units_list", false)
			.get_selected_row();
	}
}

} // namespace gui2

