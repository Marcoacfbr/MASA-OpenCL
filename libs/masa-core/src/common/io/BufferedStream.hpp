/*
 * BufferedStream.hpp
 *
 *  Created on: Apr 23, 2014
 *      Author: edans
 */

#ifndef BUFFEREDSTREAM_HPP_
#define BUFFEREDSTREAM_HPP_

#include "Buffer2.hpp"
#include "BufferLogger.hpp"

class BufferedStream {
public:
	BufferedStream();
	virtual ~BufferedStream();

    void setLogFile(string logFile, float interval);

protected:
	void initBuffer(int bufferLimit);
	void destroyBuffer();
	bool isBufferDestroyed();
	int readBuffer(cell_t* buf, int len);
	int writeBuffer(const cell_t* buf, int len);
    virtual void bufferLoop() = 0;

private:
    Buffer2* buffer;
    BufferLogger* logger;
    pthread_t threadId;

    static void* staticThreadFunction(void *arg);
};

#endif /* BUFFEREDSTREAM_HPP_ */
