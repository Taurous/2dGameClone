#include <iostream>

#include <allegro5/allegro.h>

#include "input.hpp"

constexpr int		DEFAULT_WIND_WIDTH = 1000;
constexpr int		DEFAULT_WIND_HEIGHT = 600;
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

	al_set_new_display_flags(ALLEGRO_FULLSCREEN_WINDOW);
	display = al_create_display(DEFAULT_WIND_WIDTH, DEFAULT_WIND_HEIGHT);

	if (!display)
	{
		std::cerr << "Failed to create display!" << std::endl;
		return -1;
	}

	ev_queue = al_create_event_queue();
	timer = al_create_timer(60.f / 1.f);

	InputHandler m_input;

	al_register_event_source(ev_queue, al_get_keyboard_event_source());
	al_register_event_source(ev_queue, al_get_mouse_event_source());
	al_register_event_source(ev_queue, al_get_display_event_source(display));
	al_register_event_source(ev_queue, al_get_timer_event_source(timer));

	al_start_timer(timer);
	while (running)
	{
		ALLEGRO_EVENT ev;
		if (al_get_next_event(ev_queue, &ev))
		{
			m_input.getInput(ev);
			 
			if (ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE || m_input.isKeyPressed(ALLEGRO_KEY_ESCAPE))
			{
				running = false;
			}
			else if (ev.type == ALLEGRO_EVENT_TIMER)
			{
				redraw = true;
			}
		}

		if (al_is_event_queue_empty(ev_queue) && redraw)
		{
			redraw = false;

			al_clear_to_color(al_map_rgb(0, 0, 0));

			//Draw

			al_flip_display();
		}
	}

	al_destroy_timer(timer);
	al_destroy_event_queue(ev_queue);
	al_destroy_display(display);
}