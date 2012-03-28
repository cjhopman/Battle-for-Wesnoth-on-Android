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

#include "gui/dialogs/mp_game_wait.hpp"

#include "gui/auxiliary/timer.hpp"
#include "gui/dialogs/helper.hpp"
#include "gui/widgets/drop_button.hpp"
#include "gui/widgets/label.hpp"
#include "gui/widgets/settings.hpp"

#include "gettext.hpp"
#include "gui_adaptor.hpp"
#include "leader_list.hpp"
#include "marked-up_text.hpp"
#include "wml_separators.hpp"

#include <boost/bind.hpp>

#include <sstream>

namespace gui2 {

namespace {
std::map<std::string, string_map> image_label_transform(std::string s) {
	int pos = s.find(COLUMN_SEPARATOR);
	std::string label = s.substr(pos + 1), image = s.substr(1, pos - 1);
	std::map<std::string, string_map> ret = tdrop_button::default_transform(label);
	ret["image"]["label"] = image;
	return ret;
}
void toggle_chat(tbutton* toggle, tgrid* sides, bool& showing_chat) {
	showing_chat = !showing_chat;
	sides->set_visible(showing_chat ? twidget::INVISIBLE : twidget::VISIBLE);
	toggle->set_label(showing_chat ? _("Show Sides") : _("Expand Chat"));
}

} // anonymous namespace

tmp_game_wait::tmp_game_wait(game_display& disp, const config& game_config, mp::chat& c, config& gamelist) :
	mp::wait(disp, game_config, c, gamelist),
	chat_(c),
	update_timer_(0),
	showing_chat_(false)
{
	hide_children();
}

tmp_game_wait::~tmp_game_wait() {
	chat_.set_add_message_callback();
	if (update_timer_) {
		remove_timer(update_timer_);
	}
}

twindow* tmp_game_wait::build_window(CVideo& video) {
	return build(video, get_id(MP_GAME_WAIT));
}

void tmp_game_wait::pre_show(CVideo& /*video*/, twindow& window) {
	window.set_enter_disabled(true);
	window.set_escape_disabled(true);

	if (tlabel* t = find_widget<tlabel>(&window, "title", false, false)) {
		t->set_label(title().get_text());
	}

	if (tscroll_label* chat_label = find_widget<tscroll_label>(&window, "chat_textbox", false, false)) {
		chat_.set_add_message_callback(boost::bind(&mp::chat::update_scroll_label, &chat_, boost::ref(*chat_label)));
	} else {
		chat_.set_add_message_callback();
	}

	if (ttext_box* chat_text = find_widget<ttext_box>(&window, "chat_send", false, false)) {
		chat_text->set_key_press_callback(boost::bind(&tmp_game_wait::chat_key_press_handler, this, _1, _2));
	}

	tbutton& cancel = find_widget<tbutton>(&window, "cancel", false);
	cancel.connect_click_handler(boost::bind(&tmp_game_wait::cancel_handler, this, _3));

	if (tbutton* toggle = find_widget<tbutton>(&window, "toggle_chat", false, false)) {
		tgrid* grid = find_widget<tgrid>(&window, "sides_grid", false, true);
		toggle->connect_click_handler(boost::bind(toggle_chat, toggle, grid, boost::ref(showing_chat_)));
	}

	update_timer_ = add_timer(250
			, boost::bind(&mp::ui::process_network, static_cast<mp::ui*>(this))
			, true);
	window_ = &window;
}

void update_user_list(const std::vector<std::string>& users, twindow* window) {
	if (tlistbox* user_list = find_widget<tlistbox>(window, "user_list", false, false)) {
		user_list->clear();
		std::map<std::string, string_map> data;
		foreach (const std::string& s, users) {
			data["label"]["label"] = s;
			user_list->add_row(data);
		}
		user_list->set_dirty();
	}
}

namespace {
std::string convert_and_ellipsize(std::string s, int len) {
	if (s.find('<') == 0) {
		int e = s.find('>');
		std::stringstream ss;
		ss << "<span foreground=\"#" << font::color2hexa(font::markup2color(s.substr(1, e - 1))) << "\">";
		ss << ellipsize(s.substr(e + 1), len);
		ss << "</span>";
		return ss.str();
	} else {
		return ellipsize(s, len);
	}
}
}
void update_sides(const std::vector<std::string>& sides, twindow* window) {
	if (tlistbox* side_list = find_widget<tlistbox>(window, "side_list", false, false)) {
		side_list->clear();
		std::map<std::string, string_map> data;
		foreach (const std::string& s, sides) {
			data.clear();
			std::stringstream ss(s);
			std::string v;
			std::vector<std::string> vals;
			while (getline(ss, v, COLUMN_SEPARATOR)) {
				vals.push_back(v);
			}

			assert(vals.size() == 9);

			data["player_number"]["label"] = vals[0];
			data["controller"]["label"] = ellipsize(vals[1], 16);
			data["image"]["label"] = vals[2].substr(1);
			data["faction"]["label"] = ellipsize(vals[3], 12);
			data["leader"]["label"] = ellipsize(vals[4], 25);

			ss.clear();
			ss.str(vals[5]);
			int gold;
			ss >> gold;
			data["gold"]["label"] = "Gold: " + lexical_cast<std::string>(gold);

			if (vals[6] != "") vals[6] = "(" + vals[6].substr(8);
			data["income"]["label"] = vals[6];

			data["team"]["label"] = vals[7];
			data["colour"]["label"] = convert_and_ellipsize(vals[8], 15);
			data["colour"]["use_markup"] = "true";

			side_list->add_row(data);
		}
		side_list->set_dirty();
	}
}

void tmp_game_wait::process_network_data(const config& data, const network::connection sock) {
	mp::wait::process_network_data(data, sock);

	if (get_result()) window_->close();

	update_user_list(user_list(), window_);
	update_sides(get_details(), window_);

	window_->invalidate_layout();
}

bool tmp_game_wait::chat_key_press_handler(twidget* w, SDLKey k) {
	if (k == SDLK_RETURN) {
		if (ttext_box* tb = dynamic_cast<ttext_box*>(w)) {
			do_speak(tb->get_value());
			tb->set_value("");
			return true;
		}
	}
	return false;
}

void tmp_game_wait::cancel_handler(bool& handled) {
	set_result(QUIT);
	handled = true;
}

void tmp_game_wait::post_show(twindow& window) {

}

tmp_leader_preview::tmp_leader_preview(const std::vector<const config *> &side_list, int color) :
	faction_(0),
	side_list_(side_list),
	color_(color),
	llm_(side_list)
{
}

twindow* tmp_leader_preview::build_window(CVideo& video) {
	return build(video, get_id(MP_LEADER_PREVIEW));
}

void tmp_leader_preview::pre_show(CVideo& /*video*/, twindow& window) {
	window.set_enter_disabled(true);
	window.set_escape_disabled(true);

	tdrop_button* leader_drop = find_widget<tdrop_button>(&window, "leader_drop", false, true);
	tdrop_button* gender_drop = find_widget<tdrop_button>(&window, "gender_drop", false, true);
	llm_.gui()->set_leader_combo(leader_drop);
	llm_.gui()->set_gender_combo(gender_drop);

	leader_drop->set_transform(image_label_transform);
	leader_drop->set_callback_selection_changed(boost::bind(&tmp_leader_preview::leader_change_handler, this, _2));

	gender_drop->set_transform(image_label_transform);
	gender_drop->set_callback_selection_changed(boost::bind(&tmp_leader_preview::gender_change_handler, this, _2));

	tlistbox* faction_list = find_widget<tlistbox>(&window, "faction_list", false, true);
	foreach (const config *faction, side_list_)
	{
		std::map<std::string, string_map> data;
		data["image"]["label"] = (*faction)["image"];
		data["label"]["label"] = (*faction)["name"];
		faction_list->add_row(data);
	}
	faction_list->set_callback_value_change(boost::bind(&tmp_leader_preview::faction_change_handler, this, _1));

	leader_image_ = find_widget<timage>(&window, "leader_image", false, false);
	recruits_label_ = find_widget<tlabel>(&window, "recruits_label", false, false);
}

void tmp_leader_preview::update_data() {
	if(size_t(faction_) < side_list_.size()) {
		const config& side = *side_list_[faction_];
		std::string faction = side["faction"];

		if(!faction.empty() && faction[0] == font::IMAGE) {
			std::string::size_type p = faction.find_first_of(COLUMN_SEPARATOR);
			if(p != std::string::npos && p < faction.size())
				faction = faction.substr(p+1);
		}
		std::string leader = llm_.get_leader();
		std::string gender = llm_.get_gender();

		std::string image;
		const unit_type *ut = unit_types.find(leader);
		if (ut) {
			const unit_type &utg = ut->get_gender_unit_type(gender);
			image = utg.image() + llm_.get_RC_suffix(utg.flag_rgb());
			if (leader_image_) leader_image_->set_label(image);
		}

		const std::vector<std::string> recruit_list = utils::split(side["recruit"]);
		std::ostringstream ss;
		foreach (std::string s, recruit_list) {
			const unit_type *rt = unit_types.find(s);
			if (!rt) continue;
			ss << rt->type_name() << ", ";
		}
		std::string recruits = ss.str();
		if (recruits_label_) recruits_label_->set_label(recruits.substr(0, recruits.size() - 2));
	}
}

void tmp_leader_preview::faction_change_handler(tlistbox* list) {
	faction_ = std::max(0, list->get_selected_row());
	llm_.update_leader_list(faction_);
	llm_.update_gender_list(llm_.get_leader());
	update_data();
}

void tmp_leader_preview::leader_change_handler(int) {
	llm_.update_gender_list(llm_.get_leader());
	update_data();
}

void tmp_leader_preview::gender_change_handler(int) {
	update_data();
}

void tmp_leader_preview::post_show(twindow& /*window*/) {
}

int tmp_leader_preview::get_selected_faction() {
	return faction_;
}
std::string tmp_leader_preview::get_selected_leader() {
	return llm_.get_leader();
}
std::string tmp_leader_preview::get_selected_gender() {
	return llm_.get_gender();
}

} // namespace gui2
