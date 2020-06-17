#include <iostream>
#include "play_state.hpp"

#include <allegro5/allegro_primitives.h>

#include "input.hpp"
#include "state_machine.hpp"

constexpr int GRID_SIZE = 3;

PlayState::PlayState(StateMachine& state_machine, InputHandler& input)
	: AbstractState(state_machine, input),
	tiles_empty(0), tiles_type(0), current_type(0), playing(true), selected_index(-1),
	bmp_circle(nullptr), bmp_cross(nullptr), fnt_score(nullptr), smp_click(nullptr), bmp_back(nullptr), bmp_bar(nullptr), bmp_grid(nullptr),
	cross_score(0), circle_score(0), win_state(0), back_speed(-50), back_position(0)
{
	if (!loadResources()) exit(EXIT_FAILURE);

	display_width = al_get_display_width(al_get_current_display());
	display_height = al_get_display_height(al_get_current_display());

	tile_size = 150;

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

	anims.push_back(FontAnimation(fnt_score, 0));
	anims.push_back(FontAnimation(fnt_score, 0));
}

PlayState::~PlayState()
{
	al_destroy_bitmap(bmp_circle);
	al_destroy_bitmap(bmp_cross);
	al_destroy_bitmap(bmp_back);
	al_destroy_font(fnt_score);
	al_destroy_font(fnt_win);
	al_destroy_sample(smp_click);
}

void PlayState::handleEvents()
{
	// If playing is false, then the game is in the win state, click to reset
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
					al_play_sample(smp_click, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, nullptr);
				}
			}

			for (int i = 0; i < 8; ++i) // 8 Is size of conditions
			{
				if ((tiles_empty & conditions[i]) == conditions[i])
				{
					if ((tiles_type & conditions[i]) == conditions[i])
					{
						win_state = CIRCLE_WIN;
						anims[0].start(++circle_score, 0.15);
						playing = false;
					}
					else if ((~tiles_type & conditions[i]) == conditions[i])
					{
						win_state = CROSS_WIN;
						anims[1].start(++cross_score, 0.15);
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
	back_position += back_speed * deltaTime;
	if (back_position < 0) back_position += display_width;

	for (auto& fA : anims)
	{
		fA.update(deltaTime);
	}
}
void PlayState::draw()
{
	ALLEGRO_COLOR clear_color = al_map_rgb(0x10, 0x10, 0x10);
	al_clear_to_color(clear_color);

	//Background
	int back_y = 70;
	int back_h = display_height - back_y;
	if (!playing) back_h -= 100;
	al_hold_bitmap_drawing(true);
	al_draw_bitmap_region(bmp_back, display_width - back_position, 0, back_position, back_h, 0, back_y, 0);
	al_draw_bitmap_region(bmp_back, 0, 0, display_width - back_position, back_h, back_position, back_y, 0);
	al_hold_bitmap_drawing(false);

	al_draw_tinted_bitmap(bmp_bar, clear_color, 0, back_y, 0);
	if (!playing) al_draw_tinted_bitmap(bmp_bar, clear_color, 0, back_y+back_h-al_get_bitmap_height(bmp_bar), ALLEGRO_FLIP_VERTICAL);

	//Draw Grid
	al_draw_bitmap(bmp_grid, offx, offy, 0);

	//Drawing Crosses and Circles
	int index = 0;
	for (int y = offy; y < offy + GRID_SIZE * tile_size; y+=tile_size)
	{
		for (int x = offx; x < offx + GRID_SIZE * tile_size; x+=tile_size)
		{
			if (tiles_empty & (1 << index))
			{
				if ((tiles_type >> index) & 1)
				{
					al_draw_tinted_bitmap(bmp_circle, al_map_rgb(0xE1, 0x06, 0x00), x, y, 0);
				}
				else
				{
					al_draw_tinted_bitmap(bmp_cross, al_map_rgb(0x00, 0x23, 0x9C), x, y, 0);
				}
			}
			else if (index == selected_index)
			{
				int inset = 12;
				int inset2 = inset * 2;
				int sz = 1;
				switch (current_type)
				{
				case 0:
					sz = al_get_bitmap_width(bmp_cross);
					al_draw_tinted_scaled_bitmap(bmp_cross, al_map_rgb(0x00, 0x13, 0x59), 0, 0, sz, sz, x+inset, y+inset, tile_size-inset2, tile_size-inset2, 0);
					break;
				case 1:
					sz = al_get_bitmap_width(bmp_circle);
					al_draw_tinted_scaled_bitmap(bmp_circle, al_map_rgb(0x92, 0x04, 0x00), 0, 0, sz, sz, x + inset, y + inset, tile_size - inset2, tile_size - inset2, 0);
					break;
				default:
					break;
				}
			}

			index++;
		}
	}

	int screen_mid = display_width / 2;

	al_draw_text(fnt_score, al_map_rgb(0xFA, 0x9C, 0x10), screen_mid - 12, 12, ALLEGRO_ALIGN_RIGHT, "- Cross");
	anims[1].draw(screen_mid - 12 - al_get_text_width(fnt_score, " - Cross"), 12, al_map_rgb(0xFA, 0x9C, 0x10), ALLEGRO_ALIGN_RIGHT);

	al_draw_text(fnt_score, al_map_rgb(0xFA, 0x9C, 0x10), screen_mid + 12, 12, ALLEGRO_ALIGN_LEFT, "Circle -");
	anims[0].draw(screen_mid + 12 + al_get_text_width(fnt_score, "Circle - "), 12, al_map_rgb(0xFA, 0x9C, 0x10), ALLEGRO_ALIGN_LEFT);


	if (!playing)
	{
		std::string text;
		ALLEGRO_COLOR cl = al_map_rgb(0, 0, 0);

		switch (win_state)
		{
		case DRAW:
			cl = al_map_rgb(0xFA, 0x9C, 0x10);
			text = "It's a draw!";
			break;
		case CROSS_WIN:
			cl = al_map_rgb(0x00, 0x23, 0x9C);
			text = "Cross Wins!";
			break;
		case CIRCLE_WIN:
			cl = al_map_rgb(0xE1, 0x06, 0x00);
			text = "Circle Wins!";
			break;
		}

		al_draw_text(fnt_win, cl, screen_mid, display_height - 96, ALLEGRO_ALIGN_CENTRE, text.c_str());
	}
}

bool PlayState::loadResources()
{
	bool success = true;

	const char bCircle[] = "Resources/tex/circle.png";
	const char bCross[] = "Resources/tex/cross.png";
	const char bBack[] = "Resources/tex/back.png";
	const char bBar[] = "Resources/tex/bar.png";
	const char bGrid[] = "Resources/tex/grid.png";

	const char fFont[] = "Resources/font/UASQUARE.TTF";

	const char sClick[] = "Resources/sound/click.wav";

	bmp_circle = al_load_bitmap(bCircle);
	if (!bmp_circle)
	{
		std::cerr << "Failed to load: " << bCircle << std::endl;
		success = false;
	}

	bmp_cross = al_load_bitmap(bCross);
	if (!bmp_cross)
	{
		std::cerr << "Failed to load: " << bCross << std::endl;
		success = false;
	}

	bmp_back = al_load_bitmap(bBack);
	if (!bmp_back)
	{
		std::cerr << "Failed to load: " << bBack << std::endl;
		success = false;
	}

	bmp_bar = al_load_bitmap(bBar);
	if (!bmp_bar)
	{
		std::cerr << "Failed to load: " << bBar << std::endl;
		success = false;
	}
	
	bmp_grid = al_load_bitmap(bGrid);
	if (!bmp_grid)
	{
		std::cerr << "Failed to load: " << bGrid << std::endl;
		success = false;
	}

	fnt_score = al_load_font(fFont, 42, 0);
	fnt_win = al_load_font(fFont, 68, 0);
	if (!fnt_score || !fnt_win)
	{
		std::cerr << "Failed to load: " << fFont << std::endl;
		success = false;
	}

	smp_click = al_load_sample(sClick);
	if (!smp_click)
	{
		std::cerr << "Failed to load: " << sClick << std::endl;
		success = false;
	}

	return success;
}
