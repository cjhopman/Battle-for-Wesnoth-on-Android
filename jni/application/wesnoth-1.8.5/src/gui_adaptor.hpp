/*
   Copyright (C) 2012
   Part of the Battle for Wesnoth Project http://www.wesnoth.org

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License version 2
   or at your option any later version.
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY.

   See the COPYING file for more details.
*/

#ifndef GUI_ADAPTOR_HPP_INCLUDED
#define GUI_ADAPTOR_HPP_INCLUDED

#include "widgets/combo.hpp"
#include "widgets/combo_drag.hpp"
#include "widgets/label.hpp"
#include "widgets/slider.hpp"

#include "gui/widgets/button.hpp"
#include "gui/widgets/drop_button.hpp"
#include "gui/widgets/label.hpp"
#include "gui/widgets/slider.hpp"

#include <cassert>
#include <memory>
#include <string>

#include <iostream>

namespace gui_adaptor {

template <typename T, typename U>
struct adaptor_type { };

#define ADAPTOR(T, U) typename adaptor_type<T, U>::type
#define REGISTER_ADAPTOR(T, U, TP) template <> struct adaptor_type<T, U> { typedef TP type; };

// ----- WIDGET
class widget_base {
public:
	virtual widget_base* clone() = 0;

	virtual void enable(bool) { assert(0); }
	virtual void hide(bool) { assert(0); }
	virtual bool enabled() const { assert(0); }

	virtual ~widget_base() { };
};

template <typename T>
class mock_widget : public T {
public:
	mock_widget* clone() { return new mock_widget(*this); }

	void enable(bool e) { e_ = e; }
	void hide(bool) { }
	bool enabled() const { return e_; }
private:
	bool e_;
};

class empty { };
template <typename T, typename W>
class gui1_widget : public T {
public:
	gui1_widget(W* w) : w_(w) { }
	gui1_widget* clone() { return new gui1_widget(*this); }

	void enable(bool e) { w_->enable(e); }
	void hide(bool h) { w_->hide(h); }
	bool enabled() const { return w_->enabled(); }
protected:
	W* w_;
};
template <typename T, typename W>
class gui2_widget : public T {
public:
	gui2_widget(W* w) : w_(w) { }
	gui2_widget* clone() { return new gui2_widget(*this); }

	void enable(bool e) { w_->set_active(e); }
	void hide(bool h) { w_->set_visible(h ? W::VISIBLE : W::HIDDEN); }
	bool enabled() const { return w_->get_active(); }
protected:
	W* w_;
};

template <typename T>
class widget {
public:
	widget(T* t = 0) : impl_(t ? t : new ADAPTOR(T, void)()) { }
	template <typename U>
	widget(U* u) : impl_(u ? static_cast<T*>(new ADAPTOR(T, U*)(u)) : new ADAPTOR(T, void)()) { }
	template <typename U, typename A1>
	widget(U* u, A1 a1) : impl_(u ? static_cast<T*>(new ADAPTOR(T, U*)(u, a1)) : new ADAPTOR(T, void)()) { }
	widget(const widget<T>& o) : impl_(o.impl_->clone()) { }

	template <typename U>
	void set_widget(U* u) {
		impl_.reset(u ? static_cast<T*>(new ADAPTOR(T, U*)(u)) : new ADAPTOR(T, void)());
	}
	template <typename U, typename A1>
	void set_widget(U* u, A1 a1) {
		impl_.reset(u ? static_cast<T*>(new ADAPTOR(T, U*)(u, a1)) : new ADAPTOR(T, void)());
	}

