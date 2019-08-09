#pragma once

#include "abstract_state.hpp"

constexpr int NUM_TILES = 9;
enum
{
	EMPTY,
	CROSS,
	CIRCLE
};

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
	int tiles[NUM_TILES];
	int tile_size;
	int offset_x;
	int offset_y;
};