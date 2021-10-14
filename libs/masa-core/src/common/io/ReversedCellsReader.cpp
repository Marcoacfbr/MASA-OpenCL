/*
 * ReversedCellsReader.cpp
 *
 *  Created on: Apr 22, 2014
 *      Author: edans
 */

#include "ReversedCellsReader.hpp"

#include <stdio.h>

ReversedCellsReader::ReversedCellsReader(SeekableCellsReader* reader) {
	this->reader = reader;
	this->position = position;
}

ReversedCellsReader::~ReversedCellsReader() {
	close();
}

void ReversedCellsReader::close() {
	if (reader == NULL) {
		reader->close();
		reader = NULL;
	}
}

int ReversedCellsReader::getType() {
	return INIT_WITH_CUSTOM_DATA;
}

int ReversedCellsReader::read(cell_t* buf, int len) {
	if (len > position) {
		len = position;
	}
	position -= len;
	reader->seek(position);
	reader->read(buf, len);

	// Reverse buffer order
	for (int i=0; i<len/2; i++) {
		cell_t aux = buf[i];
		buf[i] = buf[len-1-i];
		buf[len-1-i] = aux;
	}

	return len;
}


void ReversedCellsReader::seek(int position) {
	this->position = position;
}

int ReversedCellsReader::getOffset() {
	return position;
}
