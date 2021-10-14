/*
 * ReversedCellsReader.hpp
 *
 *  Created on: Apr 22, 2014
 *      Author: edans
 */

#ifndef REVERSEDCELLSREADER_HPP_
#define REVERSEDCELLSREADER_HPP_

#include "SeekableCellsReader.hpp"

class ReversedCellsReader: public SeekableCellsReader {
public:
	ReversedCellsReader(SeekableCellsReader* reader);
	virtual ~ReversedCellsReader();
	virtual void close();

	virtual int getType();
	virtual int read(cell_t* buf, int len);
	virtual void seek(int position);
	virtual int getOffset();
private:
	SeekableCellsReader* reader;
	int position;
};

#endif /* REVERSEDCELLSREADER_HPP_ */
