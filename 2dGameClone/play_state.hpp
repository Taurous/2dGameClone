#pragma once

#include "abstract_state.hpp"
#include <stdint.h>

class PlayState : public AbstractState
{
public:
	PlayState(StateMachine &state_machine, InputHandler &input);

	~PlayState() override;

	void pause() override;
	void resume() override;

	void handleEvents() override;
	void update(double deltaTime) override;
	void draw() override;

private:
	uint16_t tiles_empty;
	uint16_t tiles_type;
	uint8_t current_type;

	uint16_t conditions[8];

	int tile_size;
	bool playing;

	int offx, offy;
	int selected_index;
};