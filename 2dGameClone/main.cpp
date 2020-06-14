#include <iostream>
#include <sstream>
#include <numeric>
#include <execution>
#include <algorithm>
#include <chrono>

#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>

#include "input.hpp"
#include "state_machine.hpp"

#include "splash_state.hpp"

constexpr int		DEFAULT_WIND_WIDTH = 600;
constexpr int		DEFAULT_WIND_HEIGHT = 800;

int main(int argc, char ** argv)
{
	int prev_count = 0;
	int frame_count = 0;
	double acc_time = 0.f;

	ALLEGRO_DISPLAY		*display	= nullptr;
	ALLEGRO_EVENT_QUEUE *ev_queue	= nullptr;
	ALLEGRO_FONT		*fps_font	= nullptr;

	if (!al_init())
	{
		std::cerr << "Failed to load Allegro!" << std::endl;
		return -1;
	}

	display = al_create_display(DEFAULT_WIND_WIDTH, DEFAULT_WIND_HEIGHT);

	if (!display)
	{
		std::cerr << "Failed to create display!" << std::endl;
		return -1;
	}

	al_init_image_addon();
	al_init_font_addon();
	al_init_ttf_addon();
	al_init_primitives_addon();

	ev_queue = al_create_event_queue();

	fps_font = al_load_font("forced_square.ttf", 40, 0);

	InputHandler m_input;

	al_register_event_source(ev_queue, al_get_keyboard_event_source());
	al_register_event_source(ev_queue, al_get_mouse_event_source());
	al_register_event_source(ev_queue, al_get_display_event_source(display));

	StateMachine m_sm;
	m_sm.pushState(std::make_unique<SplashState>(m_sm, m_input));

	auto prev = std::chrono::high_resolution_clock::now();
	while (m_sm.running())
	{
		ALLEGRO_EVENT ev;

		if (al_get_next_event(ev_queue, &ev))
		{
			m_input.getInput(ev);
			m_sm.handleEvents();

			if (ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
			{
				//TODO: Signal to state that window is being closed, save stuff
				m_sm.quit();
			}
		}

		auto now = std::chrono::high_resolution_clock::now();
		std::chrono::duration<double, std::milli> deltaTime = now - prev;
		m_sm.update(deltaTime.count() / 1000.0);
		prev = now;

		acc_time += deltaTime.count();

		double time_interval = 1000.0;
		if (acc_time >= time_interval)
		{
			acc_time -= time_interval;
			prev_count = frame_count;
			frame_count = 0;
		}

		al_clear_to_color(al_map_rgb(10, 10, 20));

		m_sm.draw(false);

		if (fps_font) al_draw_textf(fps_font, al_map_rgb(235, 213, 52), 10, 10, 0, "%i", prev_count * int(1000.0 / time_interval));

		al_flip_display();
		frame_count++;

		m_sm.removeDeadStates();
	}

	al_destroy_event_queue(ev_queue);
	al_destroy_display(display);
}