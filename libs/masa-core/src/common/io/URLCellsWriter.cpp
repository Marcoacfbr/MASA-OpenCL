/*
 * URLCellsWriter.cpp
 *
 *  Created on: Apr 23, 2014
 *      Author: edans
 */

#include "URLCellsWriter.hpp"

#include "FileCellsWriter.hpp"
#include "DummyCellsWriter.hpp"
#include "SocketCellsWriter.hpp"

#include <stdio.h>
#include <stdlib.h>

URLCellsWriter::URLCellsWriter(string url) {
	int pos1 = url.find_first_of("://");
	if (pos1 == -1) {
		fprintf(stderr, "URLCellsWriter: Wrong URL format: %s\n", url.c_str());
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
		writer = new SocketCellsWriter(hostname, port);
	} else if (type == "file") {
		writer = new FileCellsWriter(param);
	} else if (type == "null") {
		writer = new DummyCellsWriter();
	} else {
		fprintf(stderr, "URLCellsWriter: Unknown writer type: %s\n", type.c_str());
		exit(1);
	}
}

URLCellsWriter::~URLCellsWriter() {
	// TODO Auto-generated destructor stub
}

void URLCellsWriter::close() {
	if (writer != NULL) {
		writer->close();
		writer = NULL;
	}
}

int URLCellsWriter::write(const cell_t* buf, int len) {
	return writer->write(buf, len);
}
