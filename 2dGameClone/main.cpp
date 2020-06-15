#include <iostream> // For std::cerr
#include <chrono>	// For FPS counting and providing game with tick time

#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>

#include "input.hpp"
#include "state_machine.hpp"
#include "splash_state.hpp"

constexpr int DEFAULT_WIND_WIDTH	= 600;
constexpr int DEFAULT_WIND_HEIGHT	= 800;
constexpr double const_fps			= 60.0;

int main(int argc, char ** argv)
{
	ALLEGRO_DISPLAY		*display	= nullptr;
	ALLEGRO_EVENT_QUEUE *ev_queue	= nullptr;
	ALLEGRO_FONT		*fps_font	= nullptr;

	bool draw_fps = false;

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

	al_install_audio();
	al_init_acodec_addon();
	al_reserve_samples(4);

	ev_queue = al_create_event_queue();

	fps_font = al_load_font("Resources/font/forced_square.ttf", 40, 0);

	InputHandler m_input;

	al_register_event_source(ev_queue, al_get_keyboard_event_source());
	al_register_event_source(ev_queue, al_get_mouse_event_source());
	al_register_event_source(ev_queue, al_get_display_event_source(display));

	StateMachine m_sm;
	m_sm.pushState(std::make_unique<SplashState>(m_sm, m_input));

	unsigned int frame_count = 0;
	double fps_time = 0.0;
	double acc_time = 0.0f;
	int fps = 0.0;

	auto last_time = std::chrono::high_resolution_clock::now();
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
			
			if (m_input.isKeyPressed(ALLEGRO_KEY_F))
			{
				draw_fps = !draw_fps;
			}
		}

		auto current_time = std::chrono::high_resolution_clock::now();
		auto delta_time = std::chrono::duration<double>(current_time - last_time).count();
		last_time = current_time;

		m_sm.update(delta_time);

		fps_time += delta_time;
		if (fps_time >= 0.5)
		{
			fps_time /= frame_count;
			fps = int(1.0 / fps_time);
			fps_time = 0.0;
			frame_count = 0;
		}

		if (acc_time >= 1.0 / const_fps)
		{
			al_clear_to_color(al_map_rgb(10, 10, 20));

			m_sm.draw(false);

			if (fps_font && draw_fps) al_draw_textf(fps_font, al_map_rgb(235, 213, 52), 10, 10, 0, "%i", fps);

			al_flip_display();

			frame_count++;
			acc_time -= 1.0 / const_fps;
		}

		acc_time += delta_time;

		m_sm.removeDeadStates();
	}

	al_destroy_event_queue(ev_queue);
	al_destroy_display(display);
}