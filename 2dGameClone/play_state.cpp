#include "play_state.hpp"

#include <iostream> // For debugging
#include <bitset>

#include <allegro5/allegro_primitives.h>

#include "input.hpp"
#include "state_machine.hpp"

constexpr int GRID_SIZE = 3;

void drawX(int x1, int y1, int x2, int y2, ALLEGRO_COLOR cl, int width);

PlayState::PlayState(StateMachine& state_machine, InputHandler& input)
	: AbstractState(state_machine, input), tiles_empty(0), tiles_type(0), current_type(0), playing(true), selected_index(-1)
{
	int display_width = al_get_display_width(al_get_current_display());
	int display_height = al_get_display_height(al_get_current_display());

	tile_size = display_width < display_height ? display_width / 4 : display_height / 4;

	offx = (display_width / 2) - (tile_size * GRID_SIZE / 2);
	offy = (display_height / 2) - (tile_size * GRID_SIZE / 2);

	conditions[0] = 0b000000111; // Top Row
	conditions[1] = 0b000111000; // Middle Row
	conditions[2] = 0b111000000; // Bottom Row
	conditions[3] = 0b001001001; // Left Column
	conditions[4] = 0b010010010; // Middle Column
	conditions[5] = 0b100100100; // Right Column
	conditions[6] = 0b001010100; // Left Diagonal
	conditions[7] = 0b100010001; // Right Diagonal

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
	if (!playing)
	{
		if (m_input.isMousePressed(MOUSE::LEFT))
		{
			tiles_empty = 0;
			tiles_type = 0;
			playing = true;
		}

		return;
	}

	int mouse_x = m_input.getMouseX();
	int mouse_y = m_input.getMouseY();

	selected_index = -1;

	if (mouse_x >= offx && mouse_x <= offx + (tile_size * GRID_SIZE) && mouse_y >= offy && mouse_y <= offy + (tile_size * GRID_SIZE))
	{
		int selected_x = (mouse_x - offx) / tile_size;
		int selected_y = (mouse_y - offy) / tile_size;

		selected_index = selected_y * GRID_SIZE + selected_x;

		if (m_input.isMousePressed(MOUSE::LEFT))
		{
			if (selected_index >= 0 && selected_index < GRID_SIZE * GRID_SIZE)
			{
				uint16_t index = 1 << selected_index;

				if (index & ~tiles_empty)
				{
					tiles_empty = tiles_empty | index;

					if (current_type == 1)
					{
						tiles_type = tiles_type | index;

						current_type = 0;
					}
					else current_type = 1;
				}
			}

			for (int i = 0; i < 8; ++i) // 8 Is size of conditions
			{
				if ((tiles_empty & conditions[i]) == conditions[i])
				{
					if ((tiles_type & conditions[i]) == conditions[i])
					{
						std::cout << "Circle Wins! " << std::endl;
						playing = false;
					}
					else if ((~tiles_type & conditions[i]) == conditions[i])
					{
						std::cout << "Cross Wins! " << std::endl;
						playing = false;
					}
				}
			}

			if (tiles_empty == 0b111111111 && playing == true)
			{
				std::cout << "Its a draw!" << std::endl;
				playing = false;
			}
		}
	}
}
void PlayState::update(double deltaTime)
{
	
}
void PlayState::draw()
{
	int index = 0;
	for (int y = 0; y < GRID_SIZE; ++y)
	{
		for (int x = 0; x < GRID_SIZE; ++x)
		{
			int x1 = offx + (x * tile_size);
			int x2 = x1 + tile_size;
			int y1 = offy + (y * tile_size);
			int y2 = y1 + tile_size;

			float inset = float(tile_size) * 0.15f;
			float radius = (tile_size / 2) - inset;

			if (tiles_empty & (1 << index))
			{
				if ((tiles_type >> index) & 1)
				{
					al_draw_circle(x1 + (tile_size / 2), y1 + (tile_size / 2), radius, al_map_rgb(0xE1, 0x06, 0x00), 6);
				}
				else
				{
					drawX(x1 + inset, y1 + inset, x2 - inset, y2 - inset, al_map_rgb(0x00, 0x23, 0x9C), 6);
				}
			}
			else if (index == selected_index)
			{
				switch (current_type)
				{
				case 0:
					drawX(x1 + inset, y1 + inset, x2 - inset, y2 - inset, al_map_rgb(0x00, 0x23, 0x81), 6);
					break;
				case 1:
					al_draw_circle(x1 + (tile_size / 2), y1 + (tile_size / 2), radius, al_map_rgb(0x9A, 0x06, 0x00), 6);
					break;
				default:
					break;
				}
			}

			index++;
		}
	}

	al_draw_line(offx + tile_size, offy, offx + tile_size, offy + (tile_size * GRID_SIZE), al_map_rgb(255, 255, 255), 4);
	al_draw_line(offx + (tile_size * 2), offy, offx + (tile_size * 2), offy + (tile_size * GRID_SIZE), al_map_rgb(255, 255, 255), 4);

	al_draw_line(offx, offy + tile_size, offx + (tile_size * GRID_SIZE), offy + tile_size, al_map_rgb(255, 255, 255), 4);
	al_draw_line(offx, offy + (tile_size * 2), offx + (tile_size * GRID_SIZE), offy + (tile_size * 2), al_map_rgb(255, 255, 255), 4);
}

void drawX(int x1, int y1, int x2, int y2, ALLEGRO_COLOR cl, int width)
{
	al_draw_line(x1, y1, x2, y2, cl, width);
	al_draw_line(x1, y2, x2, y1, cl, width);
}