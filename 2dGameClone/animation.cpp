#include "animation.hpp"

ScoreAnimation::ScoreAnimation() :
	m_prev_num(0), m_next_num(0), m_str(),
	m_font(nullptr), m_line_height(0), m_line_width(0), m_flags(0),
	m_alpha(1), m_yoff(0), m_cl()
{}

ScoreAnimation::~ScoreAnimation()
{}

void ScoreAnimation::init(double duration, int init_num, int x, int y, std::string str, ALLEGRO_FONT* font, ALLEGRO_COLOR cl, int alignment)
{
	m_duration = duration;
	m_x = x;
	m_y = y;
	m_prev_num = init_num;
	m_next_num = init_num;

	m_str = str;
	m_cl = cl;
	m_flags = alignment;

	m_font = font;
	m_line_height = al_get_font_ascent(m_font);
	m_line_width = al_get_text_width(m_font, m_str.c_str());
	m_yoff = 0;
}

void ScoreAnimation::start(int next_num)
{
	m_finished = false;
	m_next_num = next_num;

	m_alpha = 0;
	m_yoff = -m_line_height;
	m_elapsed = 0;
}

void ScoreAnimation::update(double delta_time)
{
	if (m_finished) return;

	if (m_elapsed >= m_duration)
	{
		m_finished = true;
		m_yoff = 0;
		m_alpha = 1.0;
	}

	m_yoff = (1.0 - m_elapsed / m_duration) * -m_line_height;
	m_alpha = m_elapsed / m_duration;

	m_elapsed += delta_time;
}

void ScoreAnimation::draw()
{
	float r = 0;
	float g = 0;
	float b = 0;
	float a = 0;

	al_unmap_rgba_f(m_cl, &r, &g, &b, &a);

	ALLEGRO_COLOR new_col = al_map_rgba_f(r * m_alpha, g * m_alpha, b * m_alpha, a * m_alpha);

	double inv_alpha = 1.0 - m_alpha;
	ALLEGRO_COLOR old_col = al_map_rgba_f(r * inv_alpha, g * inv_alpha, b * inv_alpha, a * inv_alpha);

	int offx = m_flags == ALLEGRO_ALIGN_LEFT ? m_line_width : -m_line_width;

	al_draw_text(m_font, m_cl, m_x, m_y, m_flags, m_str.c_str());
	al_draw_textf(m_font, new_col, m_x + offx, m_y + m_yoff, m_flags, "%i", m_next_num);
	al_draw_textf(m_font, old_col, m_x + offx, m_y + m_yoff + m_line_height, m_flags, "%i", m_prev_num);
}

ScoreAnimation& ScoreAnimation::operator++()
{
	this->start(m_next_num+1);
	return *this;
}

ScoreAnimation ScoreAnimation::operator++(int)
{
	ScoreAnimation temp = *this;
	++* this;
	return temp;
}

BitmapAnimation::BitmapAnimation() : m_bmp(nullptr), m_target_x(0), m_target_y(0), m_start_x(0), m_start_y(0), reverse(false)
{}

BitmapAnimation::~BitmapAnimation()
{}

void BitmapAnimation::init(double duration, int start_x, int start_y, int target_x, int target_y, ALLEGRO_BITMAP* bmp)
{
	m_duration = duration;

	m_x = start_x;
	m_y = start_y;

	m_start_x = start_x;
	m_start_y = start_y;

	m_target_x = target_x;
	m_target_y = target_y;

	m_bmp = bmp;
}

void BitmapAnimation::start(bool reverse)
{
	if (!m_finished) return;
	printf("Started anim...\n");

	m_finished = false;
	m_elapsed = 0.0;

	if (!reverse)
	{
		m_x = m_start_x;
		m_y = m_start_y;
	}
	else
	{
		m_x = m_target_x;
		m_y = m_target_y;
	}
}

void BitmapAnimation::update(double delta_time)
{
	if (m_finished) return;

	double t1 = 1.0 - m_elapsed / m_duration;
	double t2 = m_elapsed / m_duration;

	if (!reverse)
	{
		m_x = t1 * m_start_x + t2 * m_target_x;
		m_y = t1 * m_start_y + t2 * m_target_y;
	}
	else
	{
		m_x = t1 * m_target_x + t2 * m_start_x;
		m_y = t1 * m_target_y + t2 * m_start_y;
	}

	if (m_elapsed >= m_duration)
	{
		printf("Finished anim...\n");

		if (!reverse)
		{
			m_x = m_target_x;
			m_y = m_target_y;
		}
		else
		{
			m_x = m_start_x;
			m_y = m_start_y;
		}

		m_finished = true;
		reverse = false;
	}

	m_elapsed += delta_time;
}

void BitmapAnimation::draw()
{
	al_draw_bitmap(m_bmp, m_x, m_y, 0);
}
