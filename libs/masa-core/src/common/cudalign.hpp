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

#pragma once

//#include "cuda_configs.hpp"
//#include "sw_configs.hpp"
#include "exceptions/exceptions.hpp"
//#include "buffer/Buffer.hpp"
//#include "buffer/BufferFactory.hpp"
#include "AlignerManager.hpp"
#include "Job.hpp"
#include "SpecialRowReader.hpp"
#include "SpecialRowWriter.hpp"
#include "CrosspointsFile.hpp"
#include "Timer.hpp"
#include "RecurrentTimer.hpp"
#include "Status.hpp"
#include "BestScoreList.hpp"
#include "BlocksFile.hpp"
#include "macros.hpp"
#include "biology/biology.hpp"
#include "sra/sra.hpp"
#include "configs/Configs.hpp"
#include "utils.hpp"

//#include "../gpu/cuda_util.h"

// This is for IDE's code assistance only. 
// Set -D__DISABLE_IDE_COMPILATION_ASSISTANCE__ for proper compilation
/*#ifndef __DISABLE_IDE_CODE_ASSISTANCE__
#include "cuda_ide_code_assistance.hpp"
#endif*/
