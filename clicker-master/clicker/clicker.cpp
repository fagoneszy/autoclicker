#include "pch.hpp"
#include "clicker.hpp"
#include "menu.hpp"

namespace
{
	std::uint64_t now_ms()
	{
		return GetTickCount64();
	}

	bool is_typing_key( int i_key )
	{
		if ( i_key >= 0x30 && i_key <= 0x39 )
			return true;

		if ( i_key >= 0x41 && i_key <= 0x5A )
			return true;

		switch ( i_key )
		{
			case VK_RETURN:
			case VK_SPACE:
			case VK_BACK:
			case VK_TAB:
			case VK_DELETE:
			case VK_INSERT:
			case VK_OEM_1:
			case VK_OEM_PLUS:
			case VK_OEM_COMMA:
			case VK_OEM_MINUS:
			case VK_OEM_PERIOD:
			case VK_OEM_2:
			case VK_OEM_3:
			case VK_OEM_4:
			case VK_OEM_5:
			case VK_OEM_6:
			case VK_OEM_7:
				return true;
		}

		return false;
	}

	bool menu_can_open()
	{
		if ( config.clicker.i_menu_key == 0 )
			return false;

		if ( vars::key::b_binding_key.load() || menu_wants_text_input() )
			return false;

		if ( focus::is_self_focused() )
			return true;

		if ( config.clicker.b_menu_only_in_game && !vars::key::b_game_focused.load() )
			return false;

		if ( config.clicker.b_menu_block_gui && vars::key::b_cursor_visible.load() )
			return false;

		if ( config.clicker.b_menu_block_typing && vars::key::b_chat_open.load() )
			return false;

		const auto i_debounce = (std::uint64_t) ( config.clicker.i_menu_debounce_ms > 0 ? config.clicker.i_menu_debounce_ms : 0 );
		const auto i_last = vars::key::i_last_typing_ms.load();

		if ( i_debounce && i_last && ( now_ms() - i_last ) < i_debounce )
			return false;

		return true;
	}
}

void c_clicker::click_loop( int i_side )
{
	const bool b_left = ( i_side == 0 );
	auto& activation = b_left ? vars::key::left_activation : vars::key::right_activation;

	SetThreadPriority( GetCurrentThread(), THREAD_PRIORITY_TIME_CRITICAL );

	while ( vars::b_is_running )
	{
		activation.i_mode = b_left ? config.clicker.i_left_mode : config.clicker.i_right_mode;
		activation.i_key = b_left ? config.clicker.i_left_key : config.clicker.i_right_key;

		const bool b_activation = activation.get();

		const bool b_enabled = b_left ? config.clicker.b_enable_left_clicker : config.clicker.b_enable_right_clicker;

		bool b_active = b_enabled && vars::key::b_master_active.load() && b_activation;

		if ( b_active && b_left && config.clicker.b_left_skip_while_right )
			b_active = !( GetAsyncKeyState( VK_RBUTTON ) & 0x8000 );

		if ( b_active )
		{
			if ( config.clicker.b_pause_when_menu_open && vars::b_menu_open.load() )
				b_active = false;
			else if ( !vars::key::b_game_focused.load() )
				b_active = false;
			else if ( !focus::cursor_think() )
				b_active = false;
		}

		( b_left ? vars::stats::b_left_active : vars::stats::b_right_active ).store( b_active );

		if ( !b_active )
		{
			std::this_thread::sleep_for( 2ms );
			continue;
		}

		send_click( i_side, b_left ? config.clicker.f_left_cps : config.clicker.f_right_cps );
	}
}

