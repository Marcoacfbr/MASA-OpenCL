/*
 * AlignerPool.hpp
 *
 *  Created on: May 16, 2014
 *      Author: edans
 */

#ifndef ALIGNERPOOL_HPP_
#define ALIGNERPOOL_HPP_

#include <string>
using namespace std;

#include "../libmasa/libmasaTypes.hpp"
#include "CrosspointsFile.hpp"

class AlignerPool {
public:
	AlignerPool(string sharedPath);
	virtual ~AlignerPool();

	virtual void initialize();

	virtual string getLoadURL(int id);

	virtual void waitId(int id);
	virtual score_t receiveScore();
	virtual crosspoint_t receiveCrosspoint(int* final);
	virtual CrosspointsFile* receiveCrosspointFile();
	//virtual void waitSignal(int signal);

	virtual void registerNode(int id, int left, int right, string flushURL);
	virtual void dispatchScore(score_t score);
	virtual void dispatchCrosspoint(crosspoint_t crosspoint, int final);
	virtual void dispatchCrosspointFile(CrosspointsFile* file);
	//virtual void dispatchSignal(int signal);

	virtual bool isFirstNode();
	virtual bool isLastNode();
	const score_t& getBestNodeScore() const;
	void setBestNodeScore(const score_t& bestLocalScore);

private:
	string sharedPath;
	int right;
	int left;
	int crosspointIdSentCounter;
	int crosspointIdRecvCounter;
	score_t bestNodeScore;

	string getMsgFile(string prefix, int id, int count=-1);
	string getSignalFile(string msgFile);
	void sendSignal(string msgFile);
	void waitSignal(string msgFile);
	bool peekSignal(string msgFile);
};

#endif /* ALIGNERPOOL_HPP_ */
