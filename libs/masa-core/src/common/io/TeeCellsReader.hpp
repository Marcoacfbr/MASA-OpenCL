/*
 * TeeCellsReader.hpp
 *
 *  Created on: Apr 23, 2014
 *      Author: edans
 */

#ifndef TEECELLSREADER_HPP_
#define TEECELLSREADER_HPP_

#include "CellsReader.hpp"
#include "FileCellsReader.hpp"
#include "FileCellsWriter.hpp"

#include <string>
using namespace std;

class TeeCellsReader: public SeekableCellsReader {
public:
	TeeCellsReader(CellsReader* reader, string filename);
	virtual ~TeeCellsReader();
	virtual void close();

	virtual int getType();
	virtual int read(cell_t* buf, int len);
	virtual void seek(int position);
	virtual int getOffset();
private:
	CellsReader* reader;
	string filename;

	FileCellsWriter* fileWriter;
	FileCellsReader* fileReader;
};

#endif /* TEECELLSREADER_HPP_ */