	void enable(bool e) { impl_->enable(e); }
	void hide(bool h) { impl_->hide(h); }
	bool enabled() const { return impl_->enabled(); }
protected:
	std::auto_ptr<T> impl_;
};

typedef widget<widget_base> generic_widget;
REGISTER_ADAPTOR(widget_base, void, mock_widget<widget_base>);
typedef gui1_widget<empty, gui::widget> gui1_gen_w;
REGISTER_ADAPTOR(widget_base, gui::widget*, gui1_gen_w);
typedef gui2_widget<empty, gui2::twidget> gui2_gen_w;
REGISTER_ADAPTOR(widget_base, gui2::tcontrol*, gui2_gen_w);

// ----- VALUE
template <typename T>
class value_base : public widget_base {
public:
	virtual T get() const { assert(0); }
	virtual void set(T) { assert(0); }
	virtual value_base* clone() = 0;
};

template <typename T>
class mock_value : public mock_widget<value_base<T> > {
public:
	void set(T t) { v_ = t; }
	T get() const { return v_; }
	mock_value* clone() { return new mock_value(*this); }
private:
	T v_;
};

template <typename T>
class value_impl : public widget<value_base<T> > {
	typedef widget<value_base<T> > super;
public:
	void set(T t) { super::impl_->set(t); }
	T get() const { return super::impl_->get(); }
	void set_value(const T& t) { set(t); }
};

template <typename T>
class value_mixin : public value_impl<T> { };

template <typename T>
class value : public value_mixin<T> { 
public:
	value() { }
	template <typename U>
	value(U* u) { set_widget(u); }
};

// ----- VALUE<STRING>

template <>
class value_mixin<std::string> : public value_impl<std::string> {
public:
	void set_text(std::string s) { set(s); }
	void set_label(std::string s) { set(s); }
};

class gui1_label : public gui1_widget<value_base<std::string>, gui::label> {
	typedef gui1_widget<value_base<std::string>, gui::label> super;
public:
	gui1_label(gui::label* w) : super(w) { }
	void set(std::string s) { w_->set_text(s); }
	std::string get() const { return w_->get_text(); }
	gui1_label* clone() { return new gui1_label(*this); }
};

class gui1_button : public gui1_widget<value_base<std::string>, gui::button> {
	typedef gui1_widget<value_base<std::string>, gui::button> super;
public:
	gui1_button(gui::button* w) : super(w) { }
	void set(std::string s) { w_->set_label(s); }
	std::string get() const { return w_->label(); }
	gui1_button* clone() { return new gui1_button(*this); }
};

REGISTER_ADAPTOR(value_base<std::string>, void, mock_value<std::string>);
REGISTER_ADAPTOR(value_base<std::string>, gui::label*, gui1_label);
REGISTER_ADAPTOR(value_base<std::string>, gui::button*, gui1_button);

class gui2_button : public gui2_widget<value_base<std::string>, gui2::tbutton> {
	typedef gui2_widget<value_base<std::string>, gui2::tbutton> super;
public:
	gui2_button(gui2::tbutton* w) : super(w) { }
	void set(std::string s) { w_->set_label(s); }
	std::string get() const { return w_->label(); }
	gui2_button* clone() { return new gui2_button(*this); }
};

class gui2_label : public gui2_widget<value_base<std::string>, gui2::tlabel> {
	typedef gui2_widget<value_base<std::string>, gui2::tlabel> super;
public:
	gui2_label(gui2::tlabel* w) : super(w) { }
	void set(std::string s) { w_->set_label(s); }
	std::string get() const { return w_->label(); }
	gui2_label* clone() { return new gui2_label(*this); }
};

REGISTER_ADAPTOR(value_base<std::string>, gui2::tbutton*, gui2_button);
REGISTER_ADAPTOR(value_base<std::string>, gui2::tlabel*, gui2_label);

// ----- VALUE<INT>

class gui1_slider : public gui1_widget<value_base<int>, gui::slider> {
	typedef gui1_widget<value_base<int>, gui::slider> super;
public:
	gui1_slider(gui::slider* w) : super(w) { }
	void set(int v) { w_->set_value(v); }
	int get() const { return w_->value(); }
	gui1_slider* clone() { return new gui1_slider(*this); }
};

REGISTER_ADAPTOR(value_base<int>, void, mock_value<int>);
REGISTER_ADAPTOR(value_base<int>, gui::slider*, gui1_slider);

class gui2_slider : public gui2_widget<value_base<int>, gui2::tslider> {
	typedef gui2_widget<value_base<int>, gui2::tslider> super;
public:
	gui2_slider(gui2::tslider* w) : super(w) { }
	void set(int v) { w_->set_value(v); }
	int get() const { return w_->get_value(); }
	gui2_slider* clone() { return new gui2_slider(*this); }
};

REGISTER_ADAPTOR(value_base<int>, gui2::tslider*, gui2_slider);

// ----- LIST
class list_base : public widget_base {
public:
	virtual int selected() const { return -1; }
	virtual void set_selected(int) { }
	virtual void set_items(const std::vector<std::string>&) { }
	virtual size_t items_size() const { return 0; }

	virtual list_base* clone() = 0;
};

class mock_list : public mock_widget<list_base> {
public:
	int selected() const { return selected_; }
	void set_selected(int v) { selected_ = v; }
	void set_items(const std::vector<std::string>& l) { vals_ = l; }
	size_t items_size() const { return vals_.size(); }

	mock_list* clone() { return new mock_list(*this); }
private:
	int selected_;
	std::vector<std::string> vals_;
};

class gui1_combo : public gui1_widget<list_base, gui::combo> {
	typedef gui1_widget<list_base, gui::combo> super;
public:
	int selected() const { return w_->selected(); }
	void set_selected(int v) { w_->set_selected(v); }
	void set_items(const std::vector<std::string>& l) { w_->set_items(l); }
	size_t items_size() const { return w_->items_size(); }

	gui1_combo(gui::combo* w) : super(w) { }
	gui1_combo* clone() { return new gui1_combo(*this); }
};

REGISTER_ADAPTOR(list_base, void, mock_list);
REGISTER_ADAPTOR(list_base, gui::combo*, gui1_combo);
REGISTER_ADAPTOR(list_base, gui::combo_drag*, gui1_combo);

class gui2_combo : public gui2_widget<list_base, gui2::tdrop_button> {
	typedef gui2_widget<list_base, gui2::tdrop_button> super;
public:
	int selected() const { return w_->selected(); }
	void set_selected(int v) {
		w_->set_selected(v);
	}
	void set_items(const std::vector<std::string>& l) { 
		w_->set_items(l);
	}
	size_t items_size() const { return w_->get_items().size(); }

	gui2_combo(gui2::tdrop_button* w) : super(w) { }
	gui2_combo* clone() { return new gui2_combo(*this); }
};
REGISTER_ADAPTOR(list_base, gui2::tdrop_button*, gui2_combo);

class list : public widget<list_base> {
	typedef widget<list_base> super;
public:
	int selected() const { return impl_->selected(); }
	void set_selected(int v) { impl_->set_selected(v); }
	void set_items(const std::vector<std::string>& l) { impl_->set_items(l); }
	size_t items_size() const { return impl_->items_size(); }

	list() : super() { }
	template <typename T>
	list(T* t = 0) : super(t) { }
	template <typename T, typename A1>
	list(T* t, A1 a1) : super(t, a1) { }

	list(const list& o) : super(o) { }
};

} // namespace gui_adaptor

#endif
