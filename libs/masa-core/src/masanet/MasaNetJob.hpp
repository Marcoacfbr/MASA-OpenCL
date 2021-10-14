/*
 * MasaNetJob.hpp
 *
 *  Created on: Nov 5, 2013
 *      Author: edans
 */

#ifndef MASANETJOB_HPP_
#define MASANETJOB_HPP_

#include <string>
#include <vector>
using namespace std;

class MasaNetJob {
public:
	MasaNetJob();
	virtual ~MasaNetJob();
private:
	Sequence sequences[2];
	vector<int> split;
};

#endif /* MASANETJOB_HPP_ */
