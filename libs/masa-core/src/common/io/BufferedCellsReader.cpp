/*
 * BufferedCellReader.cpp
 *
 *  Created on: Apr 22, 2014
 *      Author: edans
 */

#include "BufferedCellsReader.hpp"

#include <stdio.h>
#include <stdlib.h>

#define DEBUG (1)

BufferedCellsReader::BufferedCellsReader(CellsReader* reader, int bufferLimit) {
    if (reader == NULL){
        printf("BufferedCellsReader::ERROR; null reader\n");
        exit(-1);
    }
	this->reader = reader;
	this->offset = 0;

	initBuffer(bufferLimit);
}

BufferedCellsReader::~BufferedCellsReader() {
	close();
}

void BufferedCellsReader::close() {
	if (DEBUG) fprintf(stderr, "BufferedCellsReader::close().\n");
	if (reader != NULL) {
		reader->close();
	}
	destroyBuffer();
	if (reader != NULL) {
		reader = NULL;
	}
	if (DEBUG) fprintf(stderr, "BufferedCellsReader::close() DONE.\n");
}

int BufferedCellsReader::getType() {
	return reader->getType();
}

int BufferedCellsReader::read(cell_t* buf, int len) {
	int ret = readBuffer(buf, len);
	offset += ret;
	return ret;
}

void BufferedCellsReader::seek(int position) {
	fprintf(stderr, "BufferedCellsReader: Cannot seek.\n");
	exit(1);
}

int BufferedCellsReader::getOffset() {
	return offset;
}

void BufferedCellsReader::bufferLoop() {
    while (!isBufferDestroyed()) {
    	cell_t cell;
    	int len = reader->read(&cell, 1);
        if (len <= 0) break;
        len = writeBuffer(&cell, 1);
        if (len <= 0) break;
    }
	if (DEBUG) printf("BufferedCellsReader::bufferLoop() - DONE\n");
}


