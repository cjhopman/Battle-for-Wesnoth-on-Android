/* $Id$ */
/*
   Copyright (C) 2010 - 2011 by Mark de Wever <koraq@xs4all.nl>
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

#include "config.hpp"
#include "foreach.hpp"

#include "gui/dialogs/preferences.hpp"
#include "gui/auxiliary/log.hpp"

#ifdef GUI2_EXPERIMENTAL_LISTBOX
#include "gui/widgets/list.hpp"
#else
#include "gui/widgets/listbox.hpp"
#endif
#include "gui/widgets/settings.hpp"
#include "gui/widgets/window.hpp"
#include "gui/widgets/spacer.hpp"
#include "gui/widgets/multi_page.hpp"
#include "gui/widgets/selectable.hpp"
#include "gui/widgets/slider.hpp"

#include "preferences_display.hpp"

#include <SDL_mouse.h>

#include <boost/bind.hpp>

namespace gui2 {

tpreferences::tpreferences(const config* cfg)
	: options_panel_(0)
	, window_(0)
	, game_cfg_(cfg)
{
}

twindow* tpreferences::build_window(CVideo& video)
{
	SDL_WarpMouse(0, 0);
	return build(video, get_id(PREFERENCES));
}

namespace {
	void preferences_callback(void(*func)(bool), twidget* widget, bool flip = false) {
		tselectable_* opt = dynamic_cast<tselectable_*>(widget);
		assert(opt);
		func(flip ? !opt->get_value() : opt->get_value());
	}

	void preferences_callback_slider(void(*func)(int), twidget* widget) {
		tslider* opt = dynamic_cast<tslider*>(widget);
		assert(opt);
		func(opt->get_value());
	}


	void advanced_preferences_callback(const std::string& field, twidget* widget) {
		tselectable_* opt = dynamic_cast<tselectable_*>(widget);
		assert(opt);
		preferences::set(field, opt->get_value() ? "yes" : "no"); 
	}

	void preferences_callback_sound(bool(*func)(bool), twidget* widget, tslider* slider, bool flip = false) {
		tselectable_* opt = dynamic_cast<tselectable_*>(widget);
		assert(opt);
		bool new_val = flip ? !opt->get_value() : opt->get_value();
		if (!func(new_val)) {
			opt->set_value(flip ? true : false);
		}
		if (slider) slider->set_active(opt->get_value());
	}
}


void tpreferences::pre_show(CVideo& /*video*/, twindow& window)
{
	window_ = &window;

	find_widget<tselectable_>(&window, "general_select", false).set_callback_state_change(boost::bind(&tpreferences::page_changed, this, _1));
	find_widget<tselectable_>(&window, "display_select", false).set_callback_state_change(boost::bind(&tpreferences::page_changed, this, _1));
	find_widget<tselectable_>(&window, "sound_select", false).set_callback_state_change(boost::bind(&tpreferences::page_changed, this, _1));
	find_widget<tselectable_>(&window, "advanced_select", false).set_callback_state_change(boost::bind(&tpreferences::page_changed, this, _1));

	options_panel_ = find_widget<tmulti_page>(&window, "options_panel", false, true);
	options_panel_->clear();
	tgrid* grid = find_widget<tgrid>(&window, "main_grid", false, true);


	{
		tspacer* sp = new tspacer();
		sp->set_id("sp_temp");
		twidget* gen_tab = grid->swap_child("general_tab", sp, true);
		assert(gen_tab);
		options_panel_->add_page(string_map());
		options_panel_->page_grid(0).set_child(gen_tab, 0, 0,
				tgrid::VERTICAL_ALIGN_CENTER | tgrid::HORIZONTAL_ALIGN_LEFT | tgrid::BORDER_ALL, 5);
	}

	{
		tspacer* sp = new tspacer();
		sp->set_id("sp_temp");
		twidget* dsp_tab = grid->swap_child("display_tab", sp, true);
		assert(dsp_tab);
		options_panel_->add_page(string_map());
		options_panel_->page_grid(1).set_child(dsp_tab, 0, 0,
				tgrid::VERTICAL_ALIGN_CENTER | tgrid::HORIZONTAL_ALIGN_LEFT | tgrid::BORDER_ALL, 5);
	}

	{
		tspacer* sp = new tspacer();
		sp->set_id("sp_temp");
		twidget* snd_tab = grid->swap_child("sound_tab", sp, true);
		assert(snd_tab);
		options_panel_->add_page(string_map());
		options_panel_->page_grid(2).set_child(snd_tab, 0, 0,
				tgrid::VERTICAL_ALIGN_CENTER | tgrid::HORIZONTAL_ALIGN_LEFT | tgrid::BORDER_ALL, 5);
	}

	{
		tspacer* sp = new tspacer();
		sp->set_id("sp_temp");
		twidget* adv_tab = grid->swap_child("advanced_tab", sp, true);
		assert(adv_tab);
		options_panel_->add_page(string_map());
		options_panel_->page_grid(3).set_child(adv_tab, 0, 0,
				tgrid::VERTICAL_ALIGN_CENTER | tgrid::HORIZONTAL_ALIGN_LEFT | tgrid::BORDER_ALL, 5);
	}

	grid->remove_child("sp_temp", true);
	grid->set_rows_cols(1, 2);

	tselectable_* option;
	tslider* slider;
	// general options
	tgrid* general_options = &options_panel_->page_grid(0);
	if ((option = find_widget<tselectable_>(general_options, "accelerated", false, false))) {
		option->set_value(preferences::turbo());
		option->set_callback_state_change(boost::bind<void>(preferences_callback, preferences::set_turbo, _1, false));
	}

	if ((option = find_widget<tselectable_>(general_options, "show_ai", false, false))) {
		option->set_value(preferences::show_ai_moves());
		option->set_callback_state_change(boost::bind<void>(preferences_callback, preferences::set_show_ai_moves, _1, false));
	}

	if ((option = find_widget<tselectable_>(general_options, "turn_dialog", false, false))) {
		option->set_value(preferences::turn_dialog());
		option->set_callback_state_change(boost::bind<void>(preferences_callback, preferences::set_turn_dialog, _1, false));
	}

	if ((option = find_widget<tselectable_>(general_options, "delete_on_victory", false, false))) {
		option->set_value(preferences::delete_saves());
		option->set_callback_state_change(boost::bind<void>(preferences_callback, preferences::set_delete_saves, _1, false));
	}

	if ((option = find_widget<tselectable_>(general_options, "replay_on_victory", false, false))) {
		option->set_value(preferences::save_replays());
		option->set_callback_state_change(boost::bind<void>(preferences_callback, preferences::set_save_replays, _1, false));
	}

	// display options
	tgrid* display_options = &options_panel_->page_grid(1);
	if ((option = find_widget<tselectable_>(display_options, "scroll_tracking", false, false))) {
		option->set_value(preferences::scroll_to_action());
		option->set_callback_state_change(boost::bind<void>(preferences_callback, preferences::set_scroll_to_action, _1, false));
	}

	if ((option = find_widget<tselectable_>(display_options, "reverse_sun", false, false))) {
		option->set_value(preferences::flip_time());
		option->set_callback_state_change(boost::bind<void>(preferences_callback, preferences::set_flip_time, _1, false));
	}

	if ((option = find_widget<tselectable_>(display_options, "floating_labels", false, false))) {
		option->set_value(preferences::show_floating_labels());
		option->set_callback_state_change(boost::bind<void>(preferences_callback, preferences::set_show_floating_labels, _1, false));
	}

	if ((option = find_widget<tselectable_>(display_options, "halo_effects", false, false))) {
		option->set_value(preferences::show_haloes());
		option->set_callback_state_change(boost::bind<void>(preferences_callback, preferences::set_show_haloes, _1, false));
	}

	if ((option = find_widget<tselectable_>(display_options, "idle_animations", false, false))) {
		option->set_value(preferences::idle_anim());
		option->set_callback_state_change(boost::bind<void>(preferences_callback, preferences::set_idle_anim, _1, false));
	}

	// sound options
	ERR_GUI_E
		<< preferences::sound_volume() << " "
		<< preferences::music_volume() << " "
		<< preferences::bell_volume() << " "
		<< preferences::UI_volume() << std::endl;
	tgrid* sound_options = &options_panel_->page_grid(2);
	if ((slider = find_widget<tslider>(sound_options, "sound_effects_slider", false, false))) {
		slider->set_value(preferences::sound_volume());
		slider->set_active(preferences::sound_on());
		slider->set_callback_positioner_move(boost::bind<void>(preferences_callback_slider, preferences::set_sound_volume, _1));
	}
	if ((option = find_widget<tselectable_>(sound_options, "sound_effects", false, false))) {
		option->set_value(preferences::sound_on());
		option->set_callback_state_change(boost::bind<void>(preferences_callback_sound, preferences::set_sound, _1, slider, false));
	}

	if ((slider = find_widget<tslider>(sound_options, "music_slider", false, false))) {
		slider->set_value(preferences::music_volume());
		slider->set_active(preferences::music_on());
		slider->set_callback_positioner_move(boost::bind<void>(preferences_callback_slider, preferences::set_music_volume, _1));
	}
	if ((option = find_widget<tselectable_>(sound_options, "music", false, false))) {
		option->set_value(preferences::music_on());
		option->set_callback_state_change(boost::bind<void>(preferences_callback_sound, preferences::set_music, _1, slider, false));
	}

	if ((slider = find_widget<tslider>(sound_options, "turn_bell_slider", false, false))) {
		slider->set_value(preferences::bell_volume());
		slider->set_active(preferences::turn_bell());
		slider->set_callback_positioner_move(boost::bind<void>(preferences_callback_slider, preferences::set_bell_volume, _1));
	}
	if ((option = find_widget<tselectable_>(sound_options, "turn_bell", false, false))) {
		option->set_value(preferences::turn_bell());
		option->set_callback_state_change(boost::bind<void>(preferences_callback_sound, preferences::set_turn_bell, _1, slider, false));
	}

	if ((slider = find_widget<tslider>(sound_options, "ui_sounds_slider", false, false))) {
		slider->set_value(preferences::UI_volume());
		slider->set_active(preferences::UI_sound_on());
		slider->set_callback_positioner_move(boost::bind<void>(preferences_callback_slider, preferences::set_UI_volume, _1));
	}
	if ((option = find_widget<tselectable_>(sound_options, "ui_sounds", false, false))) {
		option->set_value(preferences::UI_sound_on());
		option->set_callback_state_change(boost::bind<void>(preferences_callback_sound, preferences::set_UI_sound, _1, slider, false));
	}

	// advanced_options
	tgrid* advanced_options = &options_panel_->page_grid(3);
	foreach (const config &adv, game_cfg_->child_range("advanced_preference")) {
		if (adv["type"] == "boolean") {
			const std::string& field = adv["field"];
			if ((option = find_widget<tselectable_>(advanced_options, field, false, false))) {
				std::string val = preferences::get(field);
				if (val.empty()) val = adv["default"];
				option->set_value(val == "yes");
				option->set_callback_state_change(boost::bind<void>(advanced_preferences_callback, field, _1));
			}
		}
	}

	page_changed(find_widget<twidget>(&window, "general_select", false, true));
}

void tpreferences::page_changed(twidget* widget) {
	assert(widget);

	tselectable_* select;
	if ((select = find_widget<tselectable_>(window_, "general_select", false, false))) select->set_value(false);
	if ((select = find_widget<tselectable_>(window_, "display_select", false, false))) select->set_value(false);
	if ((select = find_widget<tselectable_>(window_, "sound_select", false, false))) select->set_value(false);
	if ((select = find_widget<tselectable_>(window_, "advanced_select", false, false))) select->set_value(false);

	if (widget->id() == "general_select") {
		options_panel_->select_page(0);
	} else if (widget->id() == "display_select") {
		options_panel_->select_page(1);
	} else if (widget->id() == "sound_select") {
		options_panel_->select_page(2);
	} else if (widget->id() == "advanced_select") {
		options_panel_->select_page(3);
	}

	dynamic_cast<tselectable_*>(widget)->set_value(true);
}

void tpreferences::post_show(twindow& /*window*/)
{
}

} // namespace gui2

