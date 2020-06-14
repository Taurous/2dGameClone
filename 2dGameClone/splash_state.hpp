#pragma once

#include <allegro5/allegro.h>
#include "abstract_state.hpp"

class SplashState : public AbstractState
{
public:
	SplashState(StateMachine& state_machine, InputHandler& input);

	~SplashState() override;

	void pause() override;
	void resume() override;

	void handleEvents() override;
	void update(double deltaTime) override;
	void draw() override;

private:
	ALLEGRO_BITMAP* al_logo;
	ALLEGRO_BITMAP* game_logo;
	int al_logo_w, al_logo_h, game_logo_w, game_logo_h;
	double acc_time;
};