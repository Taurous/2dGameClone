#pragma once

#include <allegro5/allegro.h>

#include "abstract_state.hpp"
#include <vector>

class MandelbrotState : public AbstractState
{
public:
	MandelbrotState(StateMachine &state_machine, InputHandler &input);

	~MandelbrotState() override;

	void pause() override;
	void resume() override;

	void handleEvents() override;
	void update(double deltaTime) override;
	void draw() override;

private:
	ALLEGRO_BITMAP *bmp;
};