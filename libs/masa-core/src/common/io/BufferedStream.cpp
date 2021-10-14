/*
 * BufferedStream.cpp
 *
 *  Created on: Apr 23, 2014
 *      Author: edans
 */

#include "BufferedStream.hpp"

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#include "BufferLogger.hpp"

BufferedStream::BufferedStream() {
	this->buffer = NULL;
}

BufferedStream::~BufferedStream() {
	destroyBuffer();
}

void BufferedStream::destroyBuffer() {
	if (buffer != NULL) {
		buffer->waitEmptyBuffer();
		buffer->destroy();
		pthread_join(threadId, NULL);
		delete buffer;
		buffer = NULL;
	}
}

bool BufferedStream::isBufferDestroyed() {
	return buffer->isDestroyed();
}

int BufferedStream::readBuffer(cell_t* buf, int len) {
	return buffer->readBuffer(buf, len);
}

int BufferedStream::writeBuffer(const cell_t* buf, int len) {
	return buffer->writeBuffer(buf, len);
}

void BufferedStream::initBuffer(int bufferLimit) {
	this->buffer = new Buffer2(bufferLimit);

    int rc = pthread_create(&threadId, NULL, staticThreadFunction, (void *)this);
    if (rc){
        printf("ERROR; return code from pthread_create() is %d\n", rc);
        exit(-1);
    }
}

void* BufferedStream::staticThreadFunction(void* arg) {
	BufferedStream* _this = (BufferedStream*)arg;
    _this->bufferLoop();
    return NULL;
}

void BufferedStream::setLogFile(string logFile, float interval) {
	buffer->setLogFile(logFile, interval);
}
