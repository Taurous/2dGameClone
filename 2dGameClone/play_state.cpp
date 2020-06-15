#include "play_state.hpp"

#include <bitset>

#include <allegro5/allegro_primitives.h>

#include "input.hpp"
#include "state_machine.hpp"

constexpr int GRID_SIZE = 3;

PlayState::PlayState(StateMachine& state_machine, InputHandler& input)
	: AbstractState(state_machine, input),
	tiles_empty(0), tiles_type(0), current_type(0), playing(true), selected_index(-1),
	bmp_circle(nullptr), bmp_cross(nullptr), fnt_score(nullptr), click(nullptr),
	cross_score(0), circle_score(0), win_state(0)
{
	bmp_circle = al_load_bitmap("Resources/tex/circle.png");
	bmp_cross = al_load_bitmap("Resources/tex/cross.png");
	fnt_score = al_load_font("Resources/font/UASQUARE.TTF", 42, 0);
	fnt_win = al_load_font("Resources/font/UASQUARE.TTF", 68, 0);
	click = al_load_sample("Resources/sound/click.wav");

	if (!bmp_cross || !bmp_circle || !fnt_score || !click) exit(-1);

	bmp_size = al_get_bitmap_width(bmp_circle);

	display_width = al_get_display_width(al_get_current_display());
	display_height = al_get_display_height(al_get_current_display());

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

	if (mouse_x >= offx && mouse_x < offx + (tile_size * GRID_SIZE) && mouse_y >= offy && mouse_y < offy + (tile_size * GRID_SIZE))
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

					al_stop_samples();
					al_play_sample(click, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, nullptr);
				}
			}

			for (int i = 0; i < 8; ++i) // 8 Is size of conditions
			{
				if ((tiles_empty & conditions[i]) == conditions[i])
				{
					if ((tiles_type & conditions[i]) == conditions[i])
					{
						win_state = CIRCLE_WIN;
						circle_score++;
						playing = false;
					}
					else if ((~tiles_type & conditions[i]) == conditions[i])
					{
						win_state = CROSS_WIN;
						cross_score++;
						playing = false;
					}
				}
			}

			if (tiles_empty == 0b111111111 && playing == true)
			{
				win_state = DRAW;
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
	for (int y = offy; y < offy + GRID_SIZE * tile_size; y+=tile_size)
	{
		for (int x = offx; x < offx + GRID_SIZE * tile_size; x+=tile_size)
		{
			if (tiles_empty & (1 << index))
			{
				if ((tiles_type >> index) & 1)
				{
					al_draw_tinted_scaled_bitmap(bmp_circle, al_map_rgb(0xE1, 0x06, 0x00), 0, 0, bmp_size, bmp_size, x, y, tile_size, tile_size, 0);
				}
				else
				{
					al_draw_tinted_scaled_bitmap(bmp_cross, al_map_rgb(0x00, 0x23, 0x9C), 0, 0, bmp_size, bmp_size, x, y, tile_size, tile_size, 0);
				}
			}
			else if (index == selected_index)
			{
				int inset = 12;
				int inset2 = inset * 2;
				switch (current_type)
				{
				case 0:
					al_draw_tinted_scaled_bitmap(bmp_cross, al_map_rgb(0x00, 0x13, 0x59), 0, 0, bmp_size, bmp_size, x+inset, y+inset, tile_size-inset2, tile_size-inset2, 0);
					break;
				case 1:
					al_draw_tinted_scaled_bitmap(bmp_circle, al_map_rgb(0x92, 0x04, 0x00), 0, 0, bmp_size, bmp_size, x + inset, y + inset, tile_size - inset2, tile_size - inset2, 0);
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

	int screen_mid = display_width / 2;

	al_draw_textf(fnt_score, al_map_rgb(0xFA, 0x9C, 0x10), screen_mid - 12, 18, ALLEGRO_ALIGN_RIGHT, "%i - Cross", cross_score);
	al_draw_textf(fnt_score, al_map_rgb(0xFA, 0x9C, 0x10), screen_mid + 12, 18, ALLEGRO_ALIGN_LEFT, "Circle - %i", circle_score);

	if (!playing)
	{
		switch (win_state)
		{
		case DRAW:
			al_draw_text(fnt_win, al_map_rgb(0xFA, 0x9C, 0x10), screen_mid, display_height - 96, ALLEGRO_ALIGN_CENTRE, "It's a Draw!");
			break;
		case CROSS_WIN:
			al_draw_text(fnt_win, al_map_rgb(0x00, 0x23, 0x9C), screen_mid, display_height - 96, ALLEGRO_ALIGN_CENTRE, "Cross Wins!");
			break;
		case CIRCLE_WIN:
			al_draw_text(fnt_win, al_map_rgb(0xE1, 0x06, 0x00), screen_mid, display_height - 96, ALLEGRO_ALIGN_CENTRE, "Circle Wins!");
			break;
		}
	}
}