package com.androthsoft.battlefree;

import android.content.Context;
import android.util.AttributeSet;
import android.view.KeyEvent;
import android.widget.AbsoluteLayout;

public class KeyboardLayout extends AbsoluteLayout {
	private static final String TAG = "KeyboardLayout";

	private MainActivity parent_;

	public KeyboardLayout(Context context, AttributeSet attrs) {
		super(context, attrs);
		parent_ = (MainActivity)context;
	}

	public KeyboardLayout(Context context) {
		super(context);
		parent_ = (MainActivity)context;
	}

	@Override
		public boolean dispatchKeyEventPreIme(KeyEvent event) {
			if (parent_ != null && event.getKeyCode() == KeyEvent.KEYCODE_BACK) {
				KeyEvent.DispatcherState state = getKeyDispatcherState();
				if (state != null) {
					if (event.getAction() == KeyEvent.ACTION_DOWN &&
							event.getRepeatCount() == 0) {
						state.startTracking(event, this);
						return true;
					} else if (event.getAction() == KeyEvent.ACTION_UP &&
							!event.isCanceled() && state.isTracking(event)) {
						parent_.hideScreenKeyboard();
						return true;
					}
				}
			}
			return super.dispatchKeyEventPreIme(event);
		}
}

