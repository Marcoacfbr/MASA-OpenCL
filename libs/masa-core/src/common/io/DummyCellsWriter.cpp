/*
 * DummyReader.cpp
 *
 *  Created on: Apr 22, 2014
 *      Author: edans
 */

#include "DummyCellsWriter.hpp"

#include <string.h>

DummyCellsWriter::DummyCellsWriter() {
}

DummyCellsWriter::~DummyCellsWriter() {
	// Does Nothing
}

void DummyCellsWriter::close() {
	// Does Nothing
}

int DummyCellsWriter::write(const cell_t* buf, int len) {
	return len;
}
