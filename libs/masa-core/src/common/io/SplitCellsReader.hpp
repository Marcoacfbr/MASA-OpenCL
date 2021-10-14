/*
 * SplitReader.hpp
 *
 *  Created on: May 11, 2014
 *      Author: edans
 */

#ifndef SPLITCELLSREADER_HPP_
#define SPLITCELLSREADER_HPP_

#include "SeekableCellsReader.hpp"

class SplitCellsReader: public SeekableCellsReader {
public:
	SplitCellsReader(SeekableCellsReader* reader, int pos0, bool closeAfterUse);
	virtual ~SplitCellsReader();
	virtual void seek(int offset);
	virtual int getOffset();
	virtual void close();
	virtual int getType();
	virtual int read(cell_t* buf, int len);

private:
	SeekableCellsReader* reader;
	int pos0;
	bool closeAfterUse;
};

#endif /* SPLITCELLSREADER_HPP_ */
