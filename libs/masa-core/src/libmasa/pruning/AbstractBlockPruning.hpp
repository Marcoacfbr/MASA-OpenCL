/*
 * AbstractBlockPruning.hpp
 *
 *  Created on: Nov 7, 2013
 *      Author: edans
 */

#ifndef ABSTRACTBLOCKPRUNING_HPP_
#define ABSTRACTBLOCKPRUNING_HPP_

#include "../libmasaTypes.hpp"
#include "../utils/Grid.hpp"
#include "../IManager.hpp" // for the recurrence types

class AbstractBlockPruning {
public:
	AbstractBlockPruning();
	virtual ~AbstractBlockPruning();

	void updateBestScore(int score);
	const Grid* getGrid() const;
	void setGrid(const Grid* grid);
	void setSuperPartition(Partition superPartition);
	void setScoreParams(const score_params_t* score_params);
	void setLocalAlignment();
	void setGlobalAlignment();
	int getRecurrenceType() const;
	void setRecurrenceType(int recurrenceType);

protected:
	bool isBlockPrunable(int bx, int by, int score);



private:
	const score_params_t* score_params;

	int  max_i;
	int  max_j;

	int  bestScore;

	int  recurrenceType;

	const Grid* grid;


	virtual void initialize() = 0;
	virtual void finalize() = 0;
};

#endif /* ABSTRACTBLOCKPRUNING_HPP_ */
