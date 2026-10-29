#pragma once

namespace vars
{
	inline auto b_is_running = true;

	inline std::atomic<bool> b_menu_open { true };
	inline std::atomic<bool> b_menu_key_down { false };

	namespace key
	{
		inline auto clicker_enabled = keybind( true, config.clicker.i_key_type, config.clicker.i_clicker_key );
		inline auto hide_window = keybind( false, keybind_state_t::toggle, config.clicker.i_hide_window_key );

		inline auto left_activation = keybind( false, config.clicker.i_left_mode, config.clicker.i_left_key );
		inline auto right_activation = keybind( false, config.clicker.i_right_mode, config.clicker.i_right_key );

		inline auto is_left_down = keybind( false, keybind_state_t::hold, VK_LBUTTON );
		inline auto is_right_down = keybind( false, keybind_state_t::hold, VK_RBUTTON );

		inline std::atomic<bool> b_master_active { false };
		inline std::atomic<bool> b_hide_forced { false };

		inline std::atomic<bool> b_game_focused { false };
		inline std::atomic<bool> b_cursor_visible { false };

		inline std::atomic<bool> b_chat_open { false };
		inline std::atomic<bool> b_binding_key { false };
		inline std::atomic<std::uint64_t> i_last_typing_ms { 0 };

		inline auto b_inventory_opened = false;
		inline auto b_shift_pressed = false;
	}

	namespace stats
	{
		inline auto f_average_cps = 0.f;
		inline std::atomic<int> i_clicks_this_session { 0 };

		inline std::atomic<bool> b_left_active { false };
		inline std::atomic<bool> b_right_active { false };
	}
}
