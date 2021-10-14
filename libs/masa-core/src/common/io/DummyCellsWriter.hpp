/*
 * DummyReader.hpp
 *
 *  Created on: Apr 22, 2014
 *      Author: edans
 */

#ifndef DUMMYCELLSWRITER_HPP_
#define DUMMYCELLSWRITER_HPP_

#include "CellsWriter.hpp"

class DummyCellsWriter: public CellsWriter {
public:
	DummyCellsWriter();
	virtual ~DummyCellsWriter();
	virtual void close();

	virtual int write(const cell_t* buf, int len);

private:
};

#endif /* DUMMYCELLSWRITER_HPP_ */
