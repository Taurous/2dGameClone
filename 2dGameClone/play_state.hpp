#pragma once

#include "abstract_state.hpp"

constexpr auto MAX_WORLD_OBJECTS = 100;

struct WorldObject
{
	double x, y;
	double x_vel, y_vel;
	double x_acc, y_acc;

	double width, height;
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
	WorldObject *worldObjectArray;
};