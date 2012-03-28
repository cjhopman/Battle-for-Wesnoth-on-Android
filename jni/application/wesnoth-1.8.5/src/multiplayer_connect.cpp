/* $Id: multiplayer_connect.cpp 49411 2011-05-07 15:11:50Z soliton $ */
/*
   Copyright (C) 2007 - 2011
   Part of the Battle for Wesnoth Project http://www.wesnoth.org

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License version 2
   or at your option any later version.
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY.

   See the COPYING file for more details.
*/

/**
 * @file multiplayer_connect.cpp
 * Prepare to join a multiplayer-game.
 */

#include "global.hpp"

#include "ai/configuration.hpp"
#include "dialogs.hpp"
#include "foreach.hpp"
#include "game_display.hpp"
#include "gettext.hpp"
#include "log.hpp"
#include "map.hpp"
#include "multiplayer_connect.hpp"
#include "savegame.hpp"
#include "statistics.hpp"
#include "unit_id.hpp"
#include "wml_separators.hpp"
#include "formula_string_utils.hpp"
#include "tod_manager.hpp"

#include <boost/bind.hpp>
using namespace boost;

static lg::log_domain log_network("network");
#define LOG_NW LOG_STREAM(info, log_network)

static lg::log_domain log_config("config");
#define LOG_CF LOG_STREAM(info, log_config)
#define WRN_CF LOG_STREAM(warn, log_config)
#define ERR_CF LOG_STREAM(err, log_config)

namespace {
	const char* controller_names[] = {
		"network",
		"human",
		"ai",
		"null",
		"reserved"
	};

}

namespace mp {

connect_side::connect_side(connect& parent, const config& cfg, int index, side_ui* ui) :
	parent_(&parent),
	cfg_(cfg),
	index_(index),
	id_(cfg_.get_attribute("id")),
	player_id_(cfg_.get_attribute("player_id")),
	save_id_(cfg_.get_attribute("save_id")),
	current_player_(cfg_.get_attribute("current_player")),
	controller_(CNTR_NETWORK),
	faction_(lexical_cast_default<int>(cfg_.get_attribute("faction"), 0)),
	team_(0),
	colour_(index),
	gold_(lexical_cast_default<int>(cfg_.get_attribute("gold"), 100)),
	income_(lexical_cast_default<int>(cfg_.get_attribute("income"), 0)),
	leader_(),
	gender_(),
	ai_algorithm_(),
	ready_for_start_(false),
	gold_lock_(utils::string_bool(cfg_.get_attribute("gold_lock"), false)),
	income_lock_(utils::string_bool(cfg_.get_attribute("income_lock"), false)),
	team_lock_(utils::string_bool(cfg_.get_attribute("team_lock"), false)),
	colour_lock_(utils::string_bool(cfg_.get_attribute("colour_lock"), false)),
	allow_player_(utils::string_bool(cfg_.get_attribute("allow_player"), true)),
	allow_changes_(utils::string_bool(cfg_.get_attribute("allow_changes"), true)),
	enabled_(!parent_->params_.saved_game),
	changed_(false),
	ui_(ui),
	llm_(parent.era_sides_, leader_list_gui())
{
	ui_->set_side(this);

	ui_->set_team_list(parent_->player_teams_);
	ui_->set_leader_list(parent_->player_types_);
	ui_->set_colour_list(parent_->player_colours_);

	ui_->set_player(current_player_);
	ui_->set_player_number(lexical_cast<std::string>(index + 1));

	ui_->enable_llm(llm_, true);
	ui_->set_gold(gold_);
	ui_->set_income(income_);

	ui_->enable_faction(enabled_);
	ui_->enable_leader(enabled_);
	ui_->enable_gender(enabled_);
	ui_->enable_team(enabled_);
	ui_->enable_colour(enabled_);
	ui_->enable_gold(!enabled_);
	ui_->enable_income(!enabled_);

	// convert ai controllers
	if (cfg_["controller"] == "human_ai"
			|| cfg_["controller"] == "network_ai")
		cfg_["controller"] = "ai";
	if(allow_player_ && enabled_) {
		controller_ = parent_->default_controller_;
	} else {
		size_t i = CNTR_NETWORK;
		if(!allow_player_)
		{
			if(cfg_["controller"] == "null") {
				controller_ = CNTR_EMPTY;
			} else {
				cfg_["controller"] = controller_names[CNTR_COMPUTER];
				controller_ = static_cast<mp::controller>(CNTR_COMPUTER);
			}
		} else {
			if (cfg_["controller"] == "network"
					|| cfg_["controller"] == "human")
			{
				cfg_["controller"] = "reserved";

			}
			for(; i != CNTR_LAST; ++i) {
				if(cfg_["controller"] == controller_names[i]) {
					controller_ = static_cast<mp::controller>(i);
					break;
				}
			}
		}
	}

	std::vector<std::string>::const_iterator itor =
			std::find(parent_->team_names_.begin(), parent_->team_names_.end(),
					cfg_.get_attribute("team_name"));
	if(itor == parent_->team_names_.end()) {
		assert(!parent_->team_names_.empty());
		team_ = 0;
	} else {
		team_ = itor - parent_->team_names_.begin();
	}

	if(!cfg_.get_attribute("colour").empty()) {
		colour_ = game_config::color_info(cfg_["colour"]).index() - 1;
	}
	llm_.set_colour(colour_);

	update_faction_combo();

	if (const config &ai = cfg_.child("ai"))
		ai_algorithm_ = lexical_cast_default<std::string>(ai["ai_algorithm"], "default");
	else
		ai_algorithm_ = "default";
	init_ai_algorithm_combo();

	// "Faction name" hack
	if (!enabled_) {
		ui_->enable_llm(llm_, false);

		faction_ = 0;
		std::vector<std::string> pseudo_factions;
		pseudo_factions.push_back(cfg_["name"]);
		ui_->set_faction_list(pseudo_factions, 0);

		// Hack: if there is a unit which can recruit, use it as a leader.
		// Necessary to display leader information when loading saves.
		config::const_child_itors units = cfg.child_range("unit");
		config::const_child_iterator leader = std::find_if(units.first, units.second,
				bind<bool>(utils::string_bool, bind(&config::get_attribute, _1, "canrecruit"), false));

		const unit_type *leader_type = NULL;
		if (leader != units.second && !(*leader)["type"].empty()) {
			leader_type = unit_types.find((*leader)["type"]);
		}

		std::string leader_name("-"), gender_name("-");
		if (leader_type) {
			gender_ = (*leader)["gender"];

			if (gender_ == "female")
				gender_name = _("Female ♀");
			else if (gender_ == "male")
				gender_name = _("Male ♂");
			else if (gender_ == "random")
				gender_name = _("gender^Random");
			else
				gender_name = "?";

			leader_name = leader_type->get_gender_unit_type(gender_).type_name();
		}

		std::vector<std::string> leader_name_pseudolist;
		std::vector<std::string> gender_name_pseudolist;

		leader_name_pseudolist.push_back(leader_name);
		gender_name_pseudolist.push_back(gender_name);

		ui_->set_leader_list(leader_name_pseudolist);
		ui_->set_gender_list(gender_name_pseudolist);

	} else if(parent_->params_.use_map_settings) {
		// gold, income, team, and colour are only suggestions unless explicitly locked
		ui_->enable_gold(!gold_lock_);
		ui_->enable_income(!income_lock_);
		ui_->enable_team(!team_lock_);
		ui_->enable_colour(!colour_lock_);

		// Set the leader and gender
		leader_ = cfg_.get_attribute("type");
		gender_ = cfg_.get_attribute("gender");
		if(!leader_.empty()) {
			ui_->enable_leader(false);
			ui_->enable_gender(false);
			ui_->enable_llm(llm_, false);

			std::string leader_name("?"), gender_name("?");
			const unit_type *leader_type = unit_types.find(leader_);
			if (leader_type) {
				leader_name = leader_type->type_name();
				if (gender_ == "female")
					gender_name = _("Female ♀");
				else if (gender_ == "male")
					gender_name = _("Male ♂");
				else if (gender_ == "random")
					gender_name = _("gender^Random");
			}

			std::vector<std::string> leader_name_pseudolist;
			std::vector<std::string> gender_name_pseudolist;

			leader_name_pseudolist.push_back(leader_name);
			gender_name_pseudolist.push_back(gender_name);

			ui_->set_leader_list(leader_name_pseudolist);
			ui_->set_gender_list(gender_name_pseudolist);
		}

		// Try to pick a faction for the sake of appearance
		// and for filling in the blanks
		if(faction_ == 0) {
			std::vector<std::string> find;
			std::string search_field;
			if(!cfg_.get_attribute("faction").empty()) {
				// Choose based on faction
				find.push_back(cfg_["faction"]);
				search_field = "id";
			} else if(utils::string_bool(cfg["faction_from_recruit"]) && !cfg_.get_attribute("recruit").empty()) {
				// Choose based on recruit
				find = utils::split(cfg_["recruit"]);
				search_field = "recruit";
			} else if(!leader_.empty()) {
				// Choose based on leader
				find.push_back(leader_);
				search_field = "leader";
			}
			// Pick the first faction with the greater amount of data matching the criteria
			int best_score = 0;

			std::vector<const config*>::const_iterator faction;
			for (faction = parent.era_sides_.begin(); faction != parent.era_sides_.end(); ++faction) {
				const config& side = (**faction);

				typedef std::vector<std::string> string_vec;
				string_vec recruit = utils::split(side[search_field]);

				int faction_score = 0;
				foreach(std::string f, find) {
					faction_score += std::find(recruit.begin(), recruit.end(), f) != recruit.end();
				}

				if(faction_score > best_score) {
					best_score = faction_score;
					faction_ = faction - parent.era_sides_.begin();
				}
			}
		}

		update_faction_ui();
		ui_->enable_faction(faction_ == 0);
	}

	update_ui();
}

connect_side::connect_side(const connect_side& a) :
	parent_(a.parent_), cfg_(a.cfg_),
	index_(a.index_), id_(a.id_), player_id_(a.player_id_),  save_id_(a.save_id_),
	current_player_(a.current_player_),
	controller_(a.controller_),
	faction_(a.faction_), team_(a.team_), colour_(a.colour_),
	gold_(a.gold_), income_(a.income_), leader_(a.leader_),
	gender_(a.gender_),
	ai_algorithm_(a.ai_algorithm_),
	ready_for_start_(a.ready_for_start_),
	gold_lock_(a.gold_lock_),
	income_lock_(a.income_lock_),
	team_lock_(a.team_lock_),
	colour_lock_(a.colour_lock_),
	allow_player_(a.allow_player_), allow_changes_(a.allow_changes_),
	enabled_(a.enabled_), changed_(a.changed_),
	ui_(a.ui_),
	llm_(a.llm_)
{
	ui_->set_side(this);
	llm_.set_colour(colour_);
	ui_->enable_llm(llm_, enabled_ && leader_.empty());
	// FIXME: this is an ugly hack to force updating the gender list when the side
	// widget is initialized. Need an optimal way. -- shadowmaster
	llm_.update_gender_list(llm_.get_leader());
}

void connect_side::update_faction_ui() {
	llm_.update_leader_list(faction_);
	llm_.update_gender_list(llm_.get_leader());
}

void connect_side::swap_player(int index) {
	side& other = parent_->sides_[index];
	std::string other_player_id = other.player_id_;
	mp::controller other_controller = other.controller_;

	if (player_id_.empty()) {
		other.set_controller(controller_);
	} else {
		other.set_player_id(player_id_);
	}

	if (other_player_id.empty()) {
		set_controller(other_controller);
	} else {
		set_player_id(other_player_id);
	}

	std::swap(other.ai_algorithm_, ai_algorithm_);

	change();
	init_ai_algorithm_combo();
	update_ui();

	other.change();
	other.init_ai_algorithm_combo();
	other.update_ui();
}

void connect_side::set_controller(int cntr) {
	const int cntr_last = CNTR_LAST - save_id_.empty(); // non-save games drop CNTR_RESERVED
	if (parent_->local_only_) {
		cntr++; // local only drops CNTR_NETWORK
	}

	if (cntr == cntr_last) {
		update_controller_ui();
	} else if (cntr < cntr_last) {
		// Correct entry number if CNTR_NETWORK is not allowed for combo_controller_
		controller_ = mp::controller(cntr);
		player_id_ = "";
		ready_for_start_ = false;
		change();
	} else {
		// give user second side
		size_t user = cntr - cntr_last - 1;

		const std::string new_id = parent_->users_[user].name;
		if (new_id != player_id_) {
			player_id_ = new_id;
			controller_ = parent_->users_[user].controller;
			ready_for_start_ = true;
			change();
		}
	}

	update_ai_algorithm_combo();
}

void connect_side::set_colour(int c) {
	colour_ = c;
	llm_.set_colour(c);
	update_faction_combo();
	change();
}

void connect_side::set_faction(int f) {
	faction_ = f;
	llm_.update_leader_list(faction_);
	llm_.update_gender_list(llm_.get_leader());
	change();
}

void connect_side::set_ai(int i) {
	ai_algorithm_ = parent_->ai_algorithms_[i]->id;
	change();
}

void connect_side::set_team(int t) {
	team_ = t;
	change();
}

void connect_side::set_gold(int g) {
	gold_ = g;
	change();
}

void connect_side::set_income(int i) {
	income_ = i;
	change();
}

void connect_side::update_gender_list() {
	llm_.update_gender_list(llm_.get_leader());
	change();
}

bool connect_side::changed()
{
	bool res = changed_;
	changed_ = false;
	return res;
}

bool connect_side::ready_for_start() const
{
	//sides without players are always ready
	if (!allow_player_)
		return true;

	//the host and the AI are always ready
	if ((controller_ == mp::CNTR_COMPUTER) ||
		(controller_ == mp::CNTR_EMPTY) ||
		(controller_ == mp::CNTR_LOCAL))
		return true;

	return ready_for_start_;
}

bool connect_side::available(const std::string& name) const
{
	if (name.empty())
	{
		return allow_player_
			&& ((controller_ == CNTR_NETWORK && player_id_.empty())
					|| controller_ == CNTR_RESERVED);
	}
	return allow_player_
		&& ((controller_ == CNTR_NETWORK && player_id_.empty())
			|| (controller_ == CNTR_RESERVED && current_player_ == name));
}

bool connect_side::allow_player() const
{
	return allow_player_;
}

void connect_side::update_controller_ui()
{
	if (player_id_.empty()) {
		ui_->set_controller(controller_ - (parent_->local_only_ ? 1 : 0));
	} else {
		connected_user_list::iterator player = parent_->find_player(player_id_);

		if (player != parent_->users_.end()) {
			const int player_first = CNTR_LAST - (save_id_.empty() ? 1 : 0) - (parent_->local_only_ ? 1 : 0) + 1;
			ui_->set_controller(player_first + (player - parent_->users_.begin()));
		} else {
			assert(parent_->local_only_ != true);
			ui_->set_controller(CNTR_NETWORK);
		}
	}

    update_ai_algorithm_combo();
}

bool connect_side::should_show_ai_combo() const {
	return controller_ != CNTR_COMPUTER;
}

void connect_side::hide_ai_algorithm_combo(bool invis)
{
	ui_->hide_ai(invis || !should_show_ai_combo());
	ui_->hide_orig_controller(invis || should_show_ai_combo());
}

void connect_side::init_ai_algorithm_combo()
{
	assert(!parent_->ai_algorithms_.empty());

	std::vector<ai::description*> &ais_list = parent_->ai_algorithms_;
	std::vector<std::string> ais;

	std::transform(ais_list.begin(), ais_list.end(), back_inserter(ais),
			bind(&ai::description::text, _1));

	int sel = find_if(ais_list.begin(), ais_list.end(),
			bind(&ai::description::id, _1) == ai_algorithm_)
		- ais_list.begin();

	ui_->set_ai_list(ais, sel);
}

void connect_side::update_faction_combo()
{
	std::vector<std::string> factions;
	foreach (const config *faction, parent_->era_sides_)
	{
		const std::string& name = (*faction)["name"];
		const std::string& icon = (*faction)["image"];
		if (!icon.empty()) {
			std::string rgb = (*faction)["flag_rgb"];
			if (rgb.empty())
				rgb = "magenta";

			factions.push_back(IMAGE_PREFIX + icon + "~RC(" + rgb + ">" + lexical_cast<std::string>(colour_+1) + ")" + COLUMN_SEPARATOR + name);
		} else {
			factions.push_back(name);
		}
	}
	ui_->set_faction_list(factions, faction_);
}

void connect_side::update_ui()
{
	update_controller_ui();

	ui_->set_faction(faction_);
	ui_->set_team(team_);
	ui_->set_colour(colour_);
	ui_->set_gold(gold_);
	ui_->set_income(income_);
}

void connect_side::update_ai_algorithm_combo() {
	hide_ai_algorithm_combo(parent_->ui_->hidden());
}

config connect_side::get_config() const
{
	config res;

	// If the user is allowed to change type, faction, leader etc,
	// then import their new values in the config.
	if(enabled_ && !parent_->era_sides_.empty()) {
		// Merge the faction data to res
		res.append(*(parent_->era_sides_[faction_]));
		res["faction_name"] = res["name"];
	}
	res.append(cfg_);
	if (cfg_.get_attribute("side").empty()
			|| cfg_["side"] != lexical_cast<std::string>(index_ + 1))
	{
		res["side"] = lexical_cast<std::string>(index_ + 1);
	}
	res["controller"] = controller_names[controller_];
	res["current_player"] = player_id_.empty() ? current_player_ : player_id_;
	res["id"] = id_;

	if (player_id_.empty()) {
		std::string description;
		switch(controller_) {
		case CNTR_NETWORK:
			description = N_("(Vacant slot)");
			break;
		case CNTR_LOCAL:
			if(enabled_ && cfg_.get_attribute("save_id").empty()) {
				res["save_id"] = preferences::login() + res["side"].str();
			}
			res["player_id"] = preferences::login() + res["side"].str();
			res["current_player"] = preferences::login();
			description = N_("Anonymous local player");
			break;
		case CNTR_COMPUTER:
			if(enabled_ && cfg_.get_attribute("save_id").empty()) {
				res["save_id"] = "ai" + res["side"].str();
			}
			{
				utils::string_map symbols;
				if (allow_player_) {
					const config &ai_cfg = ai::configuration::get_ai_config_for(ai_algorithm_);
					res.add_child("ai",ai_cfg);
					symbols["playername"] = ai_cfg["description"];
				} else { // do not import default ai cfg here - all is set by scenario config
					symbols["playername"] = _("Computer Player");
				}
				symbols["side"] = res["side"].str();
				description = vgettext("$playername $side",symbols);
			}
			break;
		case CNTR_EMPTY:
			description = N_("(Empty slot)");
			res["no_leader"] = "yes";
			break;
		case CNTR_RESERVED:
			{
				utils::string_map symbols;
				symbols["playername"] = current_player_;
				description = vgettext("(Reserved for $playername)",symbols);
			}
			break;
		case CNTR_LAST:
		default:
			description = N_("(empty)");
			assert(false);
			break;
		}
		res["user_description"] = t_string(description, "wesnoth");
	} else {
		res["player_id"] = player_id_ + res["side"];
		if(enabled_ && cfg_.get_attribute("save_id").empty()) {
			res["save_id"] = player_id_ + res["side"].str();
		}

		res["user_description"] = player_id_;
	}

	res["name"] = res["user_description"];
	res["allow_changes"] = (enabled_ && allow_changes_) ? "yes" : "no";

	if(enabled_) {
		if (leader_.empty()) {
			res["type"] = llm_.get_leader();
		} else {
			res["type"] = leader_;
		}
		if (gender_.empty()) {
			std::string dummy = llm_.get_gender();
			if (!dummy.empty() && dummy != "null" && dummy != "?")
				res["gender"] = dummy;
		} else {
			// If no genders could be resolved, let the unit engine use
			// the default gender
			if (gender_ != "null")
				res["gender"] = gender_;
		}
		res["team_name"] = parent_->team_names_[team_];
		res["user_team_name"] = parent_->user_team_names_[team_];
		res["allow_player"] = allow_player_ ? "yes" : "no";
		res["colour"] = lexical_cast<std::string>(colour_ + 1);
		res["gold"] = lexical_cast<std::string>(gold_);
		res["income"] = lexical_cast<std::string>(income_);

		if(!parent_->params_.use_map_settings || res["fog"].empty() || (res["fog"] != "yes" && res["fog"] != "no")) {
			res["fog"] = parent_->params_.fog_game ? "yes" : "no";
		}

		if(!parent_->params_.use_map_settings || res["shroud"].empty() || (res["shroud"] != "yes" && res["shroud"] != "no")) {
			res["shroud"] = parent_->params_.shroud_game ? "yes" : "no";
		}

		res["share_maps"] = parent_->params_.share_maps ? "yes" : "no";
		res["share_view"] =  parent_->params_.share_view ? "yes" : "no";
		if(!parent_->params_.use_map_settings || res["village_gold"].empty())
			res["village_gold"] = lexical_cast<std::string>(parent_->params_.village_gold);
	}

	if(parent_->params_.use_map_settings && enabled_) {
		config trimmed = cfg_;
		trimmed["side"] = "";
		trimmed["controller"] = "";
		trimmed["id"] = "";
		trimmed["team_name"] = "";
		trimmed["user_team_name"] = "";
		trimmed["colour"] = "";
		trimmed["gold"] = "";
		trimmed["income"] = "";
		trimmed["allow_changes"] = "";
/*
		trimmed["allow_player"] = "";
		trimmed["colour_lock"] = "";
		trimmed["faction"] = "";
		trimmed["gender"] = "";
		trimmed["gold_lock"] = "";
		trimmed["income_lock"] = "";
		trimmed["recruit"] = "";
		trimmed["team_lock"] = "";
*/
		if(controller_ != CNTR_COMPUTER) {
			// Only override names for computer controlled players
			trimmed["user_description"] = "";
		}
		trimmed.prune();
		res.merge_with(trimmed);
	}
	return res;
}

void connect_side::set_controller(mp::controller controller)
{
	controller_ = controller;
	player_id_ = "";

	update_ui();
}

mp::controller connect_side::get_controller() const
{
	return controller_;
}

void connect_side::update_user_list()
{
	typedef std::vector<std::string> name_list;

	name_list list = parent_->player_types_;
	if (!save_id_.empty())
		list.push_back(_("Reserved"));
	list.push_back(_("--give--"));

	std::transform(parent_->users_.begin(), parent_->users_.end(), back_inserter(list),
			bind(&connected_user::name, _1));

	if (std::find_if(parent_->users_.begin(), parent_->users_.end(),
				(bind(&connected_user::name, _1) == player_id_))
			== parent_->users_.end()) {
		player_id_ = "";
	}

	ui_->set_controller_list(list);
	update_controller_ui();
}

const std::string& connect_side::get_player_id() const
{
	return player_id_;
}

void connect_side::set_player_id(const std::string& player_id)
{
	connected_user_list::iterator i = parent_->find_player(player_id);
	if (i != parent_->users_.end()) {
		player_id_ = player_id;
		controller_ = i->controller;
	}
	update_ui();
}

void connect_side::set_ready_for_start(bool ready_for_start)
{
	ready_for_start_ = ready_for_start;
}

const std::string& connect_side::get_save_id() const
{
	return save_id_;
}

void connect_side::import_network_user(const config& data)
{
	if (controller_ == CNTR_RESERVED)
		set_ready_for_start(true);

	player_id_ = data["name"];
	controller_ = CNTR_NETWORK;

	if(enabled_ && !parent_->era_sides_.empty()) {
		if(ui_->faction_enabled()) {
			faction_ = lexical_cast_default<int>(data["faction"], 0);
			if(faction_ > int(parent_->era_sides_.size()))
				faction_ = 0;
			llm_.update_leader_list(faction_);
			llm_.update_gender_list(llm_.get_leader());
		}
		if(ui_->leader_enabled()) {
			llm_.set_leader(data["leader"]);
			// FIXME: not optimal, but this hack is necessary to do after updating
			// the leader selection. Otherwise, gender gets always forced to "male".
			llm_.update_gender_list(llm_.get_leader());
		}
		if (ui_->gender_enabled()) {
			llm_.set_gender(data["gender"]);
		}
	}
	update_ui();
}

void connect_side::reset(mp::controller controller)
{
	player_id_ = "";
	controller_ = controller;

	if ((controller == mp::CNTR_NETWORK) || (controller == mp::CNTR_RESERVED))
		ready_for_start_ = false;

	if(enabled_ && !parent_->era_sides_.empty()) {
		if(ui_->faction_enabled())
			faction_ = 0;
		if(ui_->leader_enabled())
			llm_.update_leader_list(0);
		if (ui_->gender_enabled())
			llm_.update_gender_list(llm_.get_leader());
	}

	update_ui();
}

void connect_side::resolve_random()
{
	if(!enabled_ || parent_->era_sides_.empty())
		return;

	if(utils::string_bool(
			(*parent_->era_sides_[faction_]).get_attribute("random_faction"),
			false))
	{
		std::vector<std::string> faction_choices, faction_excepts;
		faction_choices = utils::split((*parent_->era_sides_[faction_]).get_attribute("choices"));
		if(faction_choices.size() == 1 && faction_choices.front() == "") {
			faction_choices.clear();
		}
		faction_excepts = utils::split((*parent_->era_sides_[faction_]).get_attribute("except"));
		if(faction_excepts.size() == 1 && faction_excepts.front() == "") {
			faction_excepts.clear();
		}

		// Builds the list of sides eligible for choice (nonrandom factions)
		std::vector<int> nonrandom_sides;
		int num = -1;
		foreach (const config *i, parent_->era_sides_)
		{
			++num;
			if ((*i)["random_faction"] != "yes") {
				const std::string& faction_id = (*i)["id"];
				if (!faction_choices.empty() &&
						std::find(faction_choices.begin(),faction_choices.end(),faction_id) == faction_choices.end())
					continue;
				if(std::find(faction_excepts.begin(),faction_excepts.end(),faction_id) != faction_excepts.end())
					continue;
				nonrandom_sides.push_back(num);
			}
		}

		if (nonrandom_sides.size() == 0) {
			throw config::error(_("Only random sides in the current era."));
		}

		faction_ = nonrandom_sides[rand() % nonrandom_sides.size()];
	}
	bool solved_random_leader = false;

	if (llm_.get_leader() == "random") {
		// Choose a random leader type, and force gender to be random
		llm_.set_gender("random");
		const config& fact = *parent_->era_sides_[faction_];

		std::vector<std::string> types = utils::split(fact["random_leader"]);
		if (types.empty()) {
			// If random_leader= doesn't exists, we use leader=
			types = utils::split(fact["leader"]);
		}

		if (types.empty()) {
			utils::string_map i18n_symbols;
			i18n_symbols["faction"] = fact["name"];
			throw config::error(vgettext("Unable to find a leader type for faction $faction", i18n_symbols));
		}

		const int lchoice = rand() % types.size();
		leader_ = types[lchoice];
		solved_random_leader = true;
	}

	// Resolve random genders "very much" like standard unit code
	if (llm_.get_gender() == "random" || solved_random_leader) {
		const unit_type *ut = unit_types.find(leader_.empty() ? llm_.get_leader() : leader_);
		gender_ = "null";
		if (ut) {
			const std::vector<unit_race::GENDER> glist = ut->genders();
			if (!glist.empty()) {
				const int gchoice = rand() % glist.size();
				// Pick up a gender, using the random 'gchoice' index
				unit_race::GENDER sgender = glist[gchoice];
				if (sgender == unit_race::FEMALE) {
					gender_ = "female";
				} else if (sgender == unit_race::MALE) {
					gender_ = "male";
				}
			}
		} else {
			ERR_CF << "cannot obtain genders for invalid leader '" << (leader_.empty() ? llm_.get_leader() : leader_) << "'.\n";
		}
	}
}

connect::connect(connect_ui* ui, const config& game_config,
		config& gamelist, const mp_game_settings& params,
		mp::controller default_controller, bool local_players_only) :
	game_config_(game_config),
	gamelist_(gamelist),
	local_only_(local_players_only),
	level_(),
	state_(),
	params_(params),
	era_sides_(),
	player_types_(),
	player_factions_(),
	player_teams_(),
	player_colours_(),
	ai_algorithms_(),
	team_names_(),
	user_team_names_(),
	team_prefix_(std::string(_("Team")) + " "),
	sides_(),
	users_(),
	message_full_(false),
	default_controller_(default_controller),
	ui_(ui)
{
	ui_->set_connect(this);
	load_game();

	if(get_result() == ui::QUIT
		|| get_result() == ui::CREATE)
		return;

	if (level_["id"].empty()) {
		throw config::error(_("The scenario is invalid because it has no id."));
	}

	lists_init();
	if(sides_.empty()) {
		throw config::error(_("The scenario is invalid because it has no sides."));
	}

	// Send Initial information
	config response;
	config& create_game = response.add_child("create_game");
	create_game["name"] = params.name;
	if(!params.password.empty()) {
		create_game["password"] = params.password;
	}

	network::send_data(response, 0, true);

	// Adds the current user as default user.
	users_.push_back(connected_user(preferences::login(), CNTR_LOCAL, 0));
	update_user_combos();

	// Take the first available side or available side with id == login
	side_list::iterator s = std::find_if(sides_.begin(), sides_.end(),
			bind(&side::get_current_player, _1) == preferences::login());
	if (s == sides_.end()) {
		s = std::find_if(sides_.begin(), sides_.end(),
				bind(&side::allow_player, _1) && (bind(&side::get_player_id, _1) == ""));
	}
	if (s != sides_.end()) {
		s->set_player_id(preferences::login());
	}

	// Updates the "level_" variable, now that sides are loaded
	update_level();
	update_playerlist_state(true);

	// If we are connected, send data to the connected host
	network::send_data(level_, 0, true);
}

void connect::try_launch() {
	if (can_start_game())
		set_result(mp::ui::PLAY);
}

const game_state& connect::get_state()
{
	return state_;
}

void connect::start_game()
{
	// Resolves the "random faction", "random gender" and "random message"
	foreach(side& s, sides_) {
		s.resolve_random();
	}

	// Make other clients not show the results of resolve_random().
	config lock;
	lock.add_child("stop_updates");
	network::send_data(lock, 0, true);
	update_and_send_diff(true);

	// Build the gamestate object after updating the level
	level_to_gamestate(level_, state_);

	network::send_data(config("start_game"), 0, true);
}

void connect::process_network_data(const config& data, const network::connection sock)
{
	if(data.child("leave_game")) {
		set_result(ui::QUIT);
		return;
	}

	if (!data["side_drop"].empty()) {
		const int side_drop = lexical_cast_default<int>(data["side_drop"], 0) - 1;
		if(side_drop >= 0 && side_drop < int(sides_.size())) {
			connected_user_list::iterator player = find_player(sides_[side_drop].get_player_id());
			sides_[side_drop].reset(sides_[side_drop].get_controller());
			if (player != users_.end()) {
				users_.erase(player);
				update_user_combos();
			}
			update_and_send_diff();
			update_playerlist_state(true);
			return;
		}
	}

	if (!data["side"].empty()) {
		int side_taken = lexical_cast_default<int>(data["side"], 0) - 1;

		// Checks if the connecting user has a valid and unique name.
		const std::string name = data["name"];
		if(name.empty()) {
			config response;
			response["failed"] = "yes";
			network::send_data(response, sock, true);
			ERR_CF << "ERROR: No username provided with the side.\n";
			return;
		}

		connected_user_list::iterator player = find_player(name);
		if(player != users_.end()) {
			/**
			 * @todo Seems like a needless limitation to only allow one side
			 * per player.
			 */
			if(find_player_side(name) != -1) {
				config response;
				response["failed"] = "yes";
				response["message"] = "The nick '" + name + "' is already in use.";
				network::send_data(response, sock, true);
				return;
			} else {
				users_.erase(player);
				config observer_quit;
				observer_quit.add_child("observer_quit")["name"] = name;
				network::send_data(observer_quit, 0, true);
				update_user_combos();
			}
		}

		// Assigns this user to a side
		if(side_taken >= 0 && side_taken < int(sides_.size())) {
			if(!sides_[side_taken].available(name)) {
				// This side is already taken.
				// Try to reassing the player to a different position.
				side_list::const_iterator itor = find_if(sides_.begin(), sides_.end(),
						bind(&side::available, _1, ""));
				side_taken = itor - sides_.begin();

				if(itor == sides_.end()) {
					config response;
					response["failed"] = "yes";
					network::send_data(response, sock, true);

					config kick;
					kick["username"] = data["name"];
					config res;
					res.add_child("kick", kick);
					network::send_data(res, 0, true);

					update_user_combos();
					update_and_send_diff();
					ERR_CF << "ERROR: Couldn't assign a side to '" << name << "'\n";
					return;
				}
			}

			LOG_CF << "client has taken a valid position\n";

			// Adds the name to the list
			users_.push_back(connected_user(name, CNTR_NETWORK, sock));
			update_user_combos();

			sides_[side_taken].import_network_user(data);

			// Go thought and check if more sides are reserved
			// For this player
			std::for_each(sides_.begin(), sides_.end(), bind(&connect::take_reserved_side, this, _1, data));
			update_playerlist_state(false);
			update_and_send_diff();

			LOG_NW << "sent player data\n";

		} else {
			ERR_CF << "tried to take illegal side: " << side_taken << '\n';
			config response;
			response["failed"] = "yes";
			network::send_data(response, sock, true);
		}
	}

	if (const config &change_faction = data.child("change_faction")) {
		int side_taken = find_player_side(change_faction["name"]);
		if(side_taken != -1) {
			sides_[side_taken].import_network_user(change_faction);
			sides_[side_taken].set_ready_for_start(true);
			update_playerlist_state();
			update_and_send_diff();
		}
	}

	if (const config &c = data.child("observer")) {
		const t_string &observer_name = c["name"];
		if(!observer_name.empty()) {
			connected_user_list::iterator player = find_player(observer_name);
			if(player == users_.end()) {
				users_.push_back(connected_user(observer_name, CNTR_NETWORK, sock));
				update_user_combos();
				update_playerlist_state();
				update_and_send_diff();
			}
		}
	}
	if (const config &c = data.child("observer_quit")) {
		const t_string &observer_name = c["name"];
		if(!observer_name.empty()) {
			connected_user_list::iterator player = find_player(observer_name);
			if(player != users_.end() && find_player_side(observer_name) == -1) {
				users_.erase(player);
				update_user_combos();
				update_playerlist_state();
				update_and_send_diff();
			}
		}
	}
}

void connect::take_reserved_side(connect::side& side, const config& data)
{
	if (side.available(data["name"])
			&& side.get_current_player() == data["name"])
	{
		side.import_network_user(data);
	}
}

void connect::process_network_error(network::error& error)
{
	// If the problem isn't related to any specific connection,
	// it's a general error and we should just re-throw the error.
	// Likewise if we are not a server, we cannot afford any connection
	// to go down, so also re-throw the error.
	if(!error.socket || !network::is_server()) {
		error.disconnect();
		throw network::error(error.message);
	}

	// A socket has disconnected. Remove it, and resets its side
	connected_user_list::iterator user = std::find_if(users_.begin(), users_.end(),
			bind(&connected_user::connection, _1) == error.socket);
	error.disconnect();

	if(user != users_.end()) {
		int i = find_player_side(user->name);
		if (i != -1)
			sides_[i].reset(default_controller_);

		users_.erase(user);
		update_user_combos();

		update_and_send_diff();
		update_playerlist_state();
	}
}

bool connect::accept_connections()
{
	return sides_available();
}

void connect::process_network_connection(const network::connection sock)
{
	network::send_data(config("join_game"), 0, true);

	network::send_data(level_, sock, true);
}

void connect::lists_init()
{
	// Options
	if(!local_only_) {
		player_types_.push_back(_("Network Player"));
	}
	player_types_.push_back(_("Local Player"));
	player_types_.push_back(_("Computer Player"));
	player_types_.push_back(_("Empty"));

	foreach (const config *faction, era_sides_) {
		player_factions_.push_back((*faction)["name"]);
	}

	// AI algorithms
	ai_algorithms_ = ai::configuration::get_available_ais();

	// Factions
	config::child_itors sides = current_config()->child_range("side");

	// Teams
	if(params_.use_map_settings) {
		int side_num = 1;
		foreach (config &side, sides)
		{
			t_string &team_name = side["team_name"];
			t_string &user_team_name = side["user_team_name"];

			if(team_name.empty()) {
				team_name = lexical_cast<std::string>(side_num);
			}

			if(user_team_name.empty()) {
				user_team_name = team_name;
			}

			std::vector<std::string>::const_iterator itor = std::find(team_names_.begin(),
															team_names_.end(), team_name);
			if(itor == team_names_.end()) {
				team_names_.push_back(team_name);
				user_team_names_.push_back(user_team_name.to_serialized());
				player_teams_.push_back(user_team_name.str());
			}
			++side_num;
		}
	} else {
		int side_num = 1;
		foreach(const config& side, sides) {
			std::string side_str = lexical_cast<std::string>(side_num);
			team_names_.push_back(side_str);
			user_team_names_.push_back(team_prefix_ + side_str);
			player_teams_.push_back(team_prefix_ + side_str);
			++side_num;
		}
	}

	// Colours
	for(int i = 0; i < gamemap::MAX_PLAYERS; ++i) {
		player_colours_.push_back(get_colour_string(i));
	}

	// Populates "sides_" from the level configuration
	int index = 0;
	foreach(const config &s, sides) {
		side_ui* ui = ui_->next_side_ui();
		sides_.push_back(connect_side(*this, s, index++, ui));
	}

	// This function must be called after the sides_ vector is fully populated.
	ui_->show_sides(sides_);
}

void connect::load_game()
{
	if(params_.saved_game) {
		try{
			savegame::loadgame load(ui_->disp(), game_config_, state_);
			load.load_multiplayer_game();
			load.fill_mplevel_config(level_);
		}
		catch (load_game_cancelled_exception){
			set_result(ui::CREATE);
			return;
		}
	} else {
		level_.clear();
		params_.saved_game = false;
		params_.mp_scenario = params_.scenario_data["id"];
		level_.merge_with(params_.scenario_data);
		level_["turns"] = lexical_cast<std::string>(params_.num_turns);
		level_.add_child("multiplayer", params_.to_config());

		params_.hash = level_.hash();
		level_["next_underlying_unit_id"] = lexical_cast<std::string>(0);
		n_unit::id_manager::instance().clear();

		if (params_.random_start_time)
		{
			if (!tod_manager::is_start_ToD(level_["random_start_time"]))
			{
				level_["random_start_time"] = "yes";
			}
		}
		else
		{
			level_["random_start_time"] = "no";
		}

		level_["experience_modifier"] = lexical_cast<std::string>(params_.xp_modifier);
		level_["random_seed"] = lexical_cast<std::string>(state_.rng().get_random_seed());
	}

	std::string era = params_.mp_era;
	if (params_.saved_game) {
		if (const config &c = level_.child("snapshot").child("era"))
			era = c["id"];
	}

	// Initialize the list of sides available for the current era.
	const config &era_cfg = game_config_.find_child("era", "id", era);
	if (!era_cfg) {
		if (!params_.saved_game)
		{
			utils::string_map i18n_symbols;
			i18n_symbols["era"] = era;
			throw config::error(vgettext("Cannot find era $era", i18n_symbols));
		}
		// FIXME: @todo We should tell user about missing era but still load game
		WRN_CF << "Missing era in MP load game " << era << "\n";
	}
	else
	{
		era_sides_.clear();
		foreach (const config &e, era_cfg.child_range("multiplayer_side")) {
			era_sides_.push_back(&e);
		}
		level_.add_child("era", era_cfg);
	}

	// This will force connecting clients to be using the same version number as us.
	level_["version"] = game_config::version;

	level_["observer"] = params_.allow_observers ? "yes" : "no";

	if(level_["objectives"].empty()) {
		level_["objectives"] = "<big>" + t_string(N_("Victory:"), "wesnoth") +
			"</big>\n<span foreground=\"#00ff00\">" +
			t_string(N_("Defeat enemy leader(s)"), "wesnoth") + "</span>";
	}

	// Add the map name to the title.
	ui_->append_to_title(" - " + level_["name"]);

	ui_->hide_gold_title(params_.saved_game);
	ui_->hide_income_title(params_.saved_game);
}

config* connect::current_config(){
	config* cfg_level = NULL;

	//It might make sense to invent a mechanism of some sort to check whether a config node contains information
	//that you can load from(side information, specifically)
	config &snapshot = level_.child("snapshot");
	if (snapshot && snapshot.child("side")) {
		// Savegame
		cfg_level = &snapshot;
	} else if (!level_.child("side")) {
		// Start-of-scenario save, the info has to be taken from the starting_pos
		cfg_level = &state_.starting_pos;
	} else {
		// Fresh game, no snapshot available
		cfg_level = &level_;
	}

	return cfg_level;
}

void connect::update_level()
{
	// Import all sides into the level
	level_.clear_children("side");
	foreach(const side& s, sides_) {
		level_.add_child("side", s.get_config());
	}
}

void connect::update_and_send_diff(bool update_time_of_day)
{
	config old_level = level_;
	update_level();
	if (update_time_of_day)
	{
		// Set random start ToD
		tod_manager tod_mng(level_,atoi(level_["turns"].c_str()),&state_);
	}

	config diff = level_.get_diff(old_level);
	if (!diff.empty()) {
		config scenario_diff;
		scenario_diff.add_child("scenario_diff", diff);
		network::send_data(scenario_diff, 0, true);
	}
}

bool connect::sides_ready() const
{
	return find_if(sides_.begin(), sides_.end(),
			!bind(&side::ready_for_start, _1))
		== sides_.end();
}

bool connect::sides_available() const
{
	return find_if(sides_.begin(), sides_.end(),
			bind(&side::available, _1, ""))
		!= sides_.end();
}

bool connect::can_start_game() const
{
	if(!sides_ready()) {
		return false;
	}

	return find_if(sides_.begin(), sides_.end(),
			(bind(&side::get_controller, _1) != CNTR_EMPTY) && bind(&side::allow_player, _1))
		!= sides_.end();
}

void connect::update_playerlist_state(bool silent)
{
	ui_->set_waiting_label(can_start_game() ? "" : _("Waiting for players to join..."));
	ui_->enable_launch(can_start_game());

	// If the "gamelist_" variable has users, use it.
	// Else, extracts the user list from the actual player list.
	if (gamelist_.child("user") != NULL) {
		ui_->gamelist_updated(silent);
	} else {
		// Updates the player list
		std::vector<std::string> playerlist;
		foreach(const connected_user& cu, users_) {
			playerlist.push_back(cu.name);
		}
		ui_->set_user_list(playerlist, silent);
		ui_->set_user_menu_items(playerlist);
	}
}

connected_user_list::iterator connect::find_player(const std::string& id)
{
	return std::find_if(users_.begin(), users_.end(),
			bind(&connected_user::name, _1) == id);
}

int connect::find_player_side(const std::string& id) const
{
	side_list::const_iterator itor = std::find_if(sides_.begin(), sides_.end(),
			bind(&side::get_player_id, _1) == id);

	if (itor == sides_.end())
		return -1;

	return itor - sides_.begin();
}

void connect::update_user_combos()
{
	foreach(side& s, sides_) {
		s.update_user_list();
	}
}

} // end namespace mp

