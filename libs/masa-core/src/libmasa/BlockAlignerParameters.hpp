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

#ifndef BLOCKALIGNERPARAMETERS_HPP_
#define BLOCKALIGNERPARAMETERS_HPP_


#include "AbstractAlignerParameters.hpp"

#define MAX_GRID_SIZE 1024
#define MAX_BLOCK_SIZE 1024*20
#define MIN_GRID_SIZE 1
#define MIN_BLOCK_SIZE 128

#define ITERATE_BY_ROWS			(0)
#define ITERATE_BY_COLS			(1)
#define ITERATE_BY_SQUARE		(2)
#define ITERATE_BY_SQUARE_INV	(3)
#define ITERATE_BY_DIAGONAL		(4)

/**
 * This class contains the parameters for the AbstractBlockAligner.
 */
class BlockAlignerParameters : public AbstractAlignerParameters {

private:
	/** Number of column of blocks */
	int gridWidth;

	/** Number of rows of blocks */
	int gridHeight;

	/** Height of one block */
	int blockHeight;

	/** Width of one block. 0 indicates variable */
	int blockWidth;

	/** Order of block processing */
	int blockOrder;
public:
	BlockAlignerParameters();
	virtual ~BlockAlignerParameters();

	/**
	 * @see AbstractAlignerParameters::processArgument() method.
	 */
	int processArgument(int argument_code, const char* argument_str, const char* argument_option);

	int getBlockHeight() const;
	int getBlockWidth() const;
	int getGridWidth() const;
	int getGridHeight() const;
	int getBlockOrder() const;
};


#endif /* BLOCKALIGNERPARAMETERS_HPP_ */
