#pragma once

#include "archivex.hpp"

#include <shlobj.h>
#include <fstream>
#include <filesystem>

class c_config
{
private:
	std::filesystem::path path;
	std::vector<std::string> configs;

public:
	void run( std::string name );
	void load( size_t id );
	void save( size_t id ) const;
	void add( std::string name );
	void remove( size_t id );
	void rename( size_t item, std::string new_name );
	void reset();

	constexpr auto& get_configs() { return configs; };
	constexpr auto& get_path() { return path; };

	struct
	{
		// menu / interface
		int i_menu_key { 0x4D };				// 'M'
		int i_menu_mode { 0 };					// 0 = alternar, 1 = segurar

		bool b_menu_only_in_game { true };
		bool b_menu_block_gui { true };
		bool b_menu_block_typing { true };
		int i_menu_debounce_ms { 300 };
		bool b_pause_when_menu_open { true };

		// chave mestra
		int i_clicker_key { 0 };
		int i_key_type { keybind_state_t::always };
		int i_version_type { 0 };

		// clicker esquerdo
		bool b_enable_left_clicker { true };
		int i_left_key { VK_LBUTTON };
		int i_left_mode { keybind_state_t::hold };
		float f_left_cps { rng::random_real<float>( 9.5f, 12.5f ) };

		// clicker direito
		bool b_enable_right_clicker { true };
		int i_right_key { VK_RBUTTON };
		int i_right_mode { keybind_state_t::hold };
		float f_right_cps { rng::random_real<float>( 9.5f, 12.5f ) };

		bool b_left_skip_while_right { true };

		bool b_enable_blatant { false };

		int i_hide_window_key { 0 };

		bool b_enable_advanced_options { false };

		bool b_enable_persistence { false };
		float f_persistence_value { rng::random_real<float>( 1.f, 5.f ) };

		float f_persistence_update_rate { 3000.f };
		float f_default_timer_randomization { 5.f };

		int i_cps_spike_chance { 5 };
		float f_cps_spike_add { 2.5f };

		int i_cps_drop_chance { 15 };
		float f_cps_drop_remove { 3.5f };

		bool b_enable_cps_spikes { false };
		bool b_enable_cps_drops { false };

		bool b_work_in_inventory { false };

		float f_color_accent[4] { 0.41f, 0.41f, 0.86f, 1.00f };
		float f_color_accent_hovered[4] { 0.53f, 0.53f, 0.91f, 1.00f };
		float f_color_accent_active[4] { 0.40f, 0.39f, 0.90f, 1.00f };
		float f_color_accent_text[4] { 0.94f, 0.94f, 0.94f, 1.00f };

		char str_window_title[64] {};
	} clicker;
};

inline auto config = c_config();
