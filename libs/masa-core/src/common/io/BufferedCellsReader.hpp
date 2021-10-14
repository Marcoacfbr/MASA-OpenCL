/*
 * BufferedCellReader.hpp
 *
 *  Created on: Apr 22, 2014
 *      Author: edans
 */

#ifndef BUFFEREDCELLSREADER_HPP_
#define BUFFEREDCELLSREADER_HPP_

#include "CellsReader.hpp"
#include "BufferedStream.hpp"
#include "FileCellsReader.hpp"
#include "FileCellsWriter.hpp"

class BufferedCellsReader: public SeekableCellsReader, public BufferedStream {
public:
	BufferedCellsReader(CellsReader* reader, int bufferLimit);
	virtual ~BufferedCellsReader();
	virtual void close();

	virtual int getType();
	virtual int read(cell_t* buf, int len);
	virtual void seek(int position);
	virtual int getOffset();
private:
	CellsReader* reader;
	int offset;

    void bufferLoop();
};

#endif /* BUFFEREDCELLSREADER_HPP_ */
