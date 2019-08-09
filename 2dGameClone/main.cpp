#include <iostream>
#include <chrono>

#include <allegro5/allegro.h>

#include "input.hpp"
#include "state_machine.hpp"

#include "play_state.hpp"

constexpr int		DEFAULT_WIND_WIDTH = 1000;
constexpr int		DEFAULT_WIND_HEIGHT = 800;
constexpr double	ticksPerSecond = 60.f;

int main(int argc, char ** argv)
{
	bool running = true;
	bool redraw = false;

	ALLEGRO_DISPLAY		*display = nullptr;
	ALLEGRO_EVENT_QUEUE *ev_queue = nullptr;
	ALLEGRO_TIMER		*timer = nullptr;

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

	ev_queue = al_create_event_queue();
	timer = al_create_timer(1.f / ticksPerSecond);

	InputHandler m_input;

	al_register_event_source(ev_queue, al_get_keyboard_event_source());
	al_register_event_source(ev_queue, al_get_mouse_event_source());
	al_register_event_source(ev_queue, al_get_display_event_source(display));
	al_register_event_source(ev_queue, al_get_timer_event_source(timer));

	StateMachine m_sm;
	m_sm.pushState(std::make_unique<PlayState>(m_sm, m_input));

	al_start_timer(timer);
	while (running)
	{
		bool cleanup = false;
		ALLEGRO_EVENT ev;

		if (al_get_next_event(ev_queue, &ev))
		{
			m_input.getInput(ev);
			m_sm.handleEvents();

			if (ev.type == ALLEGRO_EVENT_KEY_DOWN)
			{
				if (m_input.isKeyPressed(ALLEGRO_KEY_ESCAPE))
				{
					running = false;
				}
			}
			else if (ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
			{
				running = false;
			}
			else if (ev.type == ALLEGRO_EVENT_TIMER)
			{
				m_sm.update(al_get_timer_speed(timer));
				cleanup = al_get_timer_count(timer) % int(ticksPerSecond) == 0;
				redraw = true;
			}
		}

		if (al_is_event_queue_empty(ev_queue) && redraw)
		{
			redraw = false;

			al_clear_to_color(al_map_rgb(0, 0, 0));

			m_sm.draw(false);

			al_flip_display();
		}
		if (cleanup) m_sm.removeDeadStates();
	}

	al_destroy_timer(timer);
	al_destroy_event_queue(ev_queue);
	al_destroy_display(display);
}