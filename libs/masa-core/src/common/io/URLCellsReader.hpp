/*
 * URLCellsReader.hpp
 *
 *  Created on: Apr 22, 2014
 *      Author: edans
 */

#ifndef URLCELLSREADER_HPP_
#define URLCELLSREADER_HPP_

#include "CellsReader.hpp"
#include <string>
using namespace std;

class URLCellsReader: public CellsReader {
public:
	URLCellsReader(string url);
	virtual ~URLCellsReader();
	virtual void close();

	virtual int getType();
	virtual int read(cell_t* buf, int len);

private:
	CellsReader* reader;

};

#endif /* URLCELLSREADER_HPP_ */
