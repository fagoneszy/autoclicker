#pragma once

#include <string>

enum keybind_state_t: int
{
	always = 0,
	hold,
	toggle
};

class keybind
{
public:
	bool b_state = false;
	bool b_is_down = false;

	int i_mode = 0;
	int i_key = 0;

	keybind() = default;
	explicit keybind( bool state, int mode = 0, int key = 0 )
	{
		b_state = state;
		i_mode = mode;
		i_key = key;
	}

	inline bool get()
	{
		switch ( i_mode )
		{
			case keybind_state_t::always:
				b_state = true;
				break;

			case keybind_state_t::hold:
				b_state = i_key && ( GetAsyncKeyState( i_key ) & 0x8000 );
				break;

			case keybind_state_t::toggle:
			{
				if ( !i_key )
				{
					b_state = false;
					b_is_down = false;
					break;
				}

				const auto h_state = GetAsyncKeyState( i_key );
				const bool b_down = ( h_state & 0x8000 ) != 0;

				if ( b_down )
					b_is_down = true;

				if ( b_is_down && !b_down )
				{
					b_state = !b_state;
					b_is_down = false;
				}
				break;
			}
		}

		return b_state;
	}

	inline std::wstring get_mode_as_string() const
	{
		switch ( i_mode )
		{
			case keybind_state_t::always:
				return std::wstring( L"on" );
			case keybind_state_t::hold:
				return std::wstring( L"hold" );
			case keybind_state_t::toggle:
				return std::wstring( L"toggle" );
		}

		return {};
	}
};
