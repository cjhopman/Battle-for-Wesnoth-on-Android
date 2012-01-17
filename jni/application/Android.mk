LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := wesnoth

prefix := .
datarootdir := ${prefix}
datadir := ${datarootdir}
pkgdatadir := $(datadir)
HAS_RELATIVE_LOCALEDIR := 1
LOCALEDIR = translations
fifodir = ${prefix}/var/run/wesnothd

LOCAL_CFLAGS := \
	-I$(LOCAL_PATH)/wesnoth-1.8.5/src \
	-DHAVE_CONFIG \
	-DHAVE_REVISION  \
	-I/home/chris/lib-src/pelya/project/jni/application/../SDL-1.2/include \
	-D_GNU_SOURCE=1 \
	-D_REENTRANT  \
	-DDISABLE_EDITOR \
	-DDISABLE_POOL_ALLOC \
	-DWESNOTH_PATH=\"$(pkgdatadir)\" \
	-DLOCALEDIR=\"$(LOCALEDIR)\" \
	-DHAS_RELATIVE_LOCALEDIR=$(HAS_RELATIVE_LOCALEDIR) \
	-DFIFODIR=\"$(fifodir)\" \
	-DWESNOTH_PREFIX=\"$(prefix)\" \
	$(NULL)


LOCAL_CPP_EXTENSION := .cpp

LIBWESNOTHCORE_SRC_FILES := \
	color_range.cpp \
	config.cpp \
	filesystem.cpp \
	game_config.cpp \
	gettext.cpp \
	hash.cpp \
	log.cpp \
	android_log.cpp \
	map.cpp \
	map_location.cpp \
	md5.cpp \
	thread.cpp \
	tstring.cpp \
	util.cpp \
	version.cpp \
	serialization/binary_or_text.cpp \
	serialization/binary_wml.cpp \
	serialization/parser.cpp \
	serialization/preprocessor.cpp \
	serialization/string_utils.cpp \
	serialization/tokenizer.cpp \
	$(NULL)

LIBWESNOTH_SRC_FILES = \
	builder.cpp \
	cavegen.cpp \
	clipboard.cpp \
	construct_dialog.cpp \
	cursor.cpp \
	display.cpp \
	events.cpp \
	filechooser.cpp \
	font.cpp \
	hotkeys.cpp \
	image.cpp \
	image_function.cpp \
	key.cpp \
	language.cpp \
	loadscreen.cpp \
	map_create.cpp \
	map_label.cpp \
	mapgen.cpp \
	mapgen_dialog.cpp \
	marked-up_text.cpp \
	minimap.cpp \
	pathfind/astarsearch.cpp \
	pathutils.cpp \
	preferences.cpp \
	preferences_display.cpp \
	race.cpp \
	random.cpp \
	reports.cpp \
	sdl_utils.cpp \
	show_dialog.cpp \
	sound.cpp \
	soundsource.cpp \
	sound_music_track.cpp \
	terrain.cpp \
	terrain_translation.cpp \
	video.cpp \
	theme.cpp \
	widgets/button.cpp \
	widgets/file_menu.cpp \
	widgets/label.cpp \
	widgets/menu.cpp \
	widgets/menu_style.cpp \
	widgets/progressbar.cpp \
	widgets/scrollarea.cpp \
	widgets/scrollbar.cpp \
	widgets/slider.cpp \
	widgets/textbox.cpp \
	widgets/widget.cpp \
	wml_exception.cpp  \
	$(NULL)

LOCAL_SRC_FILES := $(addprefix wesnoth-1.8.5/src/, $(LIBWESNOTH_SRC_FILES) $(LIBWESNOTHCORE_SRC_FILES))

LOCAL_LIBRARIES := $(filter-out SDL_main application wesnoth, $(APP_MODULES))

LOCAL_SHARED_LIBRARIES := $(filter-out $(APP_AVAILABLE_STATIC_LIBS), $(LOCAL_LIBRARIES))
LOCAL_STATIC_LIBRARIES := $(filter $(APP_AVAILABLE_STATIC_LIBS), $(LOCAL_LIBRARIES))

LOCAL_LDLIBS := -lz -llog


