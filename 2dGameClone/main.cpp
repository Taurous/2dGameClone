#include <iostream> // For std::cerr
#include <chrono>	// For FPS counting and providing game with tick time
#include <fstream>
#include <sstream>

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
constexpr double TARGET_SPF			= 1.0 / 60.0;
std::string DISPLAY_TITLE			= "Circles and Crosses";

int main(int argc, char ** argv)
{
	ALLEGRO_DISPLAY		*display	= nullptr;
	ALLEGRO_EVENT_QUEUE *ev_queue	= nullptr;
	
#ifndef _DEBUG
	std::ofstream log("errorlog.txt");
	if (log.is_open())
	{
		std::cerr.rdbuf(log.rdbuf());
	}
#endif

	if (!al_init())
	{
		
		std::cerr << "Failed to load Allegro!" << std::endl;
		exit(EXIT_FAILURE);
	}

	al_set_new_window_title(DISPLAY_TITLE.c_str());
	display = al_create_display(DEFAULT_WIND_WIDTH, DEFAULT_WIND_HEIGHT);

	if (!display)
	{
		std::cerr << "Failed to create display!" << std::endl;
		exit(EXIT_FAILURE);
	}

	al_init_image_addon();
	al_init_font_addon();
	al_init_ttf_addon();
	al_init_primitives_addon();

	al_install_audio();
	al_init_acodec_addon();
	al_reserve_samples(4);

	ev_queue = al_create_event_queue();

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

			std::stringstream ss;
			ss << DISPLAY_TITLE << " FPS: " << fps;
			al_set_window_title(display, ss.str().c_str());
		}

		if (acc_time >= TARGET_SPF)
		{
			m_sm.draw(false);

			al_flip_display();

			frame_count++;
			acc_time -= TARGET_SPF;
			if (acc_time >= 0.05) std::cout << "Time is accumulating..." << std::endl;
		}

		acc_time += delta_time;

		m_sm.removeDeadStates();
	}
	al_destroy_event_queue(ev_queue);
	al_destroy_display(display);
}