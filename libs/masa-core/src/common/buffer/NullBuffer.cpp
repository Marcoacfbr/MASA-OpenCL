/*
 * NullBuffer.cpp
 *
 *  Created on: Apr 17, 2013
 *      Author: edans
 */

#include "NullBuffer.hpp"

#include <stdio.h>
#include <string.h>

NullBuffer::NullBuffer(int size) {
	if (size <= 0) {
		this->size = 0x7FFFFFFF;
	} else {
		this->size = size;
	}

}

NullBuffer::~NullBuffer() {
    Buffer::destroy();
    //Buffer::joinThreads();
}

void NullBuffer::initAutoFlush() {
}

void NullBuffer::initAutoLoad() {
}

void NullBuffer::autoFlushThread() {
	fprintf(stderr, "Flushing data to null buffer.\n");
    char data[512];

    int pos = 0;
    while (!isDestroyed()) {
        int len = readBuffer(data, 1, sizeof(data));
        pos += len;
    }
    fprintf(stderr, "End of flush data: %d bytes\n", pos);
}

void NullBuffer::autoLoadThread() {
    char data[512];
    memset(data, 0, sizeof(data));
    int pos = 0;
	fprintf(stderr, "Load data from null buffer.\n");
    while (!isDestroyed()) {
    	int len = sizeof(data);
    	if (pos+len < size) {
    		writeBuffer(data, 1, len);
    		pos += len;
    	} else {
            writeBuffer(data, 1, size-pos);
            pos += size-pos;
            break;
    	}
    }
    fprintf(stderr, "End of input Buffer: %d bytes\n", pos);
}

void NullBuffer::destroyAutoFlush() {
}

void NullBuffer::destroyAutoLoad() {
}



