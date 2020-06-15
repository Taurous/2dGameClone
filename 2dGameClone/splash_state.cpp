#include "splash_state.hpp"
#include "play_state.hpp"

#include "input.hpp"
#include "state_machine.hpp"

constexpr float total_time = 4.f;
constexpr float fade_in_time = 0.5f;
constexpr float fade_duration = 0.15f;
constexpr float fade_out_time = 3.6f;

SplashState::SplashState(StateMachine& state_machine, InputHandler& input)
	: AbstractState(state_machine, input), acc_time(0)
{
	al_logo = al_load_bitmap("Resources/tex/allegro_logo.png");
	al_logo_w = al_get_bitmap_width(al_logo);
	al_logo_h = al_get_bitmap_height(al_logo);

	game_logo = al_load_bitmap("Resources/tex/game_logo_256.png");
	game_logo_w = al_get_bitmap_width(game_logo);
	game_logo_h = al_get_bitmap_height(game_logo);

}

SplashState::~SplashState()
{

}

void SplashState::pause()
{
}

void SplashState::resume()
{
}

void SplashState::handleEvents()
{
	if (m_input.isMousePressed(MOUSE::LEFT))
	{
		m_states.changeState(std::make_unique<PlayState>(m_states, m_input));
	}
}

void SplashState::update(double deltaTime)
{
	acc_time += deltaTime;
	if (acc_time >= total_time)
	{
		m_states.changeState(std::make_unique<PlayState>(m_states, m_input));
	}
}

void SplashState::draw()
{
	float val = 0.f;

	if (acc_time >= fade_out_time)
	{
		val = 1.f - ((acc_time - fade_out_time) / fade_duration);
		if (val < 0.f) val = 0.f;
	}
	else if (acc_time >= fade_in_time)
	{
		val = (acc_time-fade_in_time) / fade_duration;
		if (val > 1.f) val = 1.f;
	}

	ALLEGRO_COLOR cl = al_map_rgba_f(val, val, val, val);

	int dispW = al_get_display_width(al_get_current_display());
	int dispH = al_get_display_height(al_get_current_display());

	al_draw_bitmap(al_logo, dispW - al_logo_w - 8, dispH - al_logo_h - 8, 0);
	al_draw_tinted_bitmap(game_logo, cl, (dispW / 2) - (game_logo_w / 2), (dispH / 2) - (game_logo_h / 2), 0);
}
