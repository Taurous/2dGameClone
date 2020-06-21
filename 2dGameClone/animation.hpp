#pragma once

#include <string>

#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>

class Animation
{
public:
	Animation() : m_elapsed(0.0), m_duration(0), m_finished(true), m_x(0), m_y(0) {}
	virtual ~Animation() {}

	virtual void update(double delta_time) {}
	virtual void draw() {}

	const bool isFinished() { return m_finished; };

protected:
	double m_elapsed;
	double m_duration;;

	int m_x, m_y;

	bool m_finished;
};

class ScoreAnimation : public Animation
{
public:
	ScoreAnimation();
	~ScoreAnimation();

	void init(double duration, int init_num, int x, int y, std::string str, ALLEGRO_FONT* font, ALLEGRO_COLOR cl, int alignment);
	void start(int next_num);
	void update(double delta_time) override;
	void draw() override;

	ScoreAnimation& operator++();
	ScoreAnimation operator++(int);

private:
	ALLEGRO_FONT* m_font;
	ALLEGRO_COLOR m_cl;
	int m_line_height;
	int m_line_width;
	int m_flags;
	std::string m_str;

	int m_next_num;
	int m_prev_num;

	double m_yoff;
	double m_alpha;
};

class BitmapAnimation : public Animation
{
public:
	BitmapAnimation();
	~BitmapAnimation();

	void init(double duration, int start_x, int start_y, int target_x, int target_y, ALLEGRO_BITMAP* bmp);
	void start(bool reverse = false);
	void update(double delta_time) override;
	void draw() override;

private:
	ALLEGRO_BITMAP* m_bmp;

	bool reverse;

	int m_start_x;
	int m_start_y;
	int m_target_x;
	int m_target_y;
};