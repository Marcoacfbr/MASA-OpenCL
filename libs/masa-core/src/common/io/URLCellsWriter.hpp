/*
 * URLCellsWriter.hpp
 *
 *  Created on: Apr 23, 2014
 *      Author: edans
 */

#ifndef URLCELLSWRITER_HPP_
#define URLCELLSWRITER_HPP_

#include "CellsWriter.hpp"
#include <string>
using namespace std;

class URLCellsWriter: public CellsWriter {
public:
	URLCellsWriter(string url);
	virtual ~URLCellsWriter();
	virtual void close();

	virtual int write(const cell_t* buf, int len);

private:
	CellsWriter* writer;
};

#endif /* URLCELLSWRITER_HPP_ */
