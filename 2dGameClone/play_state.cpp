#include "play_state.hpp"

#include <iostream> // For debugging

#include <allegro5/allegro_primitives.h>

#include "input.hpp"
#include "state_machine.hpp"

PlayState::PlayState(StateMachine &state_machine, InputHandler &input)
	: AbstractState(state_machine, input), tile_size(150), offset_x(0), offset_y(0)
{
	al_init_primitives_addon();

	for (int i = 0; i < NUM_TILES; ++i)
	{
		tiles[i] = EMPTY;
	}

	tiles[4] = CROSS;
	tiles[2] = CROSS;
	tiles[3] = CIRCLE;
	tiles[1] = CIRCLE;

	int total_size = tile_size * 3;

	int display_width = al_get_display_width(al_get_current_display());
	int display_height = al_get_display_height(al_get_current_display());

	offset_x = (display_width / 2) - (total_size / 2);
	offset_y = (display_height / 2) - (total_size / 2);
}


PlayState::~PlayState()
{
}

void PlayState::pause()
{

}
void PlayState::resume()
{

}

void PlayState::handleEvents()
{
	// Only need to do on mouse click
	int mouse_x = m_input.getMouseX();
	int mouse_y = m_input.getMouseY();

	int selected_x = mouse_x - offset_x;
	int selected_y = mouse_y - offset_y;
	if (selected_x >= 0) selected_x /= tile_size;
	else selected_x = -1000000;
	if (selected_y >= 0) selected_y /= tile_size;
	else selected_y = -1000000;

	int index = selected_y * 3 + selected_x;

	if (m_input.isMousePressed(MOUSE::LEFT))
	{
		if (index >= 0 && index < NUM_TILES)
		{
			if (tiles[index] == CROSS) tiles[index] = EMPTY;
			else tiles[index] = CROSS;
		}
	}
	else if (m_input.isMousePressed(MOUSE::RIGHT))
	{
		if (index >= 0 && index < NUM_TILES)
		{
			if (tiles[index] == CIRCLE) tiles[index] = EMPTY;
			else tiles[index] = CIRCLE;
		}
	}
	else if (m_input.isKeyPressed(ALLEGRO_KEY_C))
	{
		for (int i = 0; i < NUM_TILES; ++i)
		{
			tiles[i] = EMPTY;
		}
	}
}
void PlayState::update(double deltaTime)
{

}
void PlayState::draw()
{
	int index = 0;
	for (int y = 0; y < 3; ++y)
	{
		for (int x = 0; x < 3; ++x)
		{
			int x1 = offset_x + (x * tile_size);
			int x2 = x1 + tile_size;
			int y1 = offset_y + (y * tile_size);
			int y2 = y1 + tile_size;

			al_draw_rectangle(x1, y1, x2, y2, al_map_rgb(200, 200, 200), 2);

			float inset = float(tile_size) * 0.15f;
			float radius = (tile_size / 2) - inset;
			switch (tiles[index])
			{
			case CROSS:
				al_draw_line(x1 + inset, y1 + inset, x2 - inset, y2 - inset, al_map_rgb(255, 40, 40), 2);
				al_draw_line(x1 + inset, y2 - inset, x2 - inset, y1 + inset, al_map_rgb(255, 40, 40), 2);
				break;
			case CIRCLE:
				al_draw_circle((x1 + x2) / 2, (y1 + y2) / 2, radius, al_map_rgb(40, 40, 255), 2);
				break;
			default:
				break;
			}

			index++;
		}
	}
}