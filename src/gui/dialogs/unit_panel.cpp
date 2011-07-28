#include "gui/widgets/grid.hpp"
#include "gui/widgets/control.hpp"
#include "gui/widgets/widget.hpp"
#include "gui/widgets/image.hpp"
#include "gui/widgets/label.hpp"
#include "gui/widgets/listbox.hpp"

#include "gui/auxiliary/log.hpp"

#include "unit.hpp"
#include "team.hpp"

#include "marked-up_text.hpp"

#include <sstream>
#include <string>
#include <memory>
#include <map>

namespace gui2 {

namespace {
		static void set_use_markup(
				tgrid* grid 
				, const std::string& id
				, const bool use_markup
				)
		{
			tcontrol* widget = find_widget<tcontrol>(grid, id, false, false);
			if(widget) {
				widget->set_use_markup(use_markup);
			}
		}

		static void set_label(
				tgrid* grid 
				, const std::string& id
				, const std::string& label
				)
		{
			tcontrol* widget = find_widget<tcontrol>(grid, id, false, false);
			if(widget) {
				widget->set_label(label);
			} else {
				timage* image= find_widget<timage>(grid, id, false, false);
				if (image) {
					image->set_label(label);
				}
			}
		}
}

void markup_unit_moves(std::map<std::string, string_map>& data, const unit& u) {
	SDL_Color move_color = {255, 255, 0, 0};

	if(u.movement_left() == 0) {
		move_color.g = 0;
	} else if(u.movement_left() == u.total_movement() ) {
		move_color.r = 0;
	}

	std::stringstream ss;
	ss << "<span foreground=\"#" << font::color2hexa(move_color) << "\">";
	ss << u.movement_left() << "/" << u.total_movement();
	ss << "</span>";

	data["moves"]["label"] = ss.str();
	data["moves"]["use_markup"] = "true";
}

std::map<std::string, string_map> get_unit_info(const unit_type* type) {
	std::map<std::string, string_map> info;

	std::stringstream ss;

	info["type"]["label"] = type->type_name();
	info["alignment"]["label"] = unit_type::alignment_id(type->alignment());

	ss.str("");
	int pos = 0;
	foreach(const std::string& str, type->abilities())
		ss << (pos++ ? ", " : "") << str;
	info["abilities"]["label"] = ss.str();

	ss.str("");
	ss << "Moves: " << type->movement() << "/" << type->movement();
	info["moves"]["label"] = ss.str();

	ss.str("");
	SDL_Color hp_color = {33, 225, 0, 0};
	ss << "<span foreground=\"#" << font::color2hexa(hp_color) << "\">" << type->hitpoints() << "/" << type->hitpoints() << "</span>";
	info["health_hp"]["label"] = ss.str();
	info["health_hp"]["use_markup"] = "true";

	ss.str("");
	SDL_Color xp_color = {0, 160, 255, 0};
	ss << "<span foreground=\"#" << font::color2hexa(xp_color) << "\">" << 0 << "/" << type->experience_needed() << "</span>";
	info["health_xp"]["label"] = ss.str();
	info["health_xp"]["use_markup"] = "true";

	ss.str("");
	ss << type->cost() << " Gold";
	info["cost"]["label"] = ss.str();

	int r = 0;
	foreach (const attack_type& a, type->attacks()) {
		std::stringstream id;

		ss.str("");
		ss << a.damage() << "-" << a.num_attacks() << " " << a.name();
		id << "weapon_" << r << "_stat";
		info[id.str()]["label"] = ss.str();

		id.str("");
		ss.str("");
		ss << " " << a.range() << " - " << a.type();
		id << "weapon_" << r << "_type";
		info[id.str()]["label"] = ss.str();

		id.str("");
		ss.str("");
		id << "weapon_" << r << "_special";
		if (!a.weapon_specials(true).empty()) {
			ss << " " << a.weapon_specials(true);
		}

		info[id.str()]["label"] = ss.str();

		++r;
	}
	return info;
}

std::map<std::string, string_map> get_unit_info(const unit& u) {
	std::map<std::string, string_map> info = get_unit_info(u.type());
	std::stringstream ss;

	ss << u.absolute_image() << "~RC(" << u.team_color() << ">" << team::get_side_colour_index(u.side()) << ")";
	info["portrait"]["label"] = ss.str();

	ss.str("");
	ss << u.level();
	info["level"]["label"] = ss.str();

	info["name"]["label"] = u.name();
	info["traits"]["label"] = u.traits_description();

	ss.str("");
	ss << "Moves: " << u.movement_left() << "/" << u.total_movement();
	info["moves"]["label"] = ss.str();

	ss.str("");
	ss << "<span foreground=\"#" << font::color2hexa(u.hp_color()) << "\">" << u.hitpoints() << "/" << u.max_hitpoints() << "</span>";
	info["health_hp"]["label"] = ss.str();
	info["health_hp"]["use_markup"] = "true";

	ss.str("");
	ss << "<span foreground=\"#" << font::color2hexa(u.xp_color()) << "\">" << u.experience() << "/" << u.max_experience() << "</span>";
	info["health_xp"]["label"] = ss.str();
	info["health_xp"]["use_markup"] = "true";

	
	return info;
}

std::map<std::string, string_map> get_unit_info(const unit_type* type, const team* team) {
	std::map<std::string, string_map> info = get_unit_info(type);

	std::stringstream ss;
	ss << type->image() << "~RC(" << type->flag_rgb() << ">" << team::get_side_colour_index(team->side()) << ")";
	info["portrait"]["label"] = ss.str();
	return info;
}

void set_unit_info(tgrid* g, const std::map<std::string, string_map>& data) {
	typedef std::map<std::string, string_map> info_map;
	foreach(const info_map::value_type& val, data) {
		if (val.second.count("label")) {
			set_label(g, val.first, val.second.find("label")->second);
		}
		if (val.second.count("use_markup")) {
			set_use_markup(g, val.first, val.second.find("use_markup")->second == "true");
		}
	}
}

void set_unit_info(tgrid* g, const unit_type* type, const team* team) {
	std::map<std::string, string_map> info = get_unit_info(type, team);
	set_unit_info(g, info);
}

void set_unit_info(tgrid* g, const unit& u) {
	std::map<std::string, string_map> info = get_unit_info(u);
	set_unit_info(g, info);
}
}
