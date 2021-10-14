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

#include <stdio.h>
#include "libmasa/libmasa.hpp"
#include "config.h"
#include "OpenCLAligner.hpp"
#include <CL/cl.h>
#include "cl_util.h"
#include <iostream>

/**
 * Header of Execution.
 */
#define HEADER "\
\n\
\033[1mMASA-OpenCL "PACKAGE_VERSION"  -  GPU tool for huge sequences alignment\033[0m\n"

/**
 * C entry point.
 *
 * @param argc number of arguments.
 * @param argv array of arguments.
 * @return return code.
 */

cl_platform_id glplatform;
cl_device_id gldevice;

int main ( int argc, char** argv ) {
	cl_int status;
    size_t size;
	char buf[65536];


	printf (HEADER);
	glplatform = clSetupPlatform();
	gldevice = clSetupDevice(glplatform);


	//std::cout << glplatform << "\n\n";
	//std::cout << gldevice << "\n\n";

	//status = clGetDeviceInfo(gldevice, CL_DRIVER_VERSION, sizeof buf, buf, &size);
	//std::cout << "\n\n" << buf << "\n\n";

	 return libmasa_entry_point(argc, argv, new OpenCLAligner());
}