ifeq (,$(findstring $(LOCAL_MODULE), $(APP_AVAILABLE_STATIC_LIBS)))
include $(BUILD_SHARED_LIBRARY)
else
include $(BUILD_STATIC_LIBRARY)
endif

include $(CLEAR_VARS)

LOCAL_MODULE := application

prefix := .
datarootdir := ${prefix}
datadir := ${datarootdir}
pkgdatadir := $(datadir)
HAS_RELATIVE_LOCALEDIR := 1
LOCALEDIR = translations
fifodir = ${prefix}/var/run/wesnothd

LOCAL_CFLAGS := \
	-I$(LOCAL_PATH)/wesnoth-1.8.5/src \
	-DHAVE_CONFIG \
	-DHAVE_REVISION  \
	-I/home/chris/lib-src/pelya/project/jni/application/../SDL-1.2/include \
	-D_GNU_SOURCE=1 \
	-D_REENTRANT  \
	-DDISABLE_EDITOR \
	-DDISABLE_POOL_ALLOC \
	-DWESNOTH_PATH=\"$(pkgdatadir)\" \
	-DLOCALEDIR=\"$(LOCALEDIR)\" \
	-DHAS_RELATIVE_LOCALEDIR=$(HAS_RELATIVE_LOCALEDIR) \
	-DFIFODIR=\"$(fifodir)\" \
	-DWESNOTH_PREFIX=\"$(prefix)\" \
	-mlong-calls \
	$(NULL)


LOCAL_CPP_EXTENSION := .cpp