void c_clicker::send_click( int i_side, float f_cps )
{
	const auto start = std::chrono::high_resolution_clock::now();

	if ( f_cps <= 0.f )
		return;

	if ( !config.clicker.b_enable_blatant )
		f_cps += this->f_random;

	const auto b_button = ( i_side == 0 ) ? input::mouse_button_t::left : input::mouse_button_t::right;

	float f_delay = ( 1000.f / f_cps ) / 2.f;

	if ( !config.clicker.b_enable_blatant )
		f_delay += rng::random_real<float>( 1.f, config.clicker.f_default_timer_randomization );

	// Piso de 50 ms entre inícios de clique: o Minecraft não registra mais rápido
	constexpr float k_min_half_delay = 25.f;
	const bool b_clamped = f_delay < k_min_half_delay;

	if ( b_clamped )
		f_delay = k_min_half_delay;

	// Down delay
	PreciseSleep( (double) f_delay );
	input::click( input::mouse_input_type_t::down, b_button );

	// Up delay
	PreciseSleep( (double) f_delay );
	input::click( input::mouse_input_type_t::up, b_button );

	// Compensação (double click real): se o CPS exigia intervalo < 50 ms, um clique
	// extra sai no próximo slot permitido (>= 50 ms do anterior) para manter a média
	if ( b_clamped )
	{
		PreciseSleep( (double) f_delay );
		input::click( input::mouse_input_type_t::down, b_button );

		PreciseSleep( (double) f_delay );
		input::click( input::mouse_input_type_t::up, b_button );
	}

	const auto end = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double, std::milli> elapsed { end - start };
	vars::stats::f_average_cps = (float) ( 1000.f / elapsed.count() );

	vars::stats::i_clicks_this_session.fetch_add( 1, std::memory_order_relaxed );

	log_debug( "[%d]: side: %s | CPS: %.3f | delay: %.3fms | time elapsed: %.3fms | avg. CPS: %.3f",
		vars::stats::i_clicks_this_session.load( std::memory_order_relaxed ), i_side == 0 ? "left" : "right",
		f_cps, ( f_delay * 2 ), elapsed.count(), vars::stats::f_average_cps );
}

void c_clicker::update_cps()
{
	while ( vars::b_is_running )
	{
		const auto rate = rng::random_real<float>( 1500.f, config.clicker.f_persistence_update_rate );

		if ( b_should_update )
		{
			log_debug( "Update %.3fms", rate );

			// Persistence
			if ( config.clicker.b_enable_persistence )
				this->f_random = rng::random_real<float>( 1.f, config.clicker.f_persistence_value );

			// CPS Drops
			if ( config.clicker.b_enable_cps_drops &&
				config.clicker.i_cps_drop_chance > 0 && ( std::rand() % ( 100 / config.clicker.i_cps_drop_chance ) == 0 ) )
			{
				this->f_random -= config.clicker.f_cps_drop_remove;
			}

			// CPS Spikes
			if ( config.clicker.b_enable_cps_spikes &&
				config.clicker.i_cps_spike_chance > 0 && ( std::rand() % ( 100 / config.clicker.i_cps_spike_chance ) == 0 ) )
			{
				this->f_random += config.clicker.f_cps_spike_add;
			}

			this->b_should_update = false;
		}

		PreciseSleep( (double) rate );

		this->f_random = {};
		this->b_should_update = true;
	}
}

