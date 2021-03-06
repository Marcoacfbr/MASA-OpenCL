/*
 * Grid.hpp
 *
 *  Created on: Oct 23, 2013
 *      Author: edans
 */

class Grid;

#ifndef GRID_HPP_
#define GRID_HPP_

#include <stdio.h>

#include "../Partition.hpp"

class Grid {
public:
	Grid(Partition partition);
	virtual ~Grid();

	void setBlockHeight(int blockHeight);
	void setBlockWidth(int blockWidth);

	void splitGridVertically(int count);
	void splitGridHorizontally(int count);

	void splitGridVertically(int count, int* splits);
	void splitGridHorizontally(int count, int* splits);

	void getBlockPosition(int bx, int by, int* i0, int* j0, int* i1=NULL, int* j1=NULL) const;

	void setMinBlockSize(int minBlockWidth, int minBlockHeight);

	void setAdjustment(int adjustment);
	int  getBlockAdjustment(int bx, int by) const;

	int getGridWidth() const;
	int getGridHeight() const;

	int getBlockWidth(int bx, int by) const;
	int getBlockHeight(int bx, int by) const;

	int getHeight() const;
	int getWidth() const;

	const int* getBlockSplitHorizontal() const;
	const int* getBlockSplitVertical() const;

private:
	int  blockWidth;
	int  blockHeight;
	int  adjustment;
	int  minBlockWidth;
	int  minBlockHeight;

	/*
	 * The split vectors will contains the start coordinates of each block.
	 * The first element will always be 0 and the last element will have the
	 * dimension size. Note that the vector must contain N+1 elements, so the
	 * last element is blockSplitXXX[blockCountXXX].
	 */
	int  blockCountHorizontal;
	int* blockSplitHorizontal;
	int  blockCountVertical;
	int* blockSplitVertical;

	int width;
	int height;
	Partition partition;

	void getBlockPositionH(int bx, int by, int* j0, int* j1) const;
	void getBlockPositionV(int bx, int by, int* i0, int* i1) const;

};

#endif /* GRID_HPP_ */
