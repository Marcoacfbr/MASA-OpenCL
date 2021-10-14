/*
 * URLCellsReader.cpp
 *
 *  Created on: Apr 22, 2014
 *      Author: edans
 */

#include "URLCellsReader.hpp"

#include "FileCellsReader.hpp"
#include "DummyCellsReader.hpp"
#include "SocketCellsReader.hpp"

#include <stdio.h>
#include <stdlib.h>

URLCellsReader::URLCellsReader(string url) {
	int pos1 = url.find_first_of("://");
	if (pos1 == -1) {
		fprintf(stderr, "URLCellsReader: Wrong URL format: %s\n", url.c_str());
		exit(1);
	}
	string type = url.substr(0, pos1);
	string param = url.substr(pos1+3);


	fprintf(stderr, "%s:   %s - %s\n", url.c_str(), type.c_str(), param.c_str());
	if (type == "socket") {
		int port;
		string hostname;
		int pos2 = param.find_first_of(":");
		if (pos2 > 0) {
			port = atoi(param.substr(pos2+1).c_str());
			hostname = param.substr(0, pos2);
		} else {
			hostname = param;
		}
		reader = new SocketCellsReader(hostname, port);
	} else if (type == "file") {
		reader = new FileCellsReader(param);
	} else if (type == "null") {
		int size = atoi(param.c_str());
		reader = new DummyCellsReader(size);
	} else {
		fprintf(stderr, "URLCellsReader: Unknown reader type: %s\n", type.c_str());
		exit(1);
	}
}

URLCellsReader::~URLCellsReader() {
	close();
}

void URLCellsReader::close() {
	if (reader != NULL) {
		reader->close();
		reader = NULL;
	}
}

int URLCellsReader::getType() {
	return INIT_WITH_CUSTOM_DATA;
}

int URLCellsReader::read(cell_t* buf, int len) {
	return reader->read(buf, len);
}
