/*******************************************************************************
 *
 * Copyright (c) 2010-2014   Edans Sandes
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
 *
 ******************************************************************************/

#include "OpenCLAlignerParameters.hpp"
//#include "cuda_util.h"
#include "cl_util.h"

#include "OpenCLAligner.hpp"

#include <stdio.h>
#include <getopt.h>

#include <sstream>
using namespace std;

#define USAGE "\
--gpu=GPU           Selects the index of GPU used for the computation. If  \n\
                           GPU is not informed, the fastest GPU is selected.   \n\
                           A list of available GPUs can be obtained with the   \n\
                           --list-gpus parameter. \n\
--list-gpus             Lists all available GPUs. \n\
--blocks=B              Run B CUDA Blocks\n\
"

#define ARG_GPU                 0x1001
#define ARG_LIST_GPUS           0x1002
#define ARG_BLOCKS              0x1003

/**
 * Optargs parameters
 */
static struct option long_options[] = {
        {"gpu",         required_argument,      0, ARG_GPU},
        {"list-gpus",   no_argument,            0, ARG_LIST_GPUS},
        {"blocks",      required_argument,      0, ARG_BLOCKS},
        {0, 0, 0, 0}
    };

/**
 * OpenCLAlignerParameters constructor.
 */
OpenCLAlignerParameters::OpenCLAlignerParameters() {
	blocks = 0;
	gpu = DETECT_FASTEST_GPU;

	setArguments(long_options, USAGE, "CUDA Specific Options");

}

/**
 * OpenCLAlignerParameters destructor.
 */
OpenCLAlignerParameters::~OpenCLAlignerParameters(){
}

/**
 * Processes the argument for the OpenCLAlignerParameters;
 * @copydoc AbstractAlignerParameters::processArgument
 */
int OpenCLAlignerParameters::processArgument(int argument_code, const char* argument_str, const char* argument_option) {
	//fprintf(stderr, "processArg(%d, %s, %d): %d\n", argc, current_arg, option_index, c);

	switch ( argument_code ) {
	case ARG_GPU:
		if ( optarg != NULL )  {
			sscanf ( optarg, "%d", &gpu );
		}
		break;
	case ARG_LIST_GPUS:
		clprintGPUDevices();
		exit ( 1 );
		break;
	case ARG_BLOCKS:
		if ( optarg != NULL )  {
			sscanf ( optarg, "%d", &blocks );
			if ( blocks > MAX_BLOCKS_COUNT ) {
				stringstream out;
				out << "Blocks count cannot be greater than " << MAX_BLOCKS_COUNT << ".";
				setLastError(out.str().c_str());
				return -1;
			}
		}
		break;
	}
	return 0;
}


int OpenCLAlignerParameters::getBlocks() const {
	return blocks;
}

void OpenCLAlignerParameters::setBlocks(int blocks) {
	this->blocks = blocks;
}

int OpenCLAlignerParameters::getGPU() const {
	return gpu;
}

void OpenCLAlignerParameters::setGPU(int gpu) {
	this->gpu = gpu;
}

