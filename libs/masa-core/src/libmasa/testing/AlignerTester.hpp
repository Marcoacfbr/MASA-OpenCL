/*
 * AlignerTester.hpp
 *
 *  Created on: May 4, 2013
 *      Author: edans
 */

#ifndef ALIGNERTESTER_HPP_
#define ALIGNERTESTER_HPP_

#include "../libmasa.hpp"

class AlignerTester {
public:
	AlignerTester(IAligner* aligner);
	virtual ~AlignerTester();

	bool test(char* test_filename);

private:
	IAligner* aligner;
	void compare_files(const char* file0, const char* file1);
};

#endif /* ALIGNERTESTER_HPP_ */
