#pragma once

#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>

class FontAnimation
{
public:
	FontAnimation(ALLEGRO_FONT* font, int start);
	~FontAnimation();

	void start(int new_num, double total_time);
	void update(double delta_time);
	void draw(int x, int y, ALLEGRO_COLOR cl, int flags);

	const bool isFinished() { return m_finished; };

private:
	ALLEGRO_FONT* m_font;
	int m_line_height;

	int m_new_num;
	int m_old_num;

	double m_elapsed;
	double m_total;
	double m_yoff;
	double m_alpha;

	bool m_finished;
};