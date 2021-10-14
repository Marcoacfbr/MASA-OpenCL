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

#ifndef BUFFER_H
#define BUFFER_H

#include <pthread.h>
#include <string>
using namespace std;

#include "../io/CellsReader.hpp"
#include "../io/CellsWriter.hpp"

struct buffer_statistics_t {
	float time;
	int bufferUsage;
	int totalBytes;
	float blockingTime;

	const buffer_statistics_t operator-(const buffer_statistics_t &other) const {
		buffer_statistics_t ret;
		ret.time = this->time - other.time;
		ret.bufferUsage = this->bufferUsage - other.bufferUsage;
		ret.totalBytes = this->totalBytes - other.totalBytes;
		ret.blockingTime = this->blockingTime - other.blockingTime;
		return ret;
	}
};



class Buffer : public CellsReader, public CellsWriter
{
public:
	Buffer();
	virtual ~Buffer();
	
	virtual int read(cell_t* data, int nmemb);
	virtual int write(const cell_t* data, int nmemb);
	virtual int getType();

	int readBuffer(char* data, int size, int nmemb);
	int writeBuffer(const char* data, int size, int nmemb);
	void waitEmptyBuffer();
	
	void destroy();
	bool isDestroyed();

    virtual void autoFlushThread() = 0;
    virtual void autoLoadThread() = 0;
    
    virtual void initAutoFlush() = 0;
    virtual void initAutoLoad() = 0;

    /**
     * This method is called prior to the auto-load thread destruction.
     * See the Buffer::destroyAutoLoad() method description.
     */
    virtual void destroyAutoFlush() = 0;

    /**
     * This method is called prior to the auto-load thread destruction.
     * For example, if the auto-load thread is waiting for a socket input,
     * then the destroyAutoLoad must destroy the socket in order to wake
     * the thread execution. Note that during and after this method is called,
     * the Buffer::isDestroyed() method will always return true.
     */
    virtual void destroyAutoLoad() = 0;
    
	void autoFlush();
	void autoLoad();
	
	/**
	 * Use this method only for statistics purpose, since the returned value
	 * is volatile and not synchronized (without mutex).
	 * @return the number of bytes in the buffer.
	 */
	int getUsage();

	/**
	 * Returns the maximum capacity in bytes of the buffer.
	 * @return The size of the buffer in bytes.
	 */
	int getCapacity();

	/**
	 * Returns the number of milliseconds that was blocked waiting for data.
	 * @return blocking time in readBuffer method.
	 */
	float getBlockingReadTime();

	/**
	 * Returns the number of milliseconds that was blocked waiting for space.
	 * @return blocking time in writeBuffer method.
	 */
	float getBlockingWriteTime();

	/**
	 * Returns the amount of bytes written in the buffer since its creation.
	 * @return the number of bytes written.
	 */
	int getBytesWritten();

	/**
	 * Returns the amount of bytes read from the buffer since its creation.
	 * @return the number of bytes read.
	 */
	int getBytesRead();

	/**
	 * Returns a struct containing statistics of the buffer.
	 * @return statistics of the buffer.
	 */
	buffer_statistics_t getStatistics();

	/**
	 * Defines a logfile to receive the buffer statistics in each
	 * interval period.
	 *
	 * @param logFile the filename of the log file.
	 * @param interval The period in seconds to log the statistics.
	 */
    void setLogFile(string logFile, int interval);

	private:
		float blockingReadTime;
		float blockingWriteTime;
		int volatileUsage;
		int bytesRead;
		int bytesWritten;

        float tempBlockingWriteTime;
        float tempBlockingReadTime;


		pthread_mutex_t mutex;
		pthread_cond_t notFullCond;
		pthread_cond_t notEmptyCond;
		pthread_cond_t emptyCond;
		pthread_cond_t loggerCond;
		
		bool destroyed;
		
		char* buffer;
		int buffer_size;
		int buffer_start;
		int buffer_end;
		int buffer_max;

        pthread_t threadId;
        bool inputBuffer;
        
        pthread_t loggerThread;
        string logFile;
        bool isLogging;
        int logInterval; // in seconds

		int circularLoad(char *dst, int len);
		int circularStore(const char *src, int len);
		int sizeUsed();
		int sizeAvailable();

        static void* staticFlushThread(void *arg);
        static void* staticLoadThread(void *arg);
        static void* staticLogThread(void *arg);

        void createAutoLoadThread();
        void createAutoFlushThread();
	    void logThread();

		void joinThreads();
};

#endif // BUFFER_H
