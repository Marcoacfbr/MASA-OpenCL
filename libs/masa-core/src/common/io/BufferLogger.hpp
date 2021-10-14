/*
 * BufferLogger.hpp
 *
 *  Created on: Apr 9, 2013
 *      Author: edans
 */

#ifndef BUFFERLOGGER_HPP_
#define BUFFERLOGGER_HPP_

#include <stdio.h>
#include <string>
using namespace std;

#include "Buffer2.hpp"

class BufferLogger {
public:
	BufferLogger(string file);
	virtual ~BufferLogger();

	void logHeader(int bufferMax);
	void logBuffer(buffer2_statistics_t &curr_stats);
private:
	//Buffer* buffer;
	FILE* file;

	/*float t0;
	int buffer_0;
	int cells_0;
	float wait_0;*/
	buffer2_statistics_t prev_stats;

};

#endif /* BUFFERLOGGER_HPP_ */
