/*
 * FileCellsWriter.cpp
 *
 *  Created on: Apr 23, 2014
 *      Author: edans
 */

#include "FileCellsWriter.hpp"

#include <stdlib.h>

FileCellsWriter::FileCellsWriter(FILE* file) {
		this->file = file;
}

FileCellsWriter::FileCellsWriter(const string path) {
	FILE* file = fopen(path.c_str(), "wb");
	if (file == NULL) {
		fprintf(stderr, "FileCellsWriter: Could not create writer for file (%s).\n", path.c_str());
		exit(1);
	}
	this->file = file;
}

FileCellsWriter::~FileCellsWriter() {
	close();
}

void FileCellsWriter::close() {
	if (file != NULL) {
		fclose(file);
		file = NULL;
	}
}

int FileCellsWriter::write(const cell_t* buf, int len) {
	return fwrite(buf, sizeof(cell_t), len, file);
}
