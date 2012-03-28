/*
   Copyright (C) 2011 by Chris Hopman <cjhopman@gmail.com>
   Part of the Battle for Wesnoth Project http://www.wesnoth.org/

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License version 2
   or at your option any later version.
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY.

   See the COPYING file for more details.
*/

#define GETTEXT_DOMAIN "wesnoth-lib"

#include "gui/auxiliary/window_builder/drop_button.hpp"

#include "gui/auxiliary/window_builder/helper.hpp"
#include "gui/widgets/drop_button.hpp"

#include "config.hpp"
#include "gettext.hpp"

#include <deque>
#include <algorithm>

namespace gui2 {

namespace implementation {

tbuilder_drop_button::tbuilder_drop_button(const config& cfg)
	: tbuilder_button(cfg)
{
	const config &panel_def = cfg.child("panel_definition");
	VALIDATE(panel_def, _("No list defined."));
	config list_def;
	config& column = list_def.add_child("row").add_child("column");
	column["horizontal_grow"] = "true";
	column["vertical_grow"] = "true";
	config& row = column.add_child("toggle_panel").add_child("grid").add_child("row", panel_def);
	list_builder_ = new tbuilder_grid(list_def);

	std::deque<config::any_child> widgets(row.ordered_begin(), row.ordered_end());
	while (!widgets.empty()) {
		config::any_child w = widgets.front();
		widgets.pop_front();
		std::copy(w.cfg.ordered_begin(), w.cfg.ordered_end(), std::back_inserter(widgets));
		if (w.cfg.has_attribute("linked_group")) {
			linked_groups_.push_back(w.cfg["linked_group"]);
		}
	}
	assert(list_builder_);
}

twidget* tbuilder_drop_button::build() const
{
	tdrop_button* widget = new tdrop_button();
	widget->set_list_builder(list_builder_);
	widget->set_linked_groups(linked_groups_);

	init_control(widget);

	return widget;
}

} // namespace implementation

} // namespace gui2
