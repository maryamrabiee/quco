#ifndef STOPWATCH_H
#define STOPWATCH_H

class Stopwatch
{
	private:
		unsigned int totalTime, startTime, endTime;
		
	public:
		Stopwatch();
		void start();
		void stop();
		unsigned int getTotalTime();
};

#endif