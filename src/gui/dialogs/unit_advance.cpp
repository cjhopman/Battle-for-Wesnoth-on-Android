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

#include "gui/dialogs/unit_advance.hpp"
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

#include "wml_separators.hpp"

#include "team.hpp"

#include <boost/bind.hpp>

#include <sstream>

namespace gui2 {

tunit_advance::tunit_advance(
		const std::vector<unit>& u,
		const std::vector<std::string>& o)
	: selected_(0)
	, units_(u)
	, options_(o)
{
}

twindow* tunit_advance::build_window(CVideo& video)
{
	return build(video, get_id(UNIT_ADVANCE));
}

namespace {
void selection_changed_callback(twidget* caller) {
	tunit_advance* dialog = dynamic_cast<tunit_advance*>(caller->dialog());
	assert(dialog);
	twindow* window = dynamic_cast<twindow*>(caller->get_window());
	assert(window);
	dialog->selection_changed(window, dynamic_cast<tlistbox*>(caller)->get_selected_row());
}
}

void tunit_advance::selection_changed(twindow* window, int item) {
	selected_ = item;

	find_widget<tmulti_page>(window, "unit_page", false).select_page(item);
}

void tunit_advance::pre_show(CVideo& /*video*/, twindow& window)
{
	tlistbox& advance_list =
			find_widget<tlistbox>(&window, "unit_list", false);

	advance_list.set_callback_value_change(selection_changed_callback);

	tmulti_page& multi_page = find_widget<tmulti_page>(
			&window, "unit_page", false);

	foreach(const unit& u, units_) {
		std::map<std::string, string_map> data = get_unit_info(u);
		multi_page.add_page(data);
	}

	foreach(const std::string& str, options_) {
		std::map<std::string, string_map> data;
		size_t sep = str.find_first_of(COLUMN_SEPARATOR);
		assert(sep != std::string::npos);
		data["image"]["label"] = str.substr(1, sep - 1);
		data["description"]["label"] = str.substr(sep + 1, str.size() - sep - 1);

		advance_list.add_row(data);
	}

	advance_list.select_row(0);
	selection_changed(&window, 0);

	window.set_escape_disabled(true);
}

void tunit_advance::post_show(twindow& window)
{
	if(get_retval() == twindow::OK) {
		selected_ = find_widget<tlistbox>(&window, "unit_list", false)
			.get_selected_row();
	}
}

} // namespace gui2

