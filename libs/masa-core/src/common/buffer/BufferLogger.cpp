/*
 * BufferLogger.cpp
 *
 *  Created on: Apr 9, 2013
 *      Author: edans
 */

#include "BufferLogger.hpp"

#include <stdlib.h>
#include "../Timer.hpp"
#include "../../libmasa/libmasa.hpp"

BufferLogger::BufferLogger(string file)
{
	prev_stats.time = 0;
	prev_stats.bufferUsage = 0;
	prev_stats.blockingTime = 0;
	prev_stats.totalBytes = 0;

	this->file = fopen(file.c_str(), "wt");
	if (this->file == NULL) {
		fprintf(stderr, "Could not create buffer statistics file.\n");
		exit(1);
	}
}

BufferLogger::~BufferLogger()
{
	if (this->file != NULL) {
		fclose(this->file);
	}
}

void BufferLogger::logHeader(int bufferMax) {
	fprintf(file, "#buffer_max=%d\n", bufferMax/sizeof(cell_t));
	fflush(file);
}

void BufferLogger::logBuffer(buffer_statistics_t &curr_stats) {
	buffer_statistics_t delta_stats = curr_stats - prev_stats;
	prev_stats = curr_stats;

	float delta = 0;
	float psi = 0;

	int size = sizeof(cell_t);
	if (delta_stats.time >= 0.001) {
		delta = ((float)delta_stats.bufferUsage/size)/delta_stats.time;
	}
	if (delta_stats.time - delta_stats.blockingTime > 0.000001 || delta_stats.time - delta_stats.blockingTime < -0.000001) {
		psi = ((float)delta_stats.totalBytes/size)/delta_stats.time*delta_stats.blockingTime/(delta_stats.time-delta_stats.blockingTime);
	}

	fprintf(file, "%.2f\t%d\t%d\t%.2f\t%.2f\t%d\t%d\t%.2f\t%.3f\t%.3f\t%.3f\t%.3f\n",
			curr_stats.time,  // 1
			curr_stats.bufferUsage/size, curr_stats.totalBytes/size, curr_stats.blockingTime,    // 2 3 4
			delta_stats.time, // 5
			delta_stats.bufferUsage/size, delta_stats.totalBytes/size, delta_stats.blockingTime, // 6 7 8
			delta,  psi, // 9 10
			(delta_stats.totalBytes/size)/delta_stats.time, delta+psi); // 11 12

	fflush(file);
}
