#pragma once

#include <allegro5/allegro.h>

enum MOUSE
{
	LEFT = 1,
	RIGHT = 2,
	MIDDLE = 3
};
enum MOD
{
	ANY		= 0b0000, // Ignores modifiers that are pressed (Allowing any modifier to be pressed)
	NONE	= 0b0001, // Requires that no modifiers be pressed
	SHIFT	= 0b0010,
	CTRL	= 0b0100,
	ALT		= 0b1000
};

class InputHandler
{
public:
	InputHandler();

	InputHandler(const InputHandler &other) = delete;
	InputHandler &operator=(const InputHandler &other) = delete;

	InputHandler(InputHandler &&other) = delete;
	InputHandler &operator=(InputHandler &&other) = delete;

	~InputHandler();

	void getInput(const ALLEGRO_EVENT &ev);

	char getChar() const;

	bool isKeyPressed(const int key, const char &mod = MOD::ANY) const;
	bool isKeyReleased(const int key, const char &mod = MOD::ANY) const;
	bool isKeyDown(const int key, const char &mod = MOD::ANY) const;

	bool isMousePressed(const int button, const char &mod = MOD::ANY) const;
	bool isMouseReleased(const int button, const char &mod = MOD::ANY) const;
	bool isMouseDown(const int button, const char &mod = MOD::ANY) const;
	bool isMouseWheelDown(const char &mod = MOD::ANY) const;
	bool isMouseWheelUp(const char &mod = MOD::ANY) const;

	int getMouseX() const;
	int getMouseY() const;
	bool isMouseInWindow() const;

private:
	char m_char_pressed;
	char m_mod_flags;

	ALLEGRO_KEYBOARD_STATE m_prev_key_state;
	ALLEGRO_KEYBOARD_STATE m_cur_key_state;

	ALLEGRO_MOUSE_STATE m_prev_mouse_state;
	ALLEGRO_MOUSE_STATE m_cur_mouse_state;
};