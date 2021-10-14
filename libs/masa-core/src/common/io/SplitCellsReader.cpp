/*
 * SplitReader.cpp
 *
 *  Created on: May 11, 2014
 *      Author: edans
 */

#include "SplitCellsReader.hpp"

SplitCellsReader::SplitCellsReader(SeekableCellsReader* reader, int pos0, bool closeAfterUse) {
	this->reader = reader;
	this->pos0 = pos0;
	this->closeAfterUse = closeAfterUse;
}

SplitCellsReader::~SplitCellsReader() {
	close();
}

void SplitCellsReader::seek(int offset) {
	reader->seek(pos0 + offset);
}

int SplitCellsReader::getOffset() {
	return reader->getOffset() - pos0;
}

void SplitCellsReader::close() {
	if (closeAfterUse) {
		reader->close();
	}
}

int SplitCellsReader::getType() {
	return reader->getType();
}

int SplitCellsReader::read(cell_t* buf, int len) {
	reader->read(buf, len);
}
