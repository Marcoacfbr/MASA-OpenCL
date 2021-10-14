/*******************************************************************************
 *
 * Copyright (c) 2010, 2013   Edans Sandes
 *
 * This file is part of CUDAlign.
 * 
 * CUDAlign is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * CUDAlign is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with CUDAlign.  If not, see <http://www.gnu.org/licenses/>.
 *
 ******************************************************************************/

#include "Buffer.hpp"

#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
//#include <sys/time.h>

#include "../Timer.hpp"
#include <time.h>
#include <unistd.h>
#include "BufferLogger.hpp"

Buffer::Buffer() {
    buffer_max = 8*1024*1024;
    buffer_size = buffer_max+5;
    buffer_start = 0;
    buffer_end = 0;
    buffer = (char*)malloc(buffer_size);
    destroyed = false;
	isLogging = false;
    
    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&notFullCond, NULL);
    pthread_cond_init(&notEmptyCond, NULL);
    pthread_cond_init(&emptyCond, NULL);
    pthread_cond_init(&loggerCond, NULL);

	blockingReadTime = 0;
	blockingWriteTime = 0;
	volatileUsage = 0;
	bytesRead = 0;
	bytesWritten = 0;

	tempBlockingReadTime = -1;
	tempBlockingWriteTime = -1;

	inputBuffer = false;

    /*mutex = PTHREAD_MUTEX_INITIALIZER;
    notFullCond = PTHREAD_COND_INITIALIZER;
    notEmptyCond = PTHREAD_COND_INITIALIZER;
    emptyCond = PTHREAD_COND_INITIALIZER;*/
}

Buffer::~Buffer() {
    fprintf(stderr, "Destruct Buffer...\n");
    free(buffer);
}

void Buffer::destroy() {
    pthread_mutex_lock(&mutex);

    destroyed = true;
    pthread_cond_signal(&notFullCond);
    pthread_cond_signal(&notEmptyCond);
    pthread_cond_signal(&emptyCond);
    pthread_cond_signal(&loggerCond);

    pthread_mutex_unlock(&mutex);


    if (inputBuffer) {
    	destroyAutoLoad();
    } else {
    	destroyAutoFlush();
    }

    joinThreads();
}

int Buffer::getType() {
	return INIT_WITH_CUSTOM_DATA;
}

void Buffer::joinThreads() {
    pthread_join(threadId, NULL);
    if (isLogging) {
    	isLogging = false;
    	pthread_join(loggerThread, NULL);
    }
}

bool Buffer::isDestroyed() {
    return destroyed;
}

void Buffer::waitEmptyBuffer() {
    pthread_mutex_lock(&mutex);
	while (sizeUsed() > 0 && !destroyed) {
		fprintf (stderr, "Waiting empty buffer: %d\n", sizeUsed());
        pthread_cond_wait (&emptyCond, &mutex);		
	}
    pthread_mutex_unlock(&mutex);
}

int Buffer::circularLoad(char *dst, int len) {
    
    if (buffer_start+len < buffer_size) {
        if (dst != NULL) {
        	memcpy(dst, buffer+buffer_start, len);
        }
        buffer_start += len;
    } else {
    	if (dst != NULL) {
    		memcpy(dst, buffer+buffer_start, buffer_size-buffer_start);
    		memcpy(dst+(buffer_size-buffer_start), buffer, len-(buffer_size-buffer_start));
    	}
        buffer_start = len-(buffer_size-buffer_start);
    }
    if (sizeUsed() > 0) {
        pthread_cond_signal(&notFullCond);
    } else {
        pthread_cond_signal(&emptyCond);
	}
    return len;
}

int Buffer::circularStore(const char *src, int len) {
    if (buffer_end+len < buffer_size) {
        memcpy(buffer+buffer_end, src, len);
        buffer_end += len;
    } else {
        memcpy(buffer+buffer_end, src, buffer_size-buffer_end);
        memcpy(buffer, src+(buffer_size-buffer_end), len - (buffer_size-buffer_end));
        buffer_end = len - (buffer_size-buffer_end);
    }
    if (sizeUsed() > 0) {
        pthread_cond_signal(&notEmptyCond);
    }
    return len;
}

int Buffer::sizeAvailable() {
    return buffer_max - sizeUsed();
}

int Buffer::sizeUsed() {
    if (buffer_end >= buffer_start) {
        return buffer_end - buffer_start;
    } else {
        return buffer_end - buffer_start + buffer_size;
    }
}

int Buffer::read(cell_t* data, int nmemb) {
	return readBuffer((char*)data, sizeof(cell_t), nmemb);
}

int Buffer::readBuffer(char* data, int size, int nmemb)
{
    pthread_mutex_lock(&mutex);
    int size_total = size*nmemb;
    int size_left = size_total;
    while ((sizeUsed() < size_left) && !destroyed) {
        size_left -= circularLoad((char*)data+(size_total-size_left), sizeUsed());
        float t0 = Timer::getGlobalTime();
        tempBlockingReadTime = t0;
        pthread_cond_wait (&notEmptyCond, &mutex);
        tempBlockingReadTime = -1;
        float t1 = Timer::getGlobalTime();
        blockingReadTime += (t1-t0);
    }
    if (!destroyed) {
        size_left -= circularLoad((char*)data+(size_total-size_left), size_left);
    }
    volatileUsage = sizeUsed();
    if (bytesRead == 0 && inputBuffer) {
    	pthread_cond_signal(&loggerCond);
    }
    bytesRead += (size_total-size_left);
    pthread_mutex_unlock(&mutex);
    
    if (size_left != 0) fprintf(stderr, "readBuffer: diff len: %d %d\n", size_total-size_left, size_total);
    
    return size_total-size_left;
}

