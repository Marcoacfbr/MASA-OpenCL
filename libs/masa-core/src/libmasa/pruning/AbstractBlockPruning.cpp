/*
 * AbstractBlockPruning.cpp
 *
 *  Created on: Nov 7, 2013
 *      Author: edans
 */

#include "../IManager.hpp"
#include "AbstractBlockPruning.hpp"
#include <algorithm>

AbstractBlockPruning::AbstractBlockPruning() {
	this->bestScore = -INF;
	this->grid = NULL;
	this->recurrenceType = SMITH_WATERMAN;
	this->score_params = NULL;
	this->max_i = 0;
	this->max_j = 0;
}

AbstractBlockPruning::~AbstractBlockPruning() {

}

void AbstractBlockPruning::updateBestScore(int score) {
	if (this->bestScore < score) {
		this->bestScore = score;
	}
}

const Grid* AbstractBlockPruning::getGrid() const {
	return grid;
}

void AbstractBlockPruning::setGrid(const Grid* grid) {
	if (this->grid != NULL) {
		finalize();
	}
	this->grid = grid;
	this->bestScore = -INF;
	if (this->grid != NULL) {
		initialize();
	}
}

void AbstractBlockPruning::setSuperPartition(Partition superPartition) {
	this->max_i = superPartition.getI1();
	this->max_j = superPartition.getJ1();
}

void AbstractBlockPruning::setScoreParams(const score_params_t* score_params) {
	this->score_params = score_params;
}

bool AbstractBlockPruning::isBlockPrunable(int bx, int by, int score) {
	if (grid == NULL) return false;
	int i0;
	int j0;
	int i1;
	int j1;
	int adjustment = grid->getBlockAdjustment(bx,by);
	grid->getBlockPosition(bx, by, &i0, &j0, &i1, &j1);
	if (i0 == -1) {
		return true;
	}

	int distI = max_i - i0;
	int distJ = max_j - j0;

	int distMin = (distI<distJ)?distI:distJ;
	int inc = distMin*score_params->match;
	int dec = 0;

	//fprintf(stderr, "--isBlockPrunable? (%d,%d) (%d,%d) %d+%d<%d\n", bx, by, i0, j0, score,inc, bestScore);


	if (recurrenceType == NEEDLEMAN_WUNSCH) {
		int gaps;

		gaps = abs(distJ-distI) - std::max(j1-j0, i1-i0);
		if (gaps > 0) {
			inc -= score_params->gap_open + gaps*score_params->gap_ext;
		}

		// FIXME - must consider alignment edge instead of recurrenceType
		dec -= distMin*score_params->mismatch;
		gaps = abs(distJ-distI) + std::max(j1-j0, i1-i0);
		dec += score_params->gap_open + gaps*score_params->gap_ext;
	}

	updateBestScore(score - dec);

	int max = score + inc + adjustment;
	//fprintf(stderr, "isBlockPrunable(%2d,%2d, [%d/%d]) D:%d +%d-%d - Max: %d\n", bx, by, score, bestScore, distMin, inc, dec, max);
	return (max <= bestScore);
}

void AbstractBlockPruning::setRecurrenceType(int recurrenceType) {
	this->recurrenceType = recurrenceType;
}

int AbstractBlockPruning::getRecurrenceType() const {
	return recurrenceType;
}
