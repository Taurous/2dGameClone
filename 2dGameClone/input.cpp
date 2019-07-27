#include "input.hpp"

#include <iostream> // For std::cerr

void setBit(char bytefield, char bit, bool flag)
{

}

InputHandler::InputHandler() : m_mod_flags(MOD::NONE), m_char_pressed(NULL)
{
	if (!al_is_system_installed())
	{
		std::cerr << "input - Allegro has not been installed!" << std::endl;
		abort();
	}

	al_install_keyboard();
	al_install_mouse();

	al_get_mouse_state(&m_prev_mouse_state);
	m_cur_mouse_state = m_prev_mouse_state;

	al_get_keyboard_state(&m_prev_key_state);
	m_cur_key_state = m_prev_key_state;
}

InputHandler::~InputHandler()
{
	al_uninstall_keyboard();
	al_uninstall_mouse();
}

void InputHandler::getInput(const ALLEGRO_EVENT &ev)
{
	m_char_pressed = NULL;

	m_prev_mouse_state = m_cur_mouse_state;
	al_get_mouse_state(&m_cur_mouse_state);

	m_prev_key_state = m_cur_key_state;
	al_get_keyboard_state(&m_cur_key_state);

	if (ev.type == ALLEGRO_EVENT_KEY_DOWN)
	{
		// Get Modifiers Pressed
		switch (ev.keyboard.keycode)
		{
		case ALLEGRO_KEY_LSHIFT:
		case ALLEGRO_KEY_RSHIFT:
			setBit(m_mod_flags, MOD::SHIFT, true);
			setBit(m_mod_flags, MOD::NONE, false);
			break;
		case ALLEGRO_KEY_ALTGR:
		case ALLEGRO_KEY_ALT:
			setBit(m_mod_flags, MOD::ALT, true);
			setBit(m_mod_flags, MOD::NONE, false);
			break;
		case ALLEGRO_KEY_LCTRL:
		case ALLEGRO_KEY_RCTRL:
			setBit(m_mod_flags, MOD::CTRL, true);
			setBit(m_mod_flags, MOD::NONE, false);
			break;
		default:
			break;
		};
	}

	// Clear Modifiers Released
	else if (ev.type == ALLEGRO_EVENT_KEY_UP)
	{
		switch (ev.keyboard.keycode)
		{
		case ALLEGRO_KEY_LSHIFT:
		case ALLEGRO_KEY_RSHIFT:
			setBit(m_mod_flags, MOD::SHIFT, false);
			break;
		case ALLEGRO_KEY_ALTGR:
		case ALLEGRO_KEY_ALT:
			setBit(m_mod_flags, MOD::ALT, false);
			break;
		case ALLEGRO_KEY_LCTRL:
		case ALLEGRO_KEY_RCTRL:
			setBit(m_mod_flags, MOD::CTRL, false);
			break;
		default:
			break;
		};

		if (!m_mod_flags) m_mod_flags = MOD::NONE;
	}

	// Catch unicode key pressed
	else if (ev.type == ALLEGRO_EVENT_KEY_CHAR)
	{
		m_char_pressed = ev.keyboard.unichar;
	}
}

char InputHandler::getChar() const
{
	return m_char_pressed;
}

bool InputHandler::isKeyPressed(const int key, const char &mod) const
{
	if (al_key_down(&m_cur_key_state, key) && !al_key_down(&m_prev_key_state, key))
	{
		if (mod == MOD::ANY || mod == m_mod_flags) return true;
	}

	return false;
}
bool InputHandler::isKeyReleased(const int key, const char &mod) const
{
	if (al_key_down(&m_prev_key_state, key) && !al_key_down(&m_cur_key_state, key))
	{
		if (mod == MOD::ANY || mod == m_mod_flags) return true;
	}

	return false;
}
bool InputHandler::isKeyDown(const int key, const char &mod) const
{
	if (al_key_down(&m_cur_key_state, key))
	{
		if (mod == MOD::ANY || mod == m_mod_flags) return true;
	}

	return false;
}

bool InputHandler::isMousePressed(const int button, const char &mod) const
{
	if (al_mouse_button_down(&m_cur_mouse_state, button) && !al_mouse_button_down(&m_prev_mouse_state, button))
	{
		if (mod == MOD::ANY || mod == m_mod_flags) return true;
	}

	return false;
}
bool InputHandler::isMouseReleased(const int button, const char &mod) const
{
	if (al_mouse_button_down(&m_prev_mouse_state, button) && !al_mouse_button_down(&m_cur_mouse_state, button))
	{
		if (mod == MOD::ANY || mod == m_mod_flags) return true;
	}

	return false;
}
bool InputHandler::isMouseDown(const int button, const char &mod) const
{
	if (al_mouse_button_down(&m_cur_mouse_state, button))
	{
		if (mod == MOD::ANY || mod == m_mod_flags) return true;
	}

	return false;
}

bool InputHandler::isMouseWheelDown(const char &mod) const
{
	if (m_cur_mouse_state.z < m_prev_mouse_state.z && (mod == MOD::ANY || mod == m_mod_flags))
	{
		return true;
	}
	else return false;
}
bool InputHandler::isMouseWheelUp(const char &mod) const
{
	if (m_cur_mouse_state.z > m_prev_mouse_state.z && (mod == MOD::ANY || mod == m_mod_flags))
	{
		return true;
	}
	else return false;
}

int InputHandler::getMouseX(void) const
{
	return m_cur_mouse_state.x;
}
int InputHandler::getMouseY(void) const
{
	return m_cur_mouse_state.y;
}
bool InputHandler::isMouseInWindow(void) const
{
	return (m_cur_mouse_state.display == al_get_current_display());
}