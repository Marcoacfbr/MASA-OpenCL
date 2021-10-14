/*
 * DummyReader.hpp
 *
 *  Created on: Apr 22, 2014
 *      Author: edans
 */

#ifndef DUMMYCELLSREADER_HPP_
#define DUMMYCELLSREADER_HPP_

#include "SeekableCellsReader.hpp"

class DummyCellsReader: public SeekableCellsReader {
public:
	DummyCellsReader(int size = -1);
	virtual ~DummyCellsReader();
	virtual void close();

	virtual int getType();
	virtual int read(cell_t* buf, int len);
	virtual void seek(int position);
	virtual int getOffset();
private:
	int size;
	int position;
};

#endif /* DUMMYCELLSREADER_HPP_ */
