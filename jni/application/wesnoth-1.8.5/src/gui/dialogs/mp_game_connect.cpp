/*
   Copyright (C) 2008 - 2011 by Chris Hopman <cjhopman@gmail.com>
   Part of the Battle for Wesnoth Project http://www.wesnoth.org/

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License version 2
   or at your option any later version.
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY.

   See the COPYING file for more details.
*/

#define GETTEXT_DOMAIN "wesnoth-lib"

#include "gui/dialogs/mp_game_connect.hpp"

#include "gui/auxiliary/timer.hpp"
#include "gui/dialogs/field.hpp"
#include "gui/dialogs/helper.hpp"
#include "gui/dialogs/message.hpp"
#include "gui/widgets/drop_button.hpp"
#include "gui/widgets/integer_selector.hpp"
#include "gui/widgets/label.hpp"
#include "gui/widgets/minimap.hpp"
#include "gui/widgets/multi_page.hpp"
#include "gui/widgets/scrollbar_panel.hpp"
#include "gui/widgets/settings.hpp"
#include "gui/widgets/spacer.hpp"
#include "gui/widgets/text_box.hpp"

#include "display.hpp"
#include "foreach.hpp"
#include "formula_string_utils.hpp"
#include "game_preferences.hpp"
#include "gettext.hpp"
#include "log.hpp"
#include "map.hpp"
#include "map_exception.hpp"
#include "mp_game_settings.hpp"
#include "savegame.hpp"
#include "../../settings.hpp"
#include "unit_id.hpp"
#include "wml_separators.hpp"

#include <boost/bind.hpp>

#include <map>
#include <algorithm>
#include <iterator>
#include <memory>

