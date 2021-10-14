/*
 * RecurrentTimer.hpp
 *
 *  Created on: May 11, 2013
 *      Author: edans
 */

#ifndef RECURRENTTIMER_HPP_
#define RECURRENTTIMER_HPP_

#include <pthread.h>
#include <sys/time.h>

class RecurrentTimer {
public:
	RecurrentTimer(void (*routine)(float));
	virtual ~RecurrentTimer();
	void start(float interval);
	void stop();
	void logNow();

private:
	static void *staticFunctionThread(void *arg);
	void executeLoop();

	void (*routine)(float);
	int interval;
	bool active;

	pthread_t thread;
	pthread_mutex_t mutex;
	pthread_cond_t condition;

	float getElapsedTime(timeval *end_time, timeval *start_time);
};

#endif /* RECURRENTTIMER_HPP_ */
