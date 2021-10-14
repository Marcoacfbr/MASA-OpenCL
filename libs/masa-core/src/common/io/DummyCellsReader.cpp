/*
 * DummyReader.cpp
 *
 *  Created on: Apr 22, 2014
 *      Author: edans
 */

#include "DummyCellsReader.hpp"

#include <string.h>

DummyCellsReader::DummyCellsReader(int size) {
	if (size <= 0) {
		this->size = 0x7FFFFFFF;
	} else {
		this->size = size;
	}
	this->position = 0;
}

DummyCellsReader::~DummyCellsReader() {
	// Does Nothing
}

void DummyCellsReader::close() {
	// Does Nothing
}

int DummyCellsReader::getType() {
	return INIT_WITH_CUSTOM_DATA;
}

int DummyCellsReader::read(cell_t* buf, int len) {
	if (len > size-position) {
		len = size-position;
	}
	memset(buf, 0, sizeof(cell_t)*len);
	this->position += len;
	return len;
}

void DummyCellsReader::seek(int position) {
	this->position = position;
}

int DummyCellsReader::getOffset() {
	return position;
}
