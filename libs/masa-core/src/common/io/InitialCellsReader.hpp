/*
 * InitialCellsReader.hpp
 *
 *  Created on: Apr 6, 2014
 *      Author: edans
 */

#ifndef INITIALCELLSREADER_HPP_
#define INITIALCELLSREADER_HPP_

#include "SeekableCellsReader.hpp"

class InitialCellsReader: public SeekableCellsReader {
public:
	InitialCellsReader(int startOffset);
	InitialCellsReader(int gapOpen, int gapExt, int startOffset=0);
	virtual ~InitialCellsReader();
	virtual void close();
	InitialCellsReader* clone(int offset);
	int getStartOffset();

	virtual int getType();
	virtual int read(cell_t* buf, int len);
	virtual void seek(int position);
	virtual int getOffset();

private:
	int position;
	int type;

	int gapOpen;
	int gapExt;
	int startOffset;

	void initialize();
};

#endif /* INITIALCELLSREADER_HPP_ */
