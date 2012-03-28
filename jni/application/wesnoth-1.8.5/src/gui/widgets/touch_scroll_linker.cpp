#include "gui/widgets/touch_scroll_linker.hpp"
#include "gui/auxiliary/log.hpp"
#include "gui/auxiliary/timer.hpp"
#include "gui/widgets/window.hpp"

#include "sdl_utils.hpp"
#include "inertia.hpp"


#include <boost/bind.hpp>

#include <algorithm>

namespace gui2 {
	namespace {
		int dragging_threshold = 10;
		int timer_delay = 50;
	}

	ttouch_scroll_linker::ttouch_scroll_linker()
			: panel_(NULL)
			, vert_(NULL)
			, horz_(NULL)
			, mouse_down_(false)
			, dragging_(false)
			, pos_x_(0)
			, pos_y_(0)
			, ticks_(0)
			, dx_(0)
			, dy_(0)
			, enabled_(0)
	{ 
	}

	ttouch_scroll_linker::~ttouch_scroll_linker() {
	}
	
	
	void ttouch_scroll_linker::init(twidget* w, tscrollbar_* v, tscrollbar_* h) 
	{ 
		panel_ = w;
		vert_ = v;
		horz_ = h;
		assert(panel_);
		assert(vert_);
		assert(horz_);

		if (vert_) vert_->disable_events();
		if (horz_) horz_->disable_events();

		//connect signals
		panel_->connect_signal<event::MOUSE_MOTION>(boost::bind(
					&ttouch_scroll_linker::signal_handler_mouse_motion
					, this
					, _2
					, _3
					, _4
					, _5), event::tdispatcher::front_pre_child);
		panel_->connect_signal<event::LEFT_BUTTON_DOWN>(boost::bind(
					&ttouch_scroll_linker::signal_handler_left_button_down, this, _2, _3, _4), event::tdispatcher::front_pre_child);
		panel_->connect_signal<event::LEFT_BUTTON_UP>(boost::bind(
					&ttouch_scroll_linker::signal_handler_left_button_up, this, _2, _3, _4), event::tdispatcher::front_pre_child);
		panel_->connect_signal<event::LEFT_BUTTON_CLICK>(boost::bind(
					&ttouch_scroll_linker::signal_handler_left_button_click, this, _2, _3, _4), event::tdispatcher::front_pre_child);
		panel_->connect_signal<event::MOUSE_MOTION>(boost::bind(
					&ttouch_scroll_linker::signal_handler_mouse_motion
					, this
					, _2
					, _3
					, _4
					, _5), event::tdispatcher::front_child);
		panel_->connect_signal<event::LEFT_BUTTON_DOWN>(boost::bind(
					&ttouch_scroll_linker::signal_handler_left_button_down, this, _2, _3, _4), event::tdispatcher::front_child);
		panel_->connect_signal<event::LEFT_BUTTON_UP>(boost::bind(
					&ttouch_scroll_linker::signal_handler_left_button_up, this, _2, _3, _4), event::tdispatcher::front_child);
	}

	void ttouch_scroll_linker::signal_handler_mouse_motion(
			const event::tevent /*event*/
			, bool& handled
			, bool& halt
			, const tpoint& coordinate) {

		if (!mouse_down_) return;

		tpoint mouse = coordinate;

		if (dragging_ || abs(mouse.x - x_) > dragging_threshold || abs(mouse.y - y_) > dragging_threshold)
			handled = halt = dragging_ = true;
		else return;

		int dx = x_ - mouse.x, dy = y_ - mouse.y;

		scroll(dx, dy);
	}

	void ttouch_scroll_linker::scroll(int dx, int dy) {
		int sx = horz_ ? horz_->get_step_size() : 1;
		int sy = vert_ ? vert_->get_step_size() : 1;

		float ppx = horz_ ? horz_->pixels_per_step_ : 1.0;
		float ppy = vert_ ? vert_->pixels_per_step_ : 1.0;

		int npos_x = ((dx * ppx) / sx) + pos_x_;
		int npos_y = ((dy * ppy) / sy) + pos_y_;

		int dpx = horz_ ? npos_x - horz_->positioner_offset_ : 0;
		int dpy = vert_ ? npos_y - vert_->positioner_offset_ : 0;

		if (horz_) {
			horz_->move_positioner(dpx);
		}
		if (vert_) {
			vert_->move_positioner(dpy);
		}

		if (horz_ && static_cast<unsigned>(npos_x) != horz_->positioner_offset_) {
			x_ -= dx;

				dx_ = 0;

			pos_x_ = horz_->positioner_offset_;
		}

		if (vert_ && static_cast<unsigned>(npos_y) != vert_->positioner_offset_) {
			y_ -= dy;

				dy_ = 0;

			pos_y_ = vert_->positioner_offset_;
		}
	}

	void ttouch_scroll_linker::signal_handler_left_button_down(
			const event::tevent /*event*/, bool& /*handled*/, bool& /*halt*/) {
		if (!enabled_) return;
		if (horz_->all_items_visible() && vert_->all_items_visible()) return;

		tpoint mouse = get_mouse_position();

		SDL_Rect bounds = panel_->get_rect();
		if (!point_in_rect(mouse.x, mouse.y, bounds)) return;

		px_ = x_ = mouse.x;
		py_ = y_ = mouse.y;
		pos_x_ = horz_ ? horz_->positioner_offset_ : 0;
		pos_y_ = vert_ ? vert_->positioner_offset_ : 0;

		dragging_ = false;
		mouse_down_ = true;
	}

	void ttouch_scroll_linker::signal_handler_left_button_up(
			const event::tevent /*event*/, bool& /*handled*/, bool& /*halt*/) {
		mouse_down_ = false;
	}

	void ttouch_scroll_linker::signal_handler_left_button_click(
			const event::tevent /*event*/, bool& handled, bool& halt) {
		handled = halt = dragging_;
	}

	void ttouch_scroll_linker::step() {
		unsigned long dt = SDL_GetTicks() - ticks_;
		if (dt < 30) return;
		ticks_ += dt;
		
		if (mouse_down_) {
			tpoint mouse = get_mouse_position();
			dx_ = static_cast<float>(px_ - mouse.x) / dt;
			dy_ = static_cast<float>(py_ - mouse.y) / dt;
			px_ = mouse.x;
			py_ = mouse.y;
			return;
		}

		if (!dragging_ || (!dx_ && !dy_)) return;

		float sx, sy;

		boost::tie(dx_, sx) = inertia::decay(dx_, dt);
		boost::tie(dy_, sy) = inertia::decay(dy_, dt);

		px_ -= sx * dt;
		py_ -= sy * dt;

		scroll(x_ - px_, y_ - py_);
		panel_->set_dirty();
	}
}
