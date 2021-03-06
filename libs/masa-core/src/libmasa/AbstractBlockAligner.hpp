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

#ifndef ABSTRACTBLOCKALIGNER_HPP_
#define ABSTRACTBLOCKALIGNER_HPP_

#include "AbstractAligner.hpp"
#include "BlockAlignerParameters.hpp"
#include "AbstractBlockProcessor.hpp"
#include "pruning/BlockPruningGenericN2.hpp"

/**
 * @brief Abstract class that processes blocks individually considering
 * some schedule mechanism.
 *
 * This class implements some common behavior to simplify the implementation
 * of block aligners. These aligners process blocks individually using
 * a customized schedule mechanism that processes block through an
 * AbstractBlockProcessor object.
 *
 * Use the AbstractBlockProcessor if
 * you want to create a CPU block scheduler that processes a single block
 * in the specific hardware/software architecture.
 *
 * In order to extend an AbstractDiagonalAligner, the class must implement
 * two methods.
 *
 * <ul>
 *  <li>scheduleBlocks: schedules the block executions using a customized
 *  	mechanism.
 *  <li>alignBlock: responsible to receive/dispatch data from/to MASA-Core
 *  	and call the processBlock for each block. These procedures must
 *  	be aware of the schedule mechanism in order to avoid unsafe calls
 *  	to MASA-Core.
 * </ul>
 *
 */
class AbstractBlockAligner : public AbstractAligner {
public:
	/**
	 * Constructor
	 */
	AbstractBlockAligner(AbstractBlockProcessor* blockProcessor);

	/**
	 * Destructor.
	 */
	virtual ~AbstractBlockAligner();

	/* Implementation of virtual methods from IAligner */

	virtual void initialize();
	virtual void alignPartition(Partition partition);
	virtual void finalize();

	virtual aligner_capabilities_t getCapabilities();
	virtual const score_params_t* getScoreParameters();
	virtual AbstractAlignerParameters* getParameters();

	virtual void setSequences(const char* seq0, const char* seq1, int seq0_len, int seq1_len);
	virtual void unsetSequences();

	virtual void clearStatistics();
	virtual void printInitialStatistics(FILE* file);
	virtual void printStageStatistics(FILE* file);
	virtual void printFinalStatistics(FILE* file);
	virtual void printStatistics(FILE* file);
	virtual const char* getProgressString() const;
	virtual long long getProcessedCells();


protected:
	/** Chunk of rows used to pass cells from up to bottom blocks */
	cell_t** row;
	/** Chunk of columns used to pass cells from left to right blocks */
	cell_t** col;
	/** Last cell read in the first column */
	cell_t firstColumnTail;
	/** Last cell read in the first row */
	cell_t firstRowTail;

	/** Total number of blocks containing in the grid */
	int statTotalBlocks;
	/** Number of pruned blocks */
	int statPrunedBlocks;

	/* Virtual methods that must be implemented by subclasses */

	/**
	 * Schedules all the blocks for execution. As soon as one block is
	 * ready to be executed, this method must call the
	 * AbstractBlockAligner::alignBlock(int,int) function in order to prepare
	 * this block for real execution.
	 *
	 * @param grid_width width of the grid in blocks.
	 * @param grid_height height of the grid in blocks.
	 */
	virtual void scheduleBlocks(int grid_width, int grid_height) = 0;

	/**
	 * This method is called by AbstractBlockAligner::alignBlock(int,int)
	 * with additional information about block coordinates. Then, this method
	 * must receive/dispatch rows and columns from/to MASA-Core and call
	 * the processBlock(int,int,int,int,int,int) method.
	 *
	 * @param bx horizontal block coordinate
	 * @param by vertical block coordinate
	 * @param i0 vertical first row of the block
	 * @param j0 horizontal first column of the block
	 * @param i1 vertical last row of the block
	 * @param j1 horizontal last column of the block
	 */
	virtual void alignBlock(int bx, int by, int i0, int j0, int i1, int j1) = 0;

	/* Other protected methods*/

	void alignBlock(int bx, int by);
	bool processBlock(int bx, int by, int i0, int j0, int i1, int j1);
	bool isSpecialRow(int by);
	bool isSpecialColumn(int by);

private:
	/** Parameter used in this aligner */
	BlockAlignerParameters* params;

	/** Stores the score of each grid */
	score_t** grid_scores;

	/** Processor that computes a single block */
	AbstractBlockProcessor* blockProcessor;

	/** Block Pruner object */
	BlockPruningGenericN2* blockPruner;

	/** Number of threads available */
	int maxThreads;

	/** Maintains the minimum blockWidth used. */
	int statMinBlockWidth;
	/** Maintains the maximum blockWidth used. */
	int statMaxBlockWidth;
	/** Maintains the minimum blockHeight used. */
	int statMinBlockHeight;
	/** Maintains the maximum blockHeight used. */
	int statMaxBlockHeight;
	/** Maintains the minimum gridWidth used. */
	int statMinGridWidth;
	/** Maintains the maximum gridWidth used. */
	int statMaxGridWidth;
	/** Maintains the minimum gridHeight used. */
	int statMinGridHeight;
	/** Maintains the maximum gridHeight used. */
	int statMaxGridHeight;

	/** Score parameters */
	score_params_t score_params;

	/* memory related methods */

	void allocateStructures();
	void deallocateStructures();
	Grid* configureGrid(Partition partition);

	/* Other methods */

	bool isBlockPruned(int bx, int by) const;
	void pruningUpdate(int bx, int by, int score);
};

#endif /* ABSTRACTBLOCKALIGNER_HPP_ */
