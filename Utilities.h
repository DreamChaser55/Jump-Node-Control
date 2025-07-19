#pragma once

#include <chrono>
#include <string>
#include <random>
#include <vector>
#include <boost/circular_buffer.hpp>
#include <numeric>

#include <D:\Programming\CPP\Sources\fmt-master11.1.1\include\fmt\core.h>
#include <D:\Programming\CPP\Sources\fmt-master11.1.1\include\fmt\ranges.h>
#include <D:\Programming\CPP\Sources\fmt-master11.1.1\include\fmt\format.h>

#ifdef BUILD_CONFIG_DEBUG
	#pragma comment (lib, "D:\\Programming\\CPP\\VS_Solutions\\fmt11.1.1\\Debug\\fmtd.lib")
#elif BUILD_CONFIG_PROFILE
	#pragma comment (lib, "D:\\Programming\\CPP\\VS_Solutions\\fmt11.1.1\\RelWithDebInfo\\fmt.lib")
#elif BUILD_CONFIG_RELEASE
	#pragma comment (lib, "D:\\Programming\\CPP\\VS_Solutions\\fmt11.1.1\\Release\\fmt.lib")
#endif

#define NEWL std::string("\n")

using std::string;
using std::vector;
using std::chrono::high_resolution_clock;
using std::chrono::nanoseconds;
using std::chrono::microseconds;
using std::chrono::duration_cast;
using fmt::print;
using fmt::format;
using fmt::runtime;

class Benchmark_Timer
{
private:
	high_resolution_clock clock;
	std::chrono::time_point<high_resolution_clock> benchmark_start_time;

public:
	void benchmark_start()
	{
		benchmark_start_time = clock.now();
	}

	void benchmark_end(string label) const
	{
		auto elapsed = duration_cast<microseconds>(clock.now() - benchmark_start_time);
		print(runtime(label + " - time (us): {}" + NEWL), elapsed.count());
	}
};

class Stopwatch
{
private:
	high_resolution_clock clock;
	std::chrono::time_point<high_resolution_clock> start_time;

public:
	Stopwatch() : start_time(clock.now())
	{}

	nanoseconds get_elapsed_time() const
	{
		return (clock.now() - start_time);
	}

	nanoseconds restart()
	{
		std::chrono::time_point<high_resolution_clock> now = clock.now();
		nanoseconds elapsed = now - start_time;
		start_time = now;

		return elapsed;
	}
};

class Moving_average_timer
{
public:
	Stopwatch stopwatch;
	boost::circular_buffer<nanoseconds> buffer{100};

	void start()
	{
		stopwatch.restart();
	}

	void end()
	{
		buffer.push_back(stopwatch.restart());
	}

	nanoseconds get_moving_average()
	{
		nanoseconds sum = std::accumulate(buffer.begin(), buffer.end(), nanoseconds());
		return sum / buffer.size();
	}
};

string bool_to_string(bool bool_arg)
{
	if (bool_arg)
	{
		return "true";
	}
	else
	{
		return "false";
	}
}

//Math

constexpr double PI = 3.1415926535897932384;
constexpr double TAU = 6.2831853071795864769;

std::minstd_rand minstd_generator;

//including min and max
template <typename T>
T get_random_int_in_range(const T min, const T max)
{
	std::uniform_int_distribution<T> distribution(min, max);
	return distribution(minstd_generator);
}

//including min, excluding max
template <typename T>
T get_random_real_in_range(const T min, const T max)
{
	std::uniform_real_distribution<T> distribution(min, max);
	return distribution(minstd_generator);
}

//angles in degrees, from 0 to 360
template <typename T>
T angle_difference_deg(const T ang1, const T ang2)
{
	T diff = ang1 - ang2;
	if (diff > 180)
	{
		diff -= 360;
	}
	if (diff < -180)
	{
		diff += 360;
	}
	return diff;
}

//restricts an angle in radians to the interval from 0 to TAU
double normalize_angle_rad(double angle)
{
	double int_part;
	double fract_part = modf(angle / TAU, &int_part);

	double effective_rotation = fract_part * TAU;

	if (effective_rotation >= 0)
	{
		return effective_rotation;
	}
	else //below 0
	{
		return TAU + effective_rotation;
	}
}

//restricts an angle in degrees to the interval from 0 to 360
double normalize_angle_deg(double angle)
{
	double int_part;
	double fract_part = modf(angle / 360.0, &int_part);

	double effective_rotation = fract_part * 360.0;

	if (effective_rotation >= 0)
	{
		return effective_rotation;
	}
	else //below 0
	{
		return 360.0 + effective_rotation;
	}
}

//angles in radians, from 0 to TAU
template <typename T>
T angle_difference_rad(const T ang1, const T ang2)
{
	T diff = ang1 - ang2;
	if (diff > PI)
	{
		diff -= TAU;
	}
	if (diff < -PI)
	{
		diff += TAU;
	}
	return diff;
}

constexpr double rad_to_deg(const double rad)
{
	return rad * (180.0 / PI);
}

constexpr double deg_to_rad(const double deg)
{
	return deg / (180.0 / PI);
}

// Custom assert providing more information than is typically given in C's assert function:
#ifndef NDEBUG
#define custom_assert(condition, message) \
		{ \
			if (not(condition)) \
			{ \
				std::cout << std::endl << "Assertion `" #condition "` failed in " << __FILE__ << ", line " << __LINE__ << ". " << std::endl << message << std::endl; \
				std::cout.flush(); \
			} \
		}
#else
#define custom_assert(condition, message) { }
#endif

// Like a reverse-assert, but doesn't get turned off in non-debug compile modes:
#define custom_fail_if(condition, message) \
	{ \
		if (condition) \
		{ \
			std::cout << std::endl << "Aborting because `" #condition "` in " << __FILE__ << ", line " << __LINE__ << ". " << std::endl << message << std::endl; \
			std::cout.flush(); \
		} \
	}