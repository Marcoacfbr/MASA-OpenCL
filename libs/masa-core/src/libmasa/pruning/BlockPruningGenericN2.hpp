/*
 * BlockPruningGenericN2.hpp
 *
 *  Created on: Nov 7, 2013
 *      Author: edans
 */

#ifndef BLOCKPRUNINGGENERICN2_HPP_
#define BLOCKPRUNINGGENERICN2_HPP_

#include "AbstractBlockPruning.hpp"

class BlockPruningGenericN2: public AbstractBlockPruning {
public:
	BlockPruningGenericN2();
	virtual ~BlockPruningGenericN2();

	virtual void pruningUpdate(int bx, int by, int score);
	virtual bool isBlockPruned(int bx, int by);
private:
	int** k;

	virtual void initialize();
	virtual void finalize();
};

#endif /* BLOCKPRUNINGGENERICN2_HPP_ */
