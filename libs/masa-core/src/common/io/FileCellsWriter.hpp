/*
 * FileCellsWriter.hpp
 *
 *  Created on: Apr 23, 2014
 *      Author: edans
 */

#ifndef FILECELLSWRITER_HPP_
#define FILECELLSWRITER_HPP_

#include "CellsWriter.hpp"

#include <stdio.h>
#include <string>
using namespace std;

class FileCellsWriter: public CellsWriter {
public:
	FileCellsWriter(FILE* file);
	FileCellsWriter(const string path);
	virtual ~FileCellsWriter();
	virtual void close();

	virtual int write(const cell_t* buf, int len);

private:
	FILE* file;
};

#endif /* FILECELLSWRITER_HPP_ */
