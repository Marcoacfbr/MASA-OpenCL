/*******************************************************************************
 *
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
 *
 ******************************************************************************/

#ifndef LIBMASA_HPP_
#define LIBMASA_HPP_

/* libmasa includes */
#include "libmasaTypes.hpp"
#include "IManager.hpp"
#include "IAligner.hpp"
#include "AbstractAligner.hpp"
#include "AbstractBlockAligner.hpp"
#include "AbstractDiagonalAligner.hpp"
#include "AbstractAlignerSafe.hpp"
#include "AbstractAlignerParameters.hpp"
#include "capabilities.hpp"
#include "Partition.hpp"
#include "aligners/OpenMPAligner.hpp"

/* libmasa util includes */
#include "utils/AlignerUtils.hpp"
#include "utils/Grid.hpp"

/* libmasa block pruning classes */
#include "pruning/AbstractBlockPruning.hpp"
#include "pruning/BlockPruningDiagonal.hpp"
#include "pruning/BlockPruningGeneric.hpp"
#include "pruning/BlockPruningGenericN2.hpp"


/**
 * Entry point for the MASA architecture. This function must be called
 * in the main procedure of the extension. The main argc/argv parameter
 * must be passed to the libmasa_entry_point in order to process the
 * command line parameters.
 *
 * @see The aligner/example/main.cpp source file contains a simple example
 * of calling the int libmasa_entry_point function.
 *
 *
 * @param argc	number of arguments
 * @param argv	command line arguments
 * @param aligner	an instance of the IAligner that will execute the
 * 				alignment procedure.
 * @return	exit code.
 */
int libmasa_entry_point(int argc, char** argv, IAligner* aligner);


#endif /* LIBMASA_HPP_ */
