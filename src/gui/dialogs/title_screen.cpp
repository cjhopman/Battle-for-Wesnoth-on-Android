/* $Id: title_screen.cpp 48450 2011-02-08 20:55:18Z mordante $ */
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

#define GETTEXT_DOMAIN "wesnoth-lib"

#include "gui/dialogs/title_screen.hpp"

#include "game_config.hpp"
#include "gettext.hpp"
#include "log.hpp"
#include "gui/dialogs/addon_connect.hpp"
#include "gui/dialogs/language_selection.hpp"
#include "gui/widgets/button.hpp"
#include "gui/widgets/label.hpp"
#include "gui/widgets/multi_page.hpp"
#include "gui/widgets/settings.hpp"
#include "gui/widgets/window.hpp"
#include "titlescreen.hpp"

static lg::log_domain log_config("config");
#define ERR_CF LOG_STREAM(err, log_config)

namespace gui2 {

namespace {

template<class D>
void show_dialog(twidget* caller)
{
	ttitle_screen *dialog = dynamic_cast<ttitle_screen*>(caller->dialog());
	assert(dialog);

	D dlg;
	dlg.show(*(dialog->video()));
}

} // namespace

/*WIKI
 * @page = GUIWindowDefinitionWML
 * @order = 2_title_screen
 *
 * == Title screen ==
 *
 * This shows the title screen. This dialog is still under construction and
 * is only shown when --new-widgets are used.
 *
 * @start_table = grid
 *     (addons) (button) ()       The button to get the addons.
 *     (language) (button) ()     The button to change the language.
 *
 *     (tip) (label) ()           The tip of the day.
 *     (source) (label) ()        The source for the tip of the day.
 *     (next_tip) (button) ()     The button show the next tip of day.
 *     (previous_tip) (button) () The button show the previous tip of day.
 * @end_table
 */

ttitle_screen::ttitle_screen(loadscreen::global_loadscreen_manager& ls)
	: video_(NULL)
	, tips_()
	, loadscreen_manager_(ls)
{
	read_tips_of_day(tips_);
}

twindow* ttitle_screen::build_window(CVideo& video)
{
	return build(video, get_id(TITLE_SCREEN));
}

namespace {
	void set_tips_of_day(twindow& window, const config& tips_cfg) {
		tmulti_page& box = find_widget<tmulti_page>(&window, "tip_box", false);
		config::const_child_itors itors = tips_cfg.child_range("tip");

		foreach(const config& cfg, itors) {
			std::map<std::string, string_map> data;
			data["tip"]["label"] = cfg["text"];
			data["source"]["label"] = cfg["source"];
			box.add_page(data);
		}
		box.select_page(0);
	}
}

void ttitle_screen::pre_show(CVideo& video, twindow& window)
{
	assert(!video_);
	video_ = &video;

	set_restore(false);
	window.set_click_dismiss(false);
	window.set_escape_disabled(true);

	window.canvas()[0].set_variable("revision_number",
		variant(_("Version") + std::string(" ") + game_config::revision));

	/**** Set the buttons ****/
	tbutton* addons = find_widget<tbutton>(&window, "addons", false, false);
	if (addons) {
		addons->set_callback_mouse_left_click(show_dialog<gui2::taddon_connect>);
	}

	// Note changing the language doesn't upate the title screen...
	tbutton* lang = find_widget<tbutton>(&window, "language", false, false);
	if (lang) {
		lang->set_callback_mouse_left_click(
				show_dialog<gui2::tlanguage_selection>);
	}

	/**** Set the tip of the day ****/
	set_tips_of_day(window, tips_);

	find_widget<tbutton>(&window, "next_tip", false).
			set_callback_mouse_left_click(next_tip);

	find_widget<tbutton>(&window, "previous_tip", false).
			set_callback_mouse_left_click(previous_tip);

	/***** Select a random game_title *****/
	std::vector<std::string> game_title_list =
		utils::split(game_config::game_title
				, ','
				, utils::STRIP_SPACES | utils::REMOVE_EMPTY);

	if(game_title_list.empty()) {
		ERR_CF << "No title image defined\n";
	} else {
		window.canvas()[0].set_variable("background_image",
			variant(game_title_list[rand()%game_title_list.size()]));
	}

	//find_widget<tbutton>(&window, "addons", false).set_visible(twidget::INVISIBLE);
	find_widget<tbutton>(&window, "help", false).set_visible(twidget::INVISIBLE);
	//find_widget<tbutton>(&window, "language", false).set_visible(twidget::INVISIBLE);
	find_widget<tbutton>(&window, "previous_tip", false).set_visible(twidget::INVISIBLE);
	//find_widget<tbutton>(&window, "editor", false).set_visible(twidget::INVISIBLE);
	//find_widget<tbutton>(&window, "preferences", false).set_visible(twidget::INVISIBLE);
	
	loadscreen_manager_.reset();
}

void ttitle_screen::post_show(twindow& /*window*/)
{
	video_ = NULL;
}

void ttitle_screen::update_tip(twindow& window, const bool previous)
{
	tmulti_page& tips = find_widget<tmulti_page>(&window, "tip_box", false);
	int next = tips.get_selected_page() + (previous ? -1 : 1);
	tips.select_page((next + tips.get_page_count()) % tips.get_page_count());
}

void ttitle_screen::next_tip(twidget* caller)
{
	ttitle_screen *dialog = dynamic_cast<ttitle_screen*>(caller->dialog());
	assert(dialog);

	twindow *window = caller->get_window();
	assert(window);

	dialog->update_tip(*window, true);
}

void ttitle_screen::previous_tip(twidget* caller)
{
	ttitle_screen *dialog = dynamic_cast<ttitle_screen*>(caller->dialog());
	assert(dialog);

	twindow *window = caller->get_window();
	assert(window);

	dialog->update_tip(*window, false);
}

} // namespace gui2
