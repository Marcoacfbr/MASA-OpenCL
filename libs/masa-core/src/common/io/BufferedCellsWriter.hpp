/*
 * BufferedCellsWriter.hpp
 *
 *  Created on: Apr 22, 2014
 *      Author: edans
 */

#ifndef BUFFEREDCELLSWRITER_HPP_
#define BUFFEREDCELLSWRITER_HPP_

#include "CellsWriter.hpp"
#include "BufferedStream.hpp"

class BufferedCellsWriter: public CellsWriter, public BufferedStream {
public:
	BufferedCellsWriter(CellsWriter* writer, int bufferLimit);
	virtual ~BufferedCellsWriter();
	virtual void close();

	virtual int write(const cell_t* buf, int len);

private:
	CellsWriter* writer;

    void bufferLoop();
};

#endif /* BUFFEREDCELLSWRITER_HPP_ */
