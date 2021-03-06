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

#ifndef CUDALIGNERPARAMETERS_HPP_
#define CUDALIGNERPARAMETERS_HPP_

#include "libmasa/AbstractAlignerParameters.hpp"


/**
 * Constant used to select fastest gpu available.
 */
#define DETECT_FASTEST_GPU (-1)

/**
 * Parameters for the MASA-CUDA extension.
 */
class OpenCLAlignerParameters : public AbstractAlignerParameters {
private:
	/** Selected GPU or DETECT_FASTEST_GPU for automatic selection */
	int gpu;

	/** Fixed number of blocks or 0 (zero) for auto configuration */
	int blocks;

public:
	OpenCLAlignerParameters();
	virtual ~OpenCLAlignerParameters();

	virtual int processArgument(int argument_code, const char* argument_str, const char* argument_option);

	int getBlocks() const;
	void setBlocks(int blocks);
	int getGPU() const;
	void setGPU(int gpu);

};

#endif /* CUDALIGNERPARAMETERS_HPP_ */
