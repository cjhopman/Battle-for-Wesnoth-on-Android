/* $Id: helper.hpp 48450 2011-02-08 20:55:18Z mordante $ */
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

#ifndef GUI_DIALOGS_HELPER_HPP_INCLUDED
#define GUI_DIALOGS_HELPER_HPP_INCLUDED

#include "gui/dialogs/field.hpp"
#include "gui/widgets/listbox.hpp"
#include "gui/widgets/text_box.hpp"
#include "gui/widgets/widget.hpp"
#include "gui/widgets/window.hpp"

#include <string>

namespace gui2 {

/**
 * Template for dialog callbacks. Example usage:
 * widget->set_callback(dialog_callback<my_dialog_class, &my_dialog_class::member_function>);
 */
template <class D, void (D::*fptr)(twindow&)>
void dialog_callback(twidget* caller)
{
	D* dialog = dynamic_cast<D*>(caller->dialog());
	assert(dialog);
	twindow* window = dynamic_cast<twindow*>(caller->get_window());
	assert(window);
	(dialog->*fptr)(*window);
}

/**
 * Macro (ugh) for setting standardised dialog callbacks, a button with id "foo"
 * will have it's callback registered to MY_TYPE::foo_button_callback. Designed
 * to be used within a dialog's pre_show() (requires a twindow& window in the
 * current scope).
 */
#define GUI2_EASY_BUTTON_CALLBACK(ID, MY_TYPE) \
	tbutton* ID##_btn = dynamic_cast<tbutton*>(window.find(#ID, false)); \
	VALIDATE(ID##_btn, missing_widget(#ID)); \
	ID##_btn->set_callback_mouse_left_click(dialog_callback<MY_TYPE, \
		&MY_TYPE::ID##_button_callback>);
#define GUI2_EASY_BUTTON_CALLBACK_MAYBE(ID, MY_TYPE) \
	if (tbutton* ID##_btn = dynamic_cast<tbutton*>(window.find(#ID, false))) { \
		ID##_btn->set_callback_mouse_left_click(dialog_callback<MY_TYPE, \
			&MY_TYPE::ID##_button_callback>); \
	}

inline std::string ellipsize(std::string str, unsigned len) {
	if (str.size() > len) {
		str[len - 3] = str[len - 2] = str[len - 1] = '.';
		str.resize(len);
	}
	std::string result;
	std::swap(result, str);
	return result;
}
template <typename T>
struct get_value;
template <>
struct get_value<tlistbox> {
	typedef int type;
	static type get(tlistbox* w) {
		return w->get_selected_row();
	}
};
template<>
struct get_value<ttext_box> {
	typedef std::string type;
	static type get(ttext_box* w) {
		return w->get_value();
	}
};
template <typename T>
typename get_value<T>::type get_value_or_default(twindow* const wnd, std::string id, typename get_value<T>::type val) {
	typename get_value<T>::type result;
	using std::swap;
	if (T* w = find_widget<T>(wnd, id, false, false)) {
		val = get_value<T>::get(w);
	}
	swap(result, val);
	return result;
}
template <>
struct get_value<tfield_bool> {
	typedef bool type;
};
template <>
struct get_value<tfield_integer> {
	typedef int type;
};
template <typename T>
typename get_value<T>::type get_value_or_default(T* const field, typename get_value<T>::type val) {
	typename get_value<T>::type result;
	using std::swap;
	if (field->widget()) {
		val = field->get_value();
	}
	swap(result, val);
	return result;
}

inline void set_visible_if_exists(twidget* widget, bool visible) {
	if (widget) {
		widget->set_visible(visible ? twidget::VISIBLE : twidget::INVISIBLE);
	}
}

inline void set_visible_if_exists(twindow* window, const std::string& id, bool visible) {
	set_visible_if_exists(find_widget<twidget>(window, id, false, false), visible);
}

inline void set_visible_if_exists(tgrid* grid, const char* id, bool visible)
{
	twidget* w = grid->find(id, false);
	if (w) {
		w->set_visible(visible ? twidget::VISIBLE : twidget::INVISIBLE);
	}
}

} // namespace gui2

#endif

