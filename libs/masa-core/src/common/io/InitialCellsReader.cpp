/*
 * InitialCellsReader.cpp
 *
 *  Created on: Apr 6, 2014
 *      Author: edans
 */

#include "InitialCellsReader.hpp"
#include "../../libmasa/IManager.hpp"
#include <stdio.h>

#define DEBUG (0)

InitialCellsReader::InitialCellsReader(int startOffset) {
	this->type = INIT_WITH_ZEROES;
	this->startOffset = startOffset;
	initialize();
}

InitialCellsReader::InitialCellsReader(const int gapOpen, const int gapExt, int startOffset) {
	this->gapOpen = gapOpen;
	this->gapExt = gapExt;
	if (gapOpen == 0 && gapExt == 0) {
		this->type = INIT_WITH_ZEROES;
	} else if (gapOpen == 0) {
		this->type = INIT_WITH_GAPS_OPENED;
	} else {
		this->type = INIT_WITH_GAPS;
	}
	this->startOffset = startOffset;
	initialize();
}

InitialCellsReader::~InitialCellsReader() {
	// Does nothing
}

void InitialCellsReader::close() {
	// Does nothing
}

void InitialCellsReader::seek(int position) {
	this->position = startOffset + position;
}

int InitialCellsReader::getOffset() {
	return position - startOffset;
}

InitialCellsReader* InitialCellsReader::clone(int offset) {
	if (type == INIT_WITH_GAPS || type == INIT_WITH_GAPS_OPENED) {
		return new InitialCellsReader(gapOpen, gapExt, this->startOffset + offset);
	} else {
		return new InitialCellsReader(this->startOffset + offset);
	}
}

int InitialCellsReader::getStartOffset() {
	return startOffset;
}

void InitialCellsReader::initialize() {
	this->position = startOffset;
}

int InitialCellsReader::getType() {
	return this->type;
}

int InitialCellsReader::read(cell_t* buffer, int len) {
	if (DEBUG) fprintf(stdout, "InitialCellsReader::read(%p, %d) - pos: %d\n", buffer, len, position);
	if (buffer != NULL) {
		if (type == INIT_WITH_GAPS || type == INIT_WITH_GAPS_OPENED) {
			// Pre-defined initialization functions
			int k = 0;
			if (position == 0) {
				buffer[0].h = 0;
				buffer[0].f = -INF;
				k++;
			}
			for (; k < len; k++) {
				buffer[k].h = -gapExt*(position+k) - gapOpen;
				buffer[k].e = -INF;
			}
		} else if (type == INIT_WITH_ZEROES) {
			for (int k = 0; k < len; k++) {
				buffer[k].h = 0;
				buffer[k].e = -INF;
			}
		}
	}
	position += len;
	return len;
}