void c_clicker::input_thread()
{
	bool b_menu_down_prev = false;
	bool b_cursor_prev = false;
	bool b_prev_keys[256] {};

	constexpr int tracked_keys[] =
	{
		VK_ESCAPE, VK_RETURN, VK_SPACE, VK_BACK, VK_TAB, VK_DELETE, VK_INSERT, VK_RSHIFT,
		'T', 'E',
		0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39,
		'A', 'B', 'C', 'D', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R',
		'S', 'U', 'V', 'W', 'X', 'Y', 'Z',
		VK_OEM_1, VK_OEM_PLUS, VK_OEM_COMMA, VK_OEM_MINUS, VK_OEM_PERIOD, VK_OEM_2,
		VK_OEM_3, VK_OEM_4, VK_OEM_5, VK_OEM_6, VK_OEM_7
	};

	while ( vars::b_is_running )
	{
		const auto i_now = now_ms();

		vars::key::b_game_focused.store( focus::window_think() );
		vars::key::b_cursor_visible.store( focus::is_cursor_visible() );

		// Chave mestra
		vars::key::clicker_enabled.i_mode = config.clicker.i_key_type;
		vars::key::clicker_enabled.i_key = config.clicker.i_clicker_key;
		vars::key::b_master_active.store( vars::key::clicker_enabled.get() );

		// Janela oculta
		vars::key::hide_window.i_key = config.clicker.i_hide_window_key;
		vars::key::b_hide_forced.store( vars::key::hide_window.get() );

		vars::key::is_left_down.get();
		vars::key::is_right_down.get();

		if ( vars::key::b_game_focused.load() )
		{
			vars::key::b_shift_pressed = ( GetAsyncKeyState( VK_SHIFT ) & 0x8000 ) != 0;

			for ( const auto i_key : tracked_keys )
			{
				if ( i_key == config.clicker.i_menu_key )
				{
					b_prev_keys[i_key] = ( GetAsyncKeyState( i_key ) & 0x8000 ) != 0;
					continue;
				}

				const bool b_down = ( GetAsyncKeyState( i_key ) & 0x8000 ) != 0;
				const bool b_pressed = b_down && !b_prev_keys[i_key];
				b_prev_keys[i_key] = b_down;

				if ( !b_pressed )
					continue;

				if ( is_typing_key( i_key ) )
					vars::key::i_last_typing_ms.store( i_now );

				// Chat: 'T' e '/' abrem, Enter/Escape fecham
				if ( ( i_key == 'T' || i_key == VK_OEM_2 ) && !vars::key::b_chat_open.load() )
					vars::key::b_chat_open.store( true );
				else if ( ( i_key == VK_RETURN || i_key == VK_ESCAPE ) && vars::key::b_chat_open.load() )
					vars::key::b_chat_open.store( false );

				// Inventário (debug)
				if ( ( i_key == 'E' || i_key == VK_RSHIFT ) )
					vars::key::b_inventory_opened = !vars::key::b_inventory_opened;
				else if ( i_key == VK_ESCAPE )
					vars::key::b_inventory_opened = false;
			}
		}

		// Chat fechado ao sair de uma tela (cursor visível -> oculto)
		const bool b_cursor_now = vars::key::b_cursor_visible.load();
		if ( vars::key::b_chat_open.load() && b_cursor_prev && !b_cursor_now )
			vars::key::b_chat_open.store( false );
		b_cursor_prev = b_cursor_now;

		// Expiração de falso positivo de chat (sem digitação por 20s)
		if ( vars::key::b_chat_open.load() )
		{
			const auto i_last = vars::key::i_last_typing_ms.load();
			if ( i_last && ( i_now - i_last ) > 20000 )
				vars::key::b_chat_open.store( false );
		}

		// Tecla do menu
		const int i_menu_key = config.clicker.i_menu_key;
		const bool b_menu_down = i_menu_key && ( ( GetAsyncKeyState( i_menu_key ) & 0x8000 ) != 0 );
		const bool b_menu_pressed = b_menu_down && !b_menu_down_prev;
		b_menu_down_prev = b_menu_down;
		vars::b_menu_key_down.store( b_menu_down );

		if ( config.clicker.i_menu_mode == 1 )
		{
			const bool b_open = b_menu_down && menu_can_open();

			if ( b_open )
				vars::key::b_hide_forced.store( false );

			vars::b_menu_open.store( b_open );
		}
		else if ( b_menu_pressed && !vars::key::b_binding_key.load() && !menu_wants_text_input() )
		{
			if ( vars::b_menu_open.load() && !vars::key::b_hide_forced.load() )
			{
				vars::b_menu_open.store( false );
			}
			else if ( menu_can_open() )
			{
				vars::b_menu_open.store( true );
				vars::key::b_hide_forced.store( false );
			}
		}

		std::this_thread::sleep_for( 1ms );
	}
}