WESNOTH_SRC_FILES := \
	about.cpp actions.cpp addon_checks.cpp \
	addon_management.cpp ai/actions.cpp ai/composite/ai.cpp \
	ai/composite/aspect.cpp ai/composite/component.cpp \
	ai/composite/contexts.cpp ai/composite/engine.cpp \
	ai/composite/engine_default.cpp ai/composite/engine_fai.cpp \
	ai/composite/engine_lua.cpp ai/composite/goal.cpp \
	ai/composite/rca.cpp ai/composite/stage.cpp \
	ai/configuration.cpp ai/contexts.cpp ai/default/ai.cpp \
	ai/default/attack.cpp ai/default/contexts.cpp \
	ai/default/move.cpp ai/default/village.cpp ai/formula/ai.cpp \
	ai/formula/callable_objects.cpp ai/formula/candidates.cpp \
	ai/formula/function_table.cpp \
	ai/formula/stage_rca_formulas.cpp \
	ai/formula/stage_side_formulas.cpp \
	ai/formula/stage_unit_formulas.cpp ai/game_info.cpp \
	ai/gamestate_observer.cpp ai/interface.cpp ai/manager.cpp \
	ai/registry.cpp ai/testing/aspect_attacks.cpp \
	ai/testing/ca.cpp ai/testing/ca_default_move_to_targets.cpp \
	ai/testing/ca_testing_move_to_targets.cpp \
	ai/testing/stage_rca.cpp ai/testing/stage_fallback.cpp \
	ai/testing.cpp animated_game.cpp attack_prediction.cpp \
	attack_prediction_display.cpp callable_objects.cpp \
	config_cache.cpp controller_base.cpp dialogs.cpp \
	floating_textbox.cpp formula.cpp formula_debugger.cpp \
	formula_debugger_fwd.cpp formula_function.cpp \
	formula_tokenizer.cpp formula_string_utils.cpp \
	game_display.cpp game_events.cpp game_preferences.cpp \
	game_preferences_display.cpp gamestatus.cpp \
	generate_report.cpp generic_event.cpp gui/auxiliary/canvas.cpp \
	gui/auxiliary/event/dispatcher.cpp \
	gui/auxiliary/event/distributor.cpp \
	gui/auxiliary/event/handler.cpp gui/auxiliary/log.cpp \
	gui/auxiliary/timer.cpp gui/auxiliary/widget_definition.cpp \
	gui/auxiliary/widget_definition/button.cpp \
	gui/auxiliary/widget_definition/horizontal_scrollbar.cpp \
	gui/auxiliary/widget_definition/image.cpp \
	gui/auxiliary/widget_definition/label.cpp \
	gui/auxiliary/widget_definition/listbox.cpp \
	gui/auxiliary/widget_definition/menubar.cpp \
	gui/auxiliary/widget_definition/minimap.cpp \
	gui/auxiliary/widget_definition/multi_page.cpp \
	gui/auxiliary/widget_definition/panel.cpp \
	gui/auxiliary/widget_definition/repeating_button.cpp \
	gui/auxiliary/widget_definition/scroll_label.cpp \
	gui/auxiliary/widget_definition/scrollbar_panel.cpp \
	gui/auxiliary/widget_definition/slider.cpp \
	gui/auxiliary/widget_definition/spacer.cpp \
	gui/auxiliary/widget_definition/stacked_widget.cpp \
	gui/auxiliary/widget_definition/text_box.cpp \
	gui/auxiliary/widget_definition/toggle_button.cpp \
	gui/auxiliary/widget_definition/toggle_panel.cpp \
	gui/auxiliary/widget_definition/tooltip.cpp \
	gui/auxiliary/widget_definition/tree_view.cpp \
	gui/auxiliary/widget_definition/vertical_scrollbar.cpp \
	gui/auxiliary/widget_definition/window.cpp \
	gui/auxiliary/window_builder/button.cpp \
	gui/auxiliary/window_builder/control.cpp \
	gui/auxiliary/window_builder/helper.cpp \
	gui/auxiliary/window_builder/horizontal_listbox.cpp \
	gui/auxiliary/window_builder/horizontal_scrollbar.cpp \
	gui/auxiliary/window_builder/image.cpp \
	gui/auxiliary/window_builder/label.cpp \
	gui/auxiliary/window_builder/listbox.cpp \
	gui/auxiliary/window_builder/minimap.cpp \
	gui/auxiliary/window_builder/menubar.cpp \
	gui/auxiliary/window_builder/multi_page.cpp \
	gui/auxiliary/window_builder/repeating_button.cpp \
	gui/auxiliary/window_builder/scroll_label.cpp \
	gui/auxiliary/window_builder/scrollbar_panel.cpp \
	gui/auxiliary/window_builder/slider.cpp \
	gui/auxiliary/window_builder/spacer.cpp \
	gui/auxiliary/window_builder/stacked_widget.cpp \
	gui/auxiliary/window_builder/text_box.cpp \
	gui/auxiliary/window_builder/toggle_button.cpp \
	gui/auxiliary/window_builder/vertical_scrollbar.cpp \
	gui/auxiliary/window_builder/panel.cpp \
	gui/auxiliary/window_builder/password_box.cpp \
	gui/auxiliary/window_builder/toggle_panel.cpp \
	gui/auxiliary/window_builder/tree_view.cpp \
	gui/auxiliary/window_builder.cpp gui/dialogs/addon_connect.cpp \
	gui/dialogs/addon_list.cpp gui/dialogs/campaign_selection.cpp \
	gui/dialogs/dialog.cpp gui/dialogs/formula_debugger.cpp \
	gui/dialogs/game_load.cpp gui/dialogs/game_delete.cpp \
    gui/dialogs/chat_log.cpp \
	gui/dialogs/game_save.cpp gui/dialogs/gamestate_inspector.cpp \
	gui/dialogs/language_selection.cpp \
	gui/dialogs/preferences.cpp \
	gui/dialogs/touch_game_menu.cpp \
	gui/dialogs/unit_attack.cpp \
	gui/dialogs/unit_advance.cpp \
	gui/dialogs/unit_list.cpp \
	gui/dialogs/unit_recall.cpp \
	gui/dialogs/unit_recruit.cpp \
	gui/dialogs/unit_panel.cpp \
	gui/dialogs/lobby/lobby_data.cpp \
	gui/dialogs/lobby/lobby_info.cpp gui/dialogs/lobby_main.cpp \
	gui/dialogs/lobby_player_info.cpp gui/dialogs/message.cpp \
	gui/dialogs/mp_connect.cpp gui/dialogs/mp_create_game.cpp \
	gui/dialogs/mp_method_selection.cpp \
	gui/dialogs/mp_cmd_wrapper.cpp gui/dialogs/title_screen.cpp \
	gui/dialogs/transient_message.cpp gui/dialogs/unit_create.cpp \
	gui/dialogs/wml_message.cpp gui/widgets/button.cpp \
	gui/widgets/control.cpp gui/widgets/container.cpp \
	gui/widgets/touch_scroll_linker.cpp \
	gui/widgets/debug.cpp gui/widgets/generator.cpp \
	gui/widgets/grid.cpp gui/widgets/helper.cpp \
	gui/widgets/horizontal_scrollbar.cpp gui/widgets/image.cpp \
	gui/widgets/label.cpp gui/widgets/listbox.cpp \
	gui/widgets/menubar.cpp gui/widgets/minimap.cpp \
	gui/widgets/multi_page.cpp gui/widgets/panel.cpp \
	gui/widgets/password_box.cpp gui/widgets/repeating_button.cpp \
	gui/widgets/settings.cpp gui/widgets/scroll_label.cpp \
	gui/widgets/scrollbar.cpp gui/widgets/scrollbar_container.cpp \
	gui/widgets/scrollbar_panel.cpp gui/widgets/slider.cpp \
	gui/widgets/spacer.cpp gui/widgets/stacked_widget.cpp \
	gui/widgets/text.cpp gui/widgets/text_box.cpp \
	gui/widgets/toggle_button.cpp gui/widgets/toggle_panel.cpp \
	gui/widgets/tooltip.cpp gui/widgets/tree_view.cpp \
	gui/widgets/tree_view_node.cpp \
	gui/widgets/vertical_scrollbar.cpp gui/widgets/widget.cpp \
	gui/widgets/window.cpp halo.cpp help.cpp intro.cpp \
	lobby_preferences.cpp leader_list.cpp menu_events.cpp \
	mouse_events.cpp mouse_handler_base.cpp mp_game_settings.cpp \
	multiplayer.cpp multiplayer_ui.cpp multiplayer_wait.cpp \
	multiplayer_connect.cpp multiplayer_create.cpp \
	multiplayer_lobby.cpp network.cpp network_worker.cpp \
	pathfind/pathfind.cpp playcampaign.cpp play_controller.cpp \
	playmp_controller.cpp playsingle_controller.cpp playturn.cpp \
	portrait.cpp replay.cpp replay_controller.cpp resources.cpp \
	save_blocker.cpp savegame.cpp savegame_config.cpp \
	scripting/lua.cpp sha1.cpp settings.cpp statistics.cpp \
	statistics_dialog.cpp storyscreen/interface.cpp \
	storyscreen/controller.cpp storyscreen/part.cpp \
	storyscreen/render.cpp team.cpp terrain_filter.cpp text.cpp \
	time_of_day.cpp titlescreen.cpp tod_manager.cpp tooltips.cpp \
	unit.cpp unit_abilities.cpp unit_animation.cpp \
	unit_display.cpp unit_frame.cpp unit_id.cpp unit_map.cpp \
	unit_types.cpp variable.cpp variant.cpp widgets/combo.cpp \
	widgets/combo_drag.cpp widgets/drop_target.cpp \
	widgets/scrollpane.cpp \
	$(NULL)

LOCAL_SRC_FILES := $(addprefix wesnoth-1.8.5/src/, game.cpp $(LIBWESNOTHCORE_SRC_FILES) $(LIBWESNOTH_SRC_FILES) $(WESNOTH_SRC_FILES))

LOCAL_LIBRARIES := $(filter-out SDL_main application, $(APP_MODULES))

LOCAL_SHARED_LIBRARIES := $(filter-out $(APP_AVAILABLE_STATIC_LIBS), $(LOCAL_LIBRARIES))
LOCAL_STATIC_LIBRARIES := $(filter $(APP_AVAILABLE_STATIC_LIBS), $(LOCAL_LIBRARIES))

LOCAL_LDLIBS := -lz -llog


ifeq (,$(findstring $(LOCAL_MODULE), $(APP_AVAILABLE_STATIC_LIBS)))
include $(BUILD_SHARED_LIBRARY)
else
include $(BUILD_STATIC_LIBRARY)
endif

