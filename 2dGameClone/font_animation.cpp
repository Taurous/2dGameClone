#include "font_animation.hpp"



#ifdef ANIM_DEBUG
#include <iostream>
#endif

FontAnimation::FontAnimation(ALLEGRO_FONT* font, int start) : m_font(font), m_line_height(0), m_elapsed(0), m_total(0), m_finished(true), m_old_num(0), m_new_num(start), m_alpha(0), m_yoff(0)
{
	m_line_height = al_get_font_ascent(m_font);
}

FontAnimation::~FontAnimation()
{
}

void FontAnimation::start(int new_num, double total_time)
{
	if (!m_finished) return;
#ifdef ANIM_DEBUG
	std::cout << "Started animation: " << m_old_num << " --> " << new_num << std::endl;
#endif
	m_finished = false;

	m_new_num = new_num;
	m_yoff = -m_line_height;

	m_total = total_time;
	m_elapsed = 0;
	m_alpha = 0;
}

void FontAnimation::update(double delta_time)
{
	if (m_finished) return;

	if (m_elapsed >= m_total)
	{
#ifdef ANIM_DEBUG
		std::cout << "Finished animation" << std::endl;
#endif
		m_old_num = m_new_num;
		m_finished = true;
	}

	m_yoff = (1.0 - m_elapsed / m_total) * -m_line_height;
	m_alpha = m_elapsed / m_total;

	m_elapsed += delta_time;
}

void FontAnimation::draw(int x, int y, ALLEGRO_COLOR cl, int flags)
{
	if (m_finished)
	{
		al_draw_textf(m_font, cl, x, y, flags, "%i", m_new_num);
	}
	else
	{
		float r = 0;
		float g = 0;
		float b = 0;
		float a = 0;

		al_unmap_rgba_f(cl, &r, &g, &b, &a);

		ALLEGRO_COLOR c1 = al_map_rgba_f(r * m_alpha, g * m_alpha, b * m_alpha, a * m_alpha);
		double inv_alpha = 1.0 - m_alpha;
		ALLEGRO_COLOR c2 = al_map_rgba_f(r * inv_alpha, g * inv_alpha, b * inv_alpha, a * inv_alpha);

		al_draw_textf(m_font, c1, x, y + m_yoff, flags, "%i", m_new_num);
		al_draw_textf(m_font, c2, x, y + m_yoff + m_line_height, flags, "%i", m_old_num);
	}
}
