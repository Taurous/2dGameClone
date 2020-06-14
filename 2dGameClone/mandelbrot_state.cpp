#include "mandelbrot_state.h"

#include <allegro5/allegro_primitives.h>
#include <complex>
#include <chrono>
#include <thread>

#include "state_machine.hpp"
#include "input.hpp"

using cint_pair = std::pair<std::complex<double>, int>;
using complex_array = std::vector<cint_pair>;

ALLEGRO_COLOR getColour(float v, float vmin, float vmax);
ALLEGRO_BITMAP *genMandelbrotAsync(const int bmp_size, int num_iterations, double x_loc = -0.5, double y_loc = 0.0, double scale = 1.0);
ALLEGRO_BITMAP *genMandelbrotSingle(const int bmp_size, int num_iterations, double x_loc = -0.5, double y_loc = 0.0, double scale = 1.0);
void calculateIterations(complex_array &arr, int max_iters);

MandelbrotState::MandelbrotState(StateMachine &state_machine, InputHandler &input) : AbstractState(state_machine, input)
{
	bmp = genMandelbrotAsync(4000, 80, 0.332, 0.578, 0.001);
	al_save_bitmap("E:/Desktop/mandel/gen.bmp", bmp);

	m_states.quit();
}

MandelbrotState::~MandelbrotState() 
{
	if (bmp) al_destroy_bitmap(bmp);
}

void MandelbrotState::pause()
{

}
void MandelbrotState::resume()
{

}

void MandelbrotState::handleEvents()
{

}
void MandelbrotState::update(double deltaTime)
{

}
void MandelbrotState::draw()
{

}

ALLEGRO_BITMAP *genMandelbrotAsync(const int bmp_size, int num_iterations, double x_loc, double y_loc, double scale)
{
	const int array_size = bmp_size * bmp_size;

	// Create bitmap for drawing to
	ALLEGRO_BITMAP *bmp = al_create_bitmap(bmp_size, bmp_size);
	if (!bmp)
	{
		printf("Failed to create bitmap of size %ix%i\n", bmp_size, bmp_size);
		return nullptr;
	}
	al_set_target_bitmap(bmp);

	// Calculate dimensions of Mandelbrot view, default view is -2 to 2

	double x_min = x_loc - scale;
	double y_min = y_loc - scale;
	double x_max = x_loc + scale;
	double y_max = y_loc + scale;

	printf("Range:\n\tX: %.15f to %.15f\n\tY: %.15f to %.15f\n", x_min, x_max, y_min, y_max);
	printf("Please wait...\n");

	// Create array of points to check

	auto t1 = std::chrono::high_resolution_clock::now();
	int array_quarter = (array_size) / 4;
	complex_array arr;

	for (int i = 0; i < array_size; ++i)
	{
		int pixel_y = i / bmp_size;
		int pixel_x = i % bmp_size;

		double complex_x = (double(pixel_x) / bmp_size) * (x_max - x_min) + x_min; // Make sure that the pixel_x / bmp_size results in a double
		double complex_y = (double(pixel_y) / bmp_size) * (y_max - y_min) + y_min;

		cint_pair p;
		p.first = std::complex<double>(complex_x, complex_y);
		p.second = 0;

		arr.push_back(p);
	}

	// Split array into 4 seperate arrays

	std::size_t const quarter = arr.size() / 4;

	complex_array arr1(arr.begin(), arr.begin() + quarter);
	complex_array arr2(arr.begin() + quarter, arr.begin() + (quarter * 2));
	complex_array arr3(arr.begin() + (quarter * 2), arr.begin() + (quarter * 3));
	complex_array arr4(arr.begin() + (quarter * 3), arr.end());

	arr.clear();

	// Create and launch threads

	std::thread th1(calculateIterations, std::ref(arr1), num_iterations);
	std::thread th2(calculateIterations, std::ref(arr2), num_iterations);
	std::thread th3(calculateIterations, std::ref(arr3), num_iterations);
	std::thread th4(calculateIterations, std::ref(arr4), num_iterations);

	int threads_alive = 4;
	while (threads_alive)
	{
		if (th1.joinable())
		{
			printf("Thread 1 finished\n");
			th1.join();
			--threads_alive;
		}
		if (th2.joinable())
		{
			printf("Thread 2 finished\n");
			th2.join();
			--threads_alive;
		}
		if (th3.joinable())
		{
			printf("Thread 3 finished\n");
			th3.join();
			--threads_alive;
		}
		if (th4.joinable())
		{
			printf("Thread 4 finished\n");
			th4.join();
			--threads_alive;
		}
	}

	// Draw pixels depending on results of above

	arr.reserve(arr1.size() + arr2.size() + arr3.size() + arr4.size());
	arr.insert(arr.end(), arr1.begin(), arr1.end());
	arr.insert(arr.end(), arr2.begin(), arr2.end());
	arr.insert(arr.end(), arr3.begin(), arr3.end());
	arr.insert(arr.end(), arr4.begin(), arr4.end());

	ALLEGRO_COLOR cl;

	printf("Drawing...\n");

	int index = 0;
	for (auto & c : arr)
	{
		int x = index % bmp_size;
		int y = index / bmp_size;

		if (c.second <= num_iterations) cl = getColour(c.second, 0, num_iterations);
		else cl = al_map_rgb(0, 0, 0);

		al_put_pixel(x, y, cl);

		++index;
	}

	auto t2 = std::chrono::high_resolution_clock::now();
	double duration = (std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count()) / 1000.f;
	printf("Done\n\tTime elapsed: %.3f seconds\n", duration);

	al_set_target_backbuffer(al_get_current_display());

	return bmp;
}
ALLEGRO_BITMAP *genMandelbrotSingle(const int bmp_size, int num_iterations, double x_loc, double y_loc, double scale)
{
	ALLEGRO_BITMAP *bmp = al_create_bitmap(bmp_size, bmp_size);
	if (!bmp)
	{
		printf("Failed to create bitmap of size %ix%i\n", bmp_size, bmp_size);
		return nullptr;
	}
	al_set_target_bitmap(bmp);

	// Calculate dimensions of Mandelbrot view, default view is -1 to 1

	double x_min = x_loc - scale;
	double y_min = y_loc - scale;
	double x_max = x_loc + scale;
	double y_max = y_loc + scale;

	printf("Range:\n\tX: %.15f to %.15f\n\tY: %.15f to %.15f\n", x_min, x_max, y_min, y_max);
	printf("Please wait...\n");

	auto t1 = std::chrono::high_resolution_clock::now();

	for (int i = 0; i < bmp_size * bmp_size; ++i)
	{
		int pixel_y = i / bmp_size;
		int pixel_x = i % bmp_size;

		double complex_x = (double(pixel_x) / bmp_size) * (x_max - x_min) + x_min; // Make sure that the pixel_x / bmp_size results in a double
		double complex_y = (double(pixel_y) / bmp_size) * (y_max - y_min) + y_min;

		std::complex<double> point(complex_x, complex_y);
		std::complex<double> z(0, 0);

		int count = 0;
		while (count <= num_iterations && abs(z) < 2)
		{
			z = z * z + point;
			++count;
		}

		ALLEGRO_COLOR cl;
		if (count <= num_iterations) cl = getColour(count, 0, num_iterations);
		else cl = al_map_rgb(0, 0, 0);

		al_put_pixel(pixel_x, pixel_y, cl);
	}

	auto t2 = std::chrono::high_resolution_clock::now();
	double duration = (std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count()) / 1000.f;
	printf("Done\n\tTime elapsed: %.3f seconds\n", duration);

	al_set_target_backbuffer(al_get_current_display());

	return bmp;
}

void calculateIterations(complex_array &arr, int max_iters)
{
	for (auto & c : arr)
	{
		std::complex<double> z(0, 0);
		
		int count = 0;
		while (count <= max_iters && abs(z) < 2)
		{
			z = z * z + c.first;
			++count;
		}

		c.second = count;
	}
}

ALLEGRO_COLOR getColour(float v, float vmin, float vmax)
{

	ALLEGRO_COLOR c = { 1.0, 1.0, 1.0 }; // white
	float dv;

	if (v < vmin)
		v = vmin;
	if (v > vmax)
		v = vmax;
	dv = vmax - vmin;

	if (v < (vmin + 0.25 * dv)) {
		c.g = 0;
		c.b = 4 * (v - vmin) / dv;
	}
	else if (v < (vmin + 0.5 * dv)) {
		c.g = 0;
		c.r = 1 + 4 * (vmin + 0.25 * dv - v) / dv;
	}
	else if (v < (vmin + 0.75 * dv)) {
		c.g = 4 * (v - vmin - 0.5 * dv) / dv;
		c.r = 0;
	}
	else {
		c.b = 1 + 4 * (vmin + 0.75 * dv - v) / dv;
		c.r = 0;
	}

	return(c);
}