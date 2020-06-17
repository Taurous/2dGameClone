#pragma once
#include <stdint.h>
#include <vector>

#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_audio.h>

#include "abstract_state.hpp"
#include "font_animation.hpp"

class PlayState : public AbstractState
{
public:
	PlayState(StateMachine &state_machine, InputHandler &input);

	~PlayState() override;

	void pause() override { };
	void resume() override { };

	void handleEvents() override;
	void update(double deltaTime) override;
	void draw() override;

private:
	bool loadResources();

	std::vector<FontAnimation> anims;

	ALLEGRO_BITMAP* bmp_circle;
	ALLEGRO_BITMAP* bmp_cross;
	ALLEGRO_BITMAP* bmp_back;
	ALLEGRO_BITMAP* bmp_bar;
	ALLEGRO_BITMAP* bmp_grid;

	ALLEGRO_FONT* fnt_score;
	ALLEGRO_FONT* fnt_win;

	ALLEGRO_SAMPLE* smp_click;

	uint16_t tiles_empty;
	uint16_t tiles_type;
	uint8_t current_type;

	uint16_t conditions[8];

	double back_position;
	double back_speed;
	int tile_size;
	bool playing;
	int win_state;
	int cross_score, circle_score;

	int offx, offy;
	int selected_index;

	int display_width, display_height;

	enum
	{
		DRAW,
		CROSS_WIN,
		CIRCLE_WIN
	};
};