#include "play_state.hpp"

#include <iostream> // For debugging

#include <allegro5/allegro_primitives.h>

#include "input.hpp"
#include "state_machine.hpp"

constexpr auto GRAVITY = 9.81 * 20;

PlayState::PlayState(StateMachine &state_machine, InputHandler &input)
	: AbstractState(state_machine, input)
{
	srand(time(NULL));
	
	worldObjectArray = new WorldObject[MAX_WORLD_OBJECTS];

	for (int i = 0; i < MAX_WORLD_OBJECTS; ++i)
	{
		worldObjectArray[i].x = i * 10;
		worldObjectArray[i].y = rand() % 100;
		worldObjectArray[i].width = 8;
		worldObjectArray[i].height = 10;
		worldObjectArray[i].x_vel = 0;
		worldObjectArray[i].y_vel = double(rand() % 31 - 15);
		worldObjectArray[i].x_acc = 0;
		worldObjectArray[i].y_acc = GRAVITY;
	}

	al_init_primitives_addon();
}

PlayState::~PlayState()
{
	delete worldObjectArray;
}

void PlayState::pause()
{

}
void PlayState::resume()
{

}

void PlayState::handleEvents()
{

}
void PlayState::update(double deltaTime)
{
	for (int i = 0; i < MAX_WORLD_OBJECTS; ++i)
	{
		worldObjectArray[i].x_vel += worldObjectArray[i].x_acc * deltaTime;
		worldObjectArray[i].x += worldObjectArray[i].x_vel * deltaTime;

		worldObjectArray[i].y_vel += worldObjectArray[i].y_acc * deltaTime;
		worldObjectArray[i].y += worldObjectArray[i].y_vel * deltaTime;

		if (worldObjectArray[i].y > 600 - worldObjectArray[i].height && worldObjectArray[i].y_vel >= 1.f)
		{
			worldObjectArray[i].y_vel = -0.6f * worldObjectArray[i].y_vel;
		}

		if (worldObjectArray[i].y + worldObjectArray[i].height > 600 && abs(worldObjectArray[i].y_vel) < 1.f)
		{
			worldObjectArray[i].y = 600 - worldObjectArray[i].height - 2;
			worldObjectArray[i].y_acc = 0;
			worldObjectArray[i].y_vel = 0;
		}
	}

	printf("Speed: %.2f\n", worldObjectArray[0].y_vel);
}
void PlayState::draw()
{
	int x1, y1, x2, y2;

	for (int i = 0; i < MAX_WORLD_OBJECTS; ++i)
	{
		x1 = worldObjectArray[i].x;
		y1 = worldObjectArray[i].y;
		x2 = x1 + worldObjectArray[i].width;
		y2 = y1 + worldObjectArray[i].height;

		al_draw_rectangle(x1, y1, x2, y2, al_map_rgb(255, 0, 255), 2);
	}
}