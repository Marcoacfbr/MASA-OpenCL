/*
 * SeekableCellsReader.hpp
 *
 *  Created on: Apr 22, 2014
 *      Author: edans
 */

#ifndef SEEKABLECELLSREADER_HPP_
#define SEEKABLECELLSREADER_HPP_


#include "CellsReader.hpp"

/**
 *
 */
class SeekableCellsReader : public CellsReader {
public:
	virtual ~SeekableCellsReader() {};
	virtual void seek(int position) = 0;
	virtual int getOffset() = 0;
};

#endif /* SEEKABLECELLSREADER_HPP_ */
