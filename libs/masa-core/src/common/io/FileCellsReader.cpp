/*
 * FileCellsReader.cpp
 *
 *  Created on: Apr 7, 2014
 *      Author: edans
 */

#include "FileCellsReader.hpp"


#include <stdlib.h>

FileCellsReader::FileCellsReader(FILE* file) {
	this->file = file;
}

FileCellsReader::FileCellsReader(const string path) {
	FILE* file = fopen(path.c_str(), "rb");
	if (file == NULL) {
		fprintf(stderr, "Could not open file (%s).\n", path.c_str());
		exit(1);
	}
	this->file = file;
}

FileCellsReader::~FileCellsReader() {
	close();
}

void FileCellsReader::close() {
	if (file != NULL) {
		fclose(file);
		file = NULL;
	}
}

int FileCellsReader::getType() {
	return INIT_WITH_CUSTOM_DATA;
}

int FileCellsReader::read(cell_t* buffer, int len) {
	int p = 0;
	if (buffer == NULL) {
		fseek(file, len, SEEK_CUR);
	} else {
		while (p < len) {
			int ret = fread(buffer, sizeof(cell_t), len-p, file);
			if (ret <= 0) {
				if (feof(file)) {
					fprintf(stderr, "FileCellsReader::read(): End-of-file.\n");
					return p;
				} else {
					fprintf(stderr, "FileCellsReader::read() - failed to read completely [%d/%d]\n", p, len-p);
					fprintf(stderr, "Could not load busH file (%d).\n", ret);
					exit(1);
				}
			}
			p += ret;
		}
	}
	return len;
}

void FileCellsReader::seek(int position) {
	fseek(file, position*sizeof(cell_t), SEEK_SET);
}

int FileCellsReader::getOffset() {
	return ftell(file)/sizeof(cell_t);
}
