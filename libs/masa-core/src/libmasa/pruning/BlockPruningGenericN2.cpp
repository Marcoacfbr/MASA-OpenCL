/*
 * BlockPruningGenericN2.cpp
 *
 *  Created on: Nov 7, 2013
 *      Author: edans
 */

#include "BlockPruningGenericN2.hpp"

#include <stdio.h>
#include <string.h>

BlockPruningGenericN2::BlockPruningGenericN2() {
	this->k = NULL;
}

BlockPruningGenericN2::~BlockPruningGenericN2() {
	finalize();
}

void BlockPruningGenericN2::pruningUpdate(int bx, int by, int score) {
	if (getGrid() == NULL) return;
	updateBestScore(score);

	if (isBlockPrunable(bx, by, score)) {
		k[by+1][bx+1] = true;
	}
}

bool BlockPruningGenericN2::isBlockPruned(int bx, int by) {
	if (getGrid() == NULL) return false;
	if (k[by+1][bx] && k[by][bx] && k[by][bx+1]) {
		k[by+1][bx+1] = true;
		return true;
	} else {
		return false;
	}
}

void BlockPruningGenericN2::initialize() {
	int h = getGrid()->getGridHeight();
	int w = getGrid()->getGridWidth();

	this->k = new int*[h+1];
	for (int i=0; i<=h; i++) {
		this->k[i] = new int[w+1];
		memset(this->k[i], 0, sizeof(int)*(w+1));
	}
	for (int i=1; i<=h; i++) {
		this->k[i][0] = true;
	}
	for (int i=1; i<=w; i++) {
		this->k[0][i] = true;
	}
}

void BlockPruningGenericN2::finalize() {
	if (this->k != NULL) {
		int h = getGrid()->getGridHeight();
		int w = getGrid()->getGridWidth();

		for (int i=0; i<=h; i++) {
			delete[] this->k[i];
		}
		delete[] this->k;

		this->k = NULL;
	}
}
