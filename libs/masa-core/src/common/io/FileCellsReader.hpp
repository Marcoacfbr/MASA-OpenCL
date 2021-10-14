/*
 * FileCellsReader.hpp
 *
 *  Created on: Apr 7, 2014
 *      Author: edans
 */

#ifndef FILECELLSREADER_HPP_
#define FILECELLSREADER_HPP_

#include "SeekableCellsReader.hpp"

#include <stdio.h>
#include <string>
using namespace std;

class FileCellsReader: public SeekableCellsReader {
public:
	FileCellsReader(FILE* file);
	FileCellsReader(const string path);
	virtual ~FileCellsReader();
	virtual void close();

	virtual int getType();
	virtual int read(cell_t* buf, int len);
	virtual void seek(int position);
	virtual int getOffset();
private:
	FILE* file;
};

#endif /* FILECELLSREADER_HPP_ */
