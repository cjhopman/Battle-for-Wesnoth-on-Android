#ifndef GUI_WIDGETS_TOUCH_SCROLL_LINKER_HPP_INCLUDED
#define GUI_WIDGETS_TOUCH_SCROLL_LINKER_HPP_INCLUDED

#include "gui/widgets/scrollbar.hpp"
#include "gui/widgets/widget.hpp"

#include "SDL_types.h"

namespace gui2 {

	class ttouch_scroll_linker {
		public:
		ttouch_scroll_linker();
		~ttouch_scroll_linker();

		void init(twidget* w, tscrollbar_* v, tscrollbar_* h);

		void signal_handler_mouse_motion(
				const event::tevent event
				, bool& handled
				, bool& halt
				, const tpoint& coordinate);

		void signal_handler_left_button_down(
				const event::tevent event, bool& handled, bool& halt);

		void signal_handler_left_button_up(
				const event::tevent event, bool& handled, bool& halt);

		void signal_handler_left_button_click(
				const event::tevent event, bool& handled, bool& halt);

		void signal_handler_draw(
				const event::tevent event, bool& handled, bool& halt);

		void step();

		void scroll(int dx, int dy);

		void set_enabled(bool en) { enabled_ = en; }

		private:
		twidget* panel_;
		tscrollbar_ *vert_, *horz_;

		bool mouse_down_, dragging_;
		int x_, y_;
		int px_, py_;
		unsigned pos_x_, pos_y_;

		Uint32 ticks_;
		float dx_, dy_;

		bool enabled_;
	};

} // namespace gui2

#endif
