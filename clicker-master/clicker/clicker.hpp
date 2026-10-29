#pragma once

#define PreciseSleep(ms) { timer::precise_sleep(ms / 1000.); }

class c_clicker
{
private:
	void send_click( int i_side, float f_cps );

	float f_random = {};

	bool b_should_update = {};

public:
	void click_loop( int i_side );
	void update_cps();
	void input_thread();
};

inline auto g_clicker = std::make_unique<c_clicker>();
