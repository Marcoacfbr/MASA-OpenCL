/*
 * TeeCellsReader.cpp
 *
 *  Created on: Apr 23, 2014
 *      Author: edans
 */

#include "TeeCellsReader.hpp"

#include <cstdio>
#include <cstdlib>

#define DEBUG (0)

TeeCellsReader::TeeCellsReader(CellsReader* reader, string filename) {
	this->reader = reader;
	this->filename = filename;

	this->fileWriter = new FileCellsWriter(filename);
	this->fileReader = NULL;
}

TeeCellsReader::~TeeCellsReader() {
	close();
}

void TeeCellsReader::close() {
	if (reader != NULL) {
		reader->close();
		reader = NULL;
	}
	if (fileWriter != NULL) {
		fileWriter->close();
		delete fileWriter;
		fileWriter = NULL;
	}
	if (fileReader != NULL) {
		fileReader->close();
		delete fileReader;
		fileReader = NULL;
	}
}

int TeeCellsReader::getType() {
	return INIT_WITH_CUSTOM_DATA;
}

int TeeCellsReader::read(cell_t* buf, int len) {
	if (reader != NULL) {
		int ret = reader->read(buf, len);
		fileWriter->write(buf, ret);
		return ret;
	} else if (fileReader != NULL){
		return fileReader->read(buf, len);
	} else {
		return -1;
	}
}

void TeeCellsReader::seek(int position) {
	if (DEBUG) printf("TeeCellsReader::seek(%d)\n", position);
	if (reader != NULL) {
		reader->close();
		reader = NULL;
	}
	if (fileWriter != NULL) {
		fileWriter->close();
		fileWriter = NULL;
	    fileReader = new FileCellsReader(filename);
	} else if (fileReader == NULL) {
		fprintf(stderr, "TeeCellsReader: Cannot seek.\n");
		exit(1);
	}
	this->fileReader->seek(position);
	if (DEBUG) printf("TeeCellsReader::seek() DONE\n");
}

int TeeCellsReader::getOffset() {
	return this->fileReader->getOffset();
}