int Buffer::write(const cell_t* data, int nmemb) {
	return writeBuffer((char*)data, sizeof(cell_t), nmemb);
}

int Buffer::writeBuffer(const char* data, int size, int nmemb)
{
    pthread_mutex_lock(&mutex);
    int size_total = size*nmemb;
    int size_left = size_total;
    while ((sizeAvailable() < size_left) && !destroyed) {
        size_left -= circularStore(data+(size_total-size_left), sizeAvailable());
        float t0 = Timer::getGlobalTime();
        tempBlockingWriteTime = t0;
        pthread_cond_wait (&notFullCond, &mutex);
        tempBlockingWriteTime = -1;
        float t1 = Timer::getGlobalTime();
        blockingWriteTime += (t1-t0);
    }
    if (!destroyed) {
        size_left -= circularStore(data+(size_total-size_left), size_left);
    }
    volatileUsage = sizeUsed();
    if (bytesWritten == 0 && !inputBuffer) {
    	pthread_cond_signal(&loggerCond);
    }
    bytesWritten += (size_total-size_left);
    pthread_mutex_unlock(&mutex);
    if (size_left != 0) fprintf(stderr, "writeBuffer: diff len: %d %d\n", size_total-size_left, size_total);

    return size_total-size_left;
}

void Buffer::autoFlush() {
    initAutoFlush();
    createAutoFlushThread();
}

void Buffer::autoLoad() {
    initAutoLoad();
    createAutoLoadThread();
}



void Buffer::createAutoLoadThread() {
	this->inputBuffer = true;
    int rc = pthread_create(&threadId, NULL, staticLoadThread, (void *)this);
    if (rc){
        printf("ERROR; return code from pthread_create() is %d\n", rc);
        exit(-1);
    }
}


void Buffer::createAutoFlushThread() {
	this->inputBuffer = false;
    int rc = pthread_create(&threadId, NULL, staticFlushThread, (void *)this);
    if (rc){
        printf("ERROR; return code from pthread_create() is %d\n", rc);
        exit(-1);
    }
}

void *Buffer::staticFlushThread(void *arg) {
    Buffer* buffer = (Buffer*)arg;
    buffer->autoFlushThread();
    return NULL;
}

void *Buffer::staticLoadThread(void *arg) {
    Buffer* buffer = (Buffer*)arg;
    buffer->autoLoadThread();
    return NULL;
}

void *Buffer::staticLogThread(void *arg) {
    Buffer* buffer = (Buffer*)arg;
    buffer->logThread();
    return NULL;
}

int Buffer::getUsage() {
	return volatileUsage;
}

int Buffer::getCapacity() {
	return buffer_max;
}

float Buffer::getBlockingReadTime() {
	return blockingReadTime;
}

float Buffer::getBlockingWriteTime() {
	return blockingWriteTime;
}

int Buffer::getBytesWritten() {
	return bytesWritten;
}

int Buffer::getBytesRead() {
	return bytesRead;
}

buffer_statistics_t Buffer::getStatistics() {
	buffer_statistics_t stats;
	stats.time = Timer::getGlobalTime();

    pthread_mutex_lock(&mutex);
	if (inputBuffer) {
		stats.blockingTime = blockingReadTime;
		stats.bufferUsage = volatileUsage;
		stats.totalBytes = bytesRead;
		if (tempBlockingReadTime != -1) {
			stats.blockingTime += stats.time - tempBlockingReadTime;
		}
	} else {
		stats.blockingTime = blockingWriteTime;
		stats.bufferUsage = volatileUsage;
		stats.totalBytes = bytesWritten;
		if (tempBlockingWriteTime != -1) {
			stats.blockingTime += stats.time - tempBlockingWriteTime;
		}
	}
    pthread_mutex_unlock(&mutex);

	return stats;
}

void Buffer::logThread() {
	BufferLogger* logger = new BufferLogger(logFile);
	logger->logHeader(buffer_max);
	while (!isDestroyed()) {
	    timeval event;
	    gettimeofday(&event, NULL);

		struct timespec time;
		time.tv_sec = event.tv_sec + logInterval;
		time.tv_nsec = event.tv_usec*1000;

	    pthread_mutex_lock(&mutex);
        pthread_cond_timedwait(&loggerCond, &mutex, &time);
	    pthread_mutex_unlock(&mutex);

		buffer_statistics_t stats;
		stats = getStatistics();

		logger->logBuffer(stats);
	}
	delete logger;
}

void Buffer::setLogFile(string logFile, int interval) {
	this->logFile = logFile;
	this->logInterval = interval;
	if (this->logInterval < 1) {
		this->logInterval = 1; // minimum interval.
	}
	this->isLogging = true;
    int rc = pthread_create(&loggerThread, NULL, staticLogThread, (void *)this);
    if (rc){
        fprintf(stderr, "setLogFile ERROR; return code from pthread_create() is %d\n", rc);
        exit(-1);
    }
}

