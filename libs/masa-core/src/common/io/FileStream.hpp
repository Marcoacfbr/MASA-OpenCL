/*******************************************************************************
 * Copyright (c) 2010, 2013   Edans Sandes
 *
 * This file is part of CUDAlign.
 * 
 * CUDAlign is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * CUDAlign is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with CUDAlign.  If not, see <http://www.gnu.org/licenses/>.
 ******************************************************************************/

#ifndef FILESTREAM_HPP_
#define FILESTREAM_HPP_

#include "SeekableCellsReader.hpp"
#include "CellsWriter.hpp"

#include <string>
using namespace std;

class FileStream: public SeekableCellsReader, public CellsWriter {
public:
	FileStream(string filename);
	virtual ~FileStream();

	virtual void close();
	virtual int getType();
	virtual int read(cell_t* buf, int len);
	virtual int write(const cell_t* buf, int len);
	virtual void seek(int position);
	virtual int getOffset();

private:
	FILE* file;
	int posRead;
};

#endif /* FILESTREAM_HPP_ */