namespace gui2 {

tside_ui::tside_ui(twidget* widget, tconnect_ui* parent) : widget_(widget), parent_(parent) {
	setup_adaptors();
}
tside_ui::tside_ui(const tside_ui& o) : super(o), widget_(o.widget_), parent_(o.parent_) {
	setup_adaptors();
}

void tside_ui::set_widget(twidget* widget) {
	widget_ = widget;
	setup_adaptors();
}

void setup_combo(twidget* widget, gui_adaptor::list& combo, std::string id,
		tdrop_button::callback_selection_changed_type cb,
		tdrop_button::transform_type t = tdrop_button::transform_type()) {
	tdrop_button* drop = find_widget<tdrop_button>(widget, id, false, false);
	if (drop) {
		drop->set_transform(t);
		drop->set_callback_selection_changed(cb);
	}
	combo.set_widget(drop);
}

void tside_ui::change_ai_algorithm_cb(int c) {
	side_->set_ai(c);
	parent_->full_update();
}
void tside_ui::change_controller_cb(int c) {
	side_->set_controller(c);
	parent_->full_update();
}
void tside_ui::change_faction_cb(int c) {
	side_->set_faction(c);
	parent_->full_update();
}
void tside_ui::change_leader_cb(int) {
	side_->update_gender_list();
	parent_->full_update();
}
void tside_ui::change_gender_cb(int) {
	side_->change();
	parent_->full_update();
}
void tside_ui::change_team_cb(int c) {
	side_->set_team(c);
	parent_->full_update();
}
void tside_ui::change_colour_cb(int c) {
	side_->set_colour(c);
	parent_->full_update();
}
void tside_ui::change_gold_cb() {
	side_->set_gold(slider_gold_.get());
	parent_->full_update();
}
void tside_ui::change_income_cb() {
	side_->set_income(slider_income_.get());
	parent_->full_update();
}
std::map<std::string, string_map> image_label_transform(std::string s) {
	int pos = s.find(COLUMN_SEPARATOR);
	std::string label = s.substr(pos + 1), image = s.substr(1, pos - 1);
	std::map<std::string, string_map> ret = tdrop_button::default_transform(label);
	ret["image"]["label"] = image;
	return ret;
}

std::string gold_formatter(int v) {
	return "Gold: " + tslider::default_formatter(v);
}
std::string income_formatter(int v) {
	std::string s;
	if (v < 0) {
		s = "(" + tslider::default_formatter(v) + ")";
	} else if (v == 0) {
		s = "Normal";
	} else {
		s = "+" + tslider::default_formatter(v);
	}
	return "Income: " + s;
}
void tside_ui::setup_adaptors() {
	player_number_.set_widget(find_widget<tlabel>(widget_, "player_number", false, false));
	orig_controller_.set_widget(find_widget<tlabel>(widget_, "orig_controller", false, false));
	label_gold_.set_widget(find_widget<tlabel>(widget_, "gold_label", false, false));
	label_income_.set_widget(find_widget<tlabel>(widget_, "income_label", false, false));

	if (tslider* slider = find_widget<tslider>(widget_, "gold_slider", false, false)) {
		slider_gold_.set_widget(slider);
		slider->set_formatter(gold_formatter);
		slider->set_callback_positioner_released(boost::bind(&tside_ui::change_gold_cb, this));
	}
	if (tslider* slider = find_widget<tslider>(widget_, "income_slider", false, false)) {
		slider_income_.set_widget(slider);
		slider->set_formatter(income_formatter);
		slider->set_callback_positioner_released(boost::bind(&tside_ui::change_income_cb, this));
	}

	setup_combo(widget_, combo_ai_algorithm_, "ai_algorithm",
			boost::bind(&tside_ui::change_ai_algorithm_cb, this, _2));
	setup_combo(widget_, combo_controller_, "controller",
			boost::bind(&tside_ui::change_controller_cb, this, _2));
	setup_combo(widget_, combo_faction_, "faction",
			boost::bind(&tside_ui::change_faction_cb, this, _2), image_label_transform);
	setup_combo(widget_, combo_leader_, "leader",
			boost::bind(&tside_ui::change_leader_cb, this, _2), image_label_transform);
	setup_combo(widget_, combo_gender_, "gender",
			boost::bind(&tside_ui::change_gender_cb, this, _2), image_label_transform);
	setup_combo(widget_, combo_team_, "team",
			boost::bind(&tside_ui::change_team_cb, this, _2));
	setup_combo(widget_, combo_colour_, "colour",
			boost::bind(&tside_ui::change_colour_cb, this, _2));
}

tconnect_ui::tconnect_ui(twindow& window, game_display& disp, const config& game_config,
		mp::chat& c, config& gamelist, const mp_game_settings& params) : 
	mp::connect_ui(disp, game_config, c, gamelist, params),
	window_(window)
{ 
	hide_children();
}

bool tconnect_ui::hidden() const { return false; }

void tconnect_ui::show_sides(const std::vector<mp::connect_side>& sides) {
	int flags =
		tgrid::HORIZONTAL_GROW_SEND_TO_CLIENT |
		tgrid::VERTICAL_GROW_SEND_TO_CLIENT |
		tgrid::BORDER_ALL;
	tscrollbar_panel* player_list = find_widget<tscrollbar_panel>(&window_, "game_players", false, true);
	tgrid* player_grid = player_list->content_grid();
	std::list<tside_ui>::iterator ui_it = sides_.begin();
	std::vector<mp::connect_side>::const_iterator it = sides.begin();
	for (; ui_it != sides_.end(); ++ui_it, ++it) {
		if (!it->allow_player()) {
			std::auto_ptr<tspacer> spacer(new tspacer());
			spacer->set_definition("default");
			ui_it->set_widget(spacer.get());
			player_grid->set_child(spacer.release(), it - sides.begin(), 0, flags, 0);
		}
	}
	for (size_t i = sides.size(); i < player_grid->get_rows(); ++i) {
		std::auto_ptr<tspacer> spacer(new tspacer());
		spacer->set_definition("default");
		player_grid->set_child(spacer.release(), i, 0, flags, 0);
	}
}
mp::side_ui* tconnect_ui::next_side_ui() { 
	tscrollbar_panel* player_list = find_widget<tscrollbar_panel>(&window_, "game_players", false, true);
	tgrid* player_grid = player_list->content_grid();
	tgrid* grid = dynamic_cast<tgrid*>(player_grid->widget(sides_.size(), 0));
	assert(grid);
	sides_.push_back(tside_ui(grid, this));
	return &sides_.back();
}

void tconnect_ui::set_user_list(const std::vector<std::string>& list, bool silent) {
	mp::connect_ui::set_user_list(list, silent);
	if (tlistbox* user_list = find_widget<tlistbox>(&window_, "user_list", false, false)) {
		user_list->clear();
		std::map<std::string, string_map> data;
		foreach(const std::string& s, list) {
			data["username"]["label"] = s;
			user_list->add_row(data);
		}
		window_.invalidate_layout();
	}
}

tmp_game_connect::tmp_game_connect(game_display& disp, const config& game_config, mp::chat& c, config& gamelist,
			const mp_game_settings& params, const mp::controller default_controller, bool local_players_only, game_state& state) :
	disp_(disp),
	game_config_(game_config),
	chat_(c),
	gamelist_(gamelist),
	params_(params),
	default_controller_(default_controller),
	local_players_only_(local_players_only),
	state_(state),
	update_timer_(0),
	showing_chat_(true)
{
}

tmp_game_connect::~tmp_game_connect() {
	chat_.set_add_message_callback();
	if (update_timer_) {
		remove_timer(update_timer_);
	}
}


twindow* tmp_game_connect::build_window(CVideo& video)
{
	return build(video, get_id(MP_GAME_CONNECT));
}

bool tmp_game_connect::chat_key_press_handler(twidget* w, SDLKey k) {
	if (k == SDLK_RETURN) {
		if (ttext_box* tb = dynamic_cast<ttext_box*>(w)) {
			ui_->do_speak(tb->get_value());
			tb->set_value("");
			return true;
		}
	}
	return false;
}

namespace {
void toggle_chat(tbutton* toggle, tmulti_page* multi, bool& showing_chat) {
	showing_chat = !showing_chat;
	multi->select_page(showing_chat ? 1 : 0);
	toggle->set_label(showing_chat ? _("Show Sides") : _("Show Chat"));
}
void collapse_multi_grid(tgrid& grid) {
	tmulti_page* multi = dynamic_cast<tmulti_page*>(grid.widget(0, 0));
	assert(multi);
	for (unsigned i = 1; i < grid.get_rows(); i++) {
		std::auto_ptr<twidget> aw(new tspacer());
		aw->set_id("sp_temp");
		std::string id = grid.widget(i, 0)->id();
		if (id == "") id = (grid.widget(i, 0)->set_id("__temp"), "__temp");
		aw.reset(grid.swap_child(id, aw.release(), false));
		assert(aw.get());
		if (id == "__temp") aw->set_id("");
		multi->add_page(string_map());
		multi->page_grid(i - 1).set_child(aw.release(), 0, 0,
				tgrid::VERTICAL_GROW_SEND_TO_CLIENT | tgrid::HORIZONTAL_GROW_SEND_TO_CLIENT | tgrid::BORDER_ALL, 5);
	}
	grid.remove_child("sp_temp", true);
	grid.set_rows_cols(1, 1);
}
} // anonymous namespace

void tmp_game_connect::pre_show(CVideo& /*video*/, twindow& window)
{
	window.set_enter_disabled(true);
	window.set_escape_disabled(true);
	ui_.reset(new tconnect_ui(window, disp_, game_config_, chat_, gamelist_, params_));
	connect_.reset(new mp::connect(ui_.get(), game_config_, gamelist_, params_, default_controller_, local_players_only_));

	if (tscroll_label* chat_label = find_widget<tscroll_label>(&window, "chat_textbox", false, false)) {
		chat_.set_add_message_callback(boost::bind(&mp::chat::update_scroll_label, &chat_, boost::ref(*chat_label)));
	} else {
		chat_.set_add_message_callback();
	}

	if (ttext_box* chat_text = find_widget<ttext_box>(&window, "chat_send", false, false)) {
		chat_text->set_key_press_callback(boost::bind(&tmp_game_connect::chat_key_press_handler, this, _1, _2));
	}

	if (tbutton* toggle = find_widget<tbutton>(&window, "toggle_chat", false, false)) {
		tgrid& multi_grid = find_widget<tgrid>(&window, "multi_grid", false);
		collapse_multi_grid(multi_grid);

		tmulti_page* multi_page = dynamic_cast<tmulti_page*>(multi_grid.widget(0, 0));
		assert(multi_page);

		toggle->connect_click_handler(boost::bind(toggle_chat, toggle, multi_page, boost::ref(showing_chat_)));
		toggle_chat(toggle, multi_page, showing_chat_);
	}

	tbutton& ready = find_widget<tbutton>(&window, "ready", false);
	ready.connect_click_handler(boost::bind(&tmp_game_connect::ready_handler, this, &window, _3));

	tbutton& cancel = find_widget<tbutton>(&window, "cancel", false);
	cancel.connect_click_handler(boost::bind(&tmp_game_connect::cancel_handler, this, &window, _3));

	ui_->full_update();

	update_timer_ = add_timer(250
			, boost::bind(&mp::ui::process_network, ui_.get())
			, true);

	if (tlabel* title = find_widget<tlabel>(&window, "title", false, false)) {
		title->set_label(ui_->title().get_text());
	}
}

void tmp_game_connect::ready_handler(twindow* window, bool& handled) {
	connect_->try_launch();
	if (get_result() == mp::ui::PLAY) {
		window->close();
	}
	handled = true;
}

void tmp_game_connect::cancel_handler(twindow* window, bool& handled) {
	if(network::nconnections() > 0) {
		config cfg;
		cfg.add_child("leave_game");
		network::send_data(cfg, 0, true);
	}
	set_result(mp::ui::QUIT);
	window->close();
	handled = true;
}

void tmp_game_connect::post_show(twindow& /*window*/)
{
	if(get_result() == mp::ui::PLAY) {
		connect_->start_game();
		state_ = connect_->get_state();
	}
}

} // namespace gui2
