#include "stopwatch.h"
#include "fancy_time.h"

Stopwatch::Stopwatch()
{
	totalTime = 0;
}

void Stopwatch::start()
{
	startTime = FancyTime::preciseClock();
}

void Stopwatch::stop()
{
	endTime = FancyTime::preciseClock();
	totalTime += endTime - startTime;
}

unsigned int Stopwatch::getTotalTime()
{
	return totalTime;
}