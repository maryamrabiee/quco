#ifndef FANCY_TIME_H
#define FANCY_TIME_H

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#ifndef _MSC_VER
#include <sys/time.h>
#endif
#ifdef _WIN32
#include <windows.h>
#endif

class FancyTime
{
	private:
		#ifdef __APPLE__
			#define CLOCK_MONOTONIC 0
			static int clock_gettime(int foo, struct timespec *ts)
			{
				struct timeval tv;
 
				gettimeofday(&tv, NULL);
				ts->tv_sec = tv.tv_sec;
				ts->tv_nsec = tv.tv_usec * 1000;
				return (0);
			}
		#endif

		#ifdef _WIN32
			#define CLOCK_MONOTONIC 0

			struct timespec
			{
				time_t tv_sec;
				long tv_nsec;
			};

			static int clock_gettime(int X, struct timespec *tv)
			{
				LARGE_INTEGER           t;
				double                  microseconds;
				static double           frequencyToMicroseconds;
				static bool             initialized = false;

				if (!initialized)
				{
					LARGE_INTEGER performanceFrequency;
					initialized = true;
					QueryPerformanceFrequency(&performanceFrequency);
					frequencyToMicroseconds = (double) performanceFrequency.QuadPart / 1000000.;
				}
				QueryPerformanceCounter(&t);

				microseconds = (double)t.QuadPart / frequencyToMicroseconds;
				t.QuadPart = (LONGLONG) microseconds;
				tv->tv_sec = t.QuadPart / 1000000;
				tv->tv_nsec = (t.QuadPart % 1000000) * 1000;
				return 0;
			}
		#endif

	public:
		static unsigned int preciseClock()
		{
			struct timespec ts;

			clock_gettime(CLOCK_MONOTONIC, &ts);
			return (unsigned int) (ts.tv_sec * 10000 + ts.tv_nsec / 100000);
		}
};

#endif