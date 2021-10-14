/*
 * cl_util.h
 *
 *  Created on: 08/09/2014
 *      Author: marco
 */

/*******************************************************************************
 *
 * Copyright (c) 2010-2014   Edans Sandes / Marco Figueiredo
 *
 * This file is part of CUDAlign - OpenCl .
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

/**
 * @file cl_util.h
 * @brief File with basic functions and macros for OpenCL calls.
 */

#include <stdio.h>
#include <stdlib.h>

#include <CL/opencl.h>
//#include <CL20/opencl.h>

struct clDeviceProp {
	char name[30];
	cl_ulong totalGlobalMem;
	cl_ulong sharedMemPerBlock;
    size_t regsPerBlock;
	size_t maxThreadsDim[3];
	cl_uint clockRate;
	cl_ulong totalConstMem;
	cl_uint multiProcessorCount;
};

#define clutilSafeCall(err)           __clSafeCall      (err, __FILE__, __LINE__)
inline void __clSafeCall( cl_int err, const char *file, const int line );
const char * clgetErrorStr(cl_int e);
cl_mem clalloc0(cl_context context, cl_device_id device, int size);
cl_mem clallocSeq(cl_context context, cl_device_id device, const char* data, int len, const int padding_len, char padding_char);
void clprintDevProp(FILE * fp, cl_device_id deviceId);
void clgetMemoryUsage(size_t* freeMem, size_t* totalMem=NULL);
void clprintGPUDevices(FILE* file=stdout);
cl_int clgetGPUMultiprocessors(cl_device_id deviceId);
cl_device_id clselectGPU(cl_device_id id);
cl_int clgetGPUWeights(cl_int* proportion, cl_int n);
cl_int clgetAvailableGPU(cl_int* ids, cl_int n);
int clgetDevCapability(cl_device_id deviceId);
int clgetCompiledCapability();
clDeviceProp clgetDeviceProperties(cl_device_id dev);
cl_platform_id clSetupPlatform();
cl_device_id clSetupDevice(cl_platform_id platform);
cl_context CreateContext(cl_platform_id PlatformId);
cl_command_queue CreateCommandQueue(cl_context context, cl_device_id *device);
cl_program CreateProgram(cl_context context, cl_device_id device, const char* fileName);
void Cleanup(cl_context context, cl_program program);




