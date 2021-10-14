/*
 * BufferedCellsWriter.cpp
 *
 *  Created on: Apr 22, 2014
 *      Author: edans
 */

#include "BufferedCellsWriter.hpp"

#include <stdio.h>
#include <stdlib.h>

#define DEBUG (0)

BufferedCellsWriter::BufferedCellsWriter(CellsWriter* writer, int bufferLimit) {
    if (writer == NULL){
        printf("BufferedCellsWriter::ERROR; null writer\n");
        exit(-1);
    }
	this->writer = writer;
	if (writer != NULL) {
		initBuffer(bufferLimit);
	}
}

BufferedCellsWriter::~BufferedCellsWriter() {
	close();
}

void BufferedCellsWriter::close() {
	printf("BufferedCellsWriter::close()\n");
	if (writer != NULL) {
		destroyBuffer();
		writer->close();
		writer = NULL;
	}
}

int BufferedCellsWriter::write(const cell_t* buf, int len) {
	return writeBuffer(buf, len);
}

void BufferedCellsWriter::bufferLoop() {

    while (!isBufferDestroyed()) {
    	cell_t cell;
        int len = readBuffer(&cell, 1);
        if (len <= 0) break;
    	len = writer->write(&cell, 1);
        if (len <= 0) break;
    }
	if (DEBUG) printf("BufferedCellsWriter::bufferLoop() - DONE\n");
}
