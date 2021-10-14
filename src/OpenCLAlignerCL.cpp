/*******************************************************************************
 *
// * Copyright (c) 2010-2014   Edans Sandes
// *
// * This file is part of CUDAlign.
// *
// * CUDAlign is free software: you can redistribute it and/or modify
// * it under the terms of the GNU General Public License as published by
// * the Free Software Foundation, either version 3 of the License, or
// * (at your option) any later version.
// *
// * CUDAlign is distributed in the hope that it will be useful,
// * but WITHOUT ANY WARRANTY; without even the implied warranty of
// * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// * GNU General Public License for more details.
// *
// * You should have received a copy of the GNU General Public License
// * along with CUDAlign.  If not, see <http://www.gnu.org/licenses/>.
// *
// ******************************************************************************/
//

#define STR_HELPER(x) #x
#define STR(x) STR_HELPER(x)

#include "OpenCLAligner.hpp"
#include "cl_util.h"
//#include "cuda_util.h"
#include "config.h"

#include <stdio.h>
#include <getopt.h>
#include <iostream>
#include <string>
#include <fstream>


void cl_bind_textures(cl_structures_t * op, const char* seq0, const int seq0_len, const char* seq1, const int seq1_len) {

	cl_int errNum;

	cl_mem buffert0 = clalloc0(op->glcontext, op->gldevice, sizeof(char) * seq0_len);
	cl_mem buffert1 = clalloc0(op->glcontext, op->gldevice, sizeof(char) * seq1_len);


	errNum = clEnqueueWriteBuffer (op->glcommand, buffert0, CL_TRUE, 0, sizeof(char) * (seq0_len-MAX_GRID_HEIGHT), seq0, 0, NULL, NULL);

	errNum = clEnqueueWriteBuffer (op->glcommand, buffert1, CL_TRUE, 0, sizeof(char) * seq1_len, seq1, 0, NULL, NULL);
	clFlush(op->glcommand);
	clFinish(op->glcommand);
	op->t_seq0 = buffert0;
	op->t_seq1 = buffert1;
	//cutilSafeCall(cudaBindTexture(0, t_seq0, seq0, seq0_len));
	//cutilSafeCall(cudaBindTexture(0, t_seq1, seq1, seq1_len));
}

/**
 * Unbind the textures for the DNA sequences.
 */
void cl_unbind_textures() {
//	cutilSafeCall(cudaUnbindTexture(t_seq1));
//	cutilSafeCall(cudaUnbindTexture(t_seq0));
return;
	}

/**
 * Copies the split positions (used to identify the range of columns for each
 * block) to the GPU constant memory. The element split[0] must be the first
 * column of the partition and split[blocks] must be the last column.
 *
 * @param split vector with the block split positions.
 * @param blocks number of blocks.
 */
void cl_copy_split(cl_structures_t * op, const int* split, const int blocks) {
	cl_int err;
	cl_command_queue command = op->glcommand;
	size_t local;
	size_t global;
	char buffer[10240];
	cl_program program;
	char options[64];

	strcpy (options, "-D THREADS_COUNT=" STR(THREADS_COUNT));

	std::ifstream srcFile("src/OpenCLAligner.cl");
	if (!srcFile.is_open())
		exit(0);
	std::string srcProg(std::istreambuf_iterator<char>(srcFile),(std::istreambuf_iterator<char>()));
	const char * src = srcProg.c_str();
	size_t length = srcProg.length();
	program = clCreateProgramWithSource(op->glcontext, 1, &src, &length, &err);
	if (!program)
			exit(0);
	op->glprogram = program;
	err = clBuildProgram(program, 0, NULL, options, NULL, NULL);

	clGetProgramBuildInfo(program, op->gldevice, CL_PROGRAM_BUILD_LOG, sizeof(buffer), buffer, &length);
	cout<<"--- Build log ---\n "<<buffer<<endl;

	err = clEnqueueWriteBuffer(command, op->d_split_m, CL_TRUE, 0, sizeof(int)*(blocks+1), &split[0], 0, NULL, NULL);
	if (err != CL_SUCCESS)
		exit(0);

	clFlush(command);
	clFinish(command);

}

/**

 * Initialize the horizontal bus with H=-INF and F=-INF.
 *
 * @param p0,p1 range to be initialized.
 * @param d_busH bus to be initialized.
 */
void cl_initializeBusHInfinity(cl_structures_t * op, const int p0, const int p1) {
	int threads = 512;
	int blocks = MAX_BLOCKS_COUNT;
	cl_int err;
	cl_command_queue command = op->glcommand;
	cl_kernel kernel;
	size_t local;
	size_t global;
    int diff = p1-p0;

	kernel = clCreateKernel(op->glprogram, "cl_kernel_initialize_busH_ungapped", &err);
	if (!kernel)
		exit(0);
	err = 0;
	err  = clSetKernelArg(kernel, 0, sizeof(cl_mem), &op->d_busH);
	err |= clSetKernelArg(kernel, 1, sizeof(p0), &p0);
	err |= clSetKernelArg(kernel, 2, sizeof(diff), &diff);
	if (err != CL_SUCCESS)
		exit(0);

	global = blocks * threads;
	local = threads;

	err = clEnqueueNDRangeKernel(command, kernel, 1, NULL, &global, &local, 0, NULL, NULL);
	if (err != CL_SUCCESS)
		exit(0);

	clFlush(command);
	clFinish(command);
	clReleaseKernel(kernel);

	//kernel_initialize_busH_ungapped<<<threads, blocks>>>(d_busH, p0, (p1-p0));

}
/****************************************************************************
 *
 * Templates function for creating precompiled different kernels.
 * Since we have four templates (COLUMN_SOURCE, COLUMN_DESTINATION,
 * RECURRENCE_TYPE, CHECK_LOCATION) with many options, we have
 * a total of 2x2x2x2 = 16 precompiled kernels with different
 * constant parameters. This improve significantly the
 * performance of the different kernel calls, but increases
 * compilation time and executable size.
 *
 ****************************************************************************/

/**
 * Invoke a kernel to process one external diagonal. Depending on the number
 * of blocks, it decides which kernel will be called: single phase or
 * short/long phase. The templates creates many precompiled kernels with
 * different constant parameters, instead of variable parameters. This increases
 * the performance with a tradeoff of creating 16 precompiled kernels (bigger
 * executable size).
 *
 * @tparam COLUMN_SOURCE, COLUMN_DESTINATION, RECURRENCE_TYPE, CHECK_LOCATION
 * 				See these templates in the "Detailed Description" section in the beginning of this file.
 * @param blocks Number of blocks. If blocks is equal to 1, then the
 * single phase kernel is called, otherwise it calls the short/long kernels.
 * @param threads Maximum number of threads per block.
 * @param i0 first column id.
 * @param i1 last column id.
 * @param step the current external diagonal id, starting from 0.
 * @param cutBlock pruning window.
 * @param cuda the object containing all the cuda allocated structures.
 */
//template <int COLUMN_SOURCE, int COLUMN_DESTINATION, int RECURRENCE_TYPE, int CHECK_LOCATION>
void cl_lauch_external_diagonals(cl_context context, cl_device_id device, int COLUMN_SOURCE, int COLUMN_DESTINATION, int RECURRENCE_TYPE, int CHECK_LOCATION, const int blocks, const int threads,
		const int i0, const int i1, int step, const int2 cutBlock, cuda_structures_t* cuda, cl_structures_t* op) {

	cl_command_queue commands = op->glcommand;
	cl_program program;                 // compute program
	cl_kernel kernel, kernel2, kernel3;                   // compute kernel
	cl_event k_event;
	char buffer[10240];

	cl_mem output;                      // device memory used for the output array
	cl_int err;
	size_t local;
	size_t global;
	size_t max_work_groups;
	size_t max_compute_units;
	size_t max_wg_size;
	size_t length;

	int result[1];
	char options[64];

	strcpy (options, "-D THREADS_COUNT=" STR(THREADS_COUNT));
	program = op->glprogram;
	//err = clBuildProgram(op->glprogram, 0, NULL, options, NULL, NULL);
	//clGetProgramBuildInfo(program, op->gldevice, CL_PROGRAM_BUILD_LOG, sizeof(buffer), buffer, &length);
	//cout<<"--- Build log ---\n "<<buffer<<endl;

	kernel = clCreateKernel(program, "cl_kernel_single_phase", &err);
	if (!kernel)
		exit(0);
	kernel2 = clCreateKernel(program, "cl_kernel_long_phase", &err);
	if (!kernel2)
		exit(0);
	kernel3 = clCreateKernel(program, "cl_kernel_short_phase", &err);
	if (!kernel3)
		exit(0);


	if (blocks == 1) {

		global = blocks * threads;
		local = threads;

		err = clGetKernelWorkGroupInfo(kernel, device, CL_KERNEL_WORK_GROUP_SIZE, sizeof(max_work_groups), &max_work_groups, NULL);
		if (err != CL_SUCCESS)
			exit(0);
		if (local > max_work_groups)
		    local = max_work_groups;


		err = 0;
		err  = clSetKernelArg(kernel, 0, sizeof(i0), &i0);
		err |= clSetKernelArg(kernel, 1, sizeof(i1), &i1);
		err |= clSetKernelArg(kernel, 2, sizeof(step), &step);
		err |= clSetKernelArg(kernel, 3, sizeof(cutBlock), &cutBlock);
		err |= clSetKernelArg(kernel, 4, sizeof(cl_mem), (void*) &op->d_blockResult);
		err |= clSetKernelArg(kernel, 5, sizeof(cl_mem), (void*) &op->d_busH);
		err |= clSetKernelArg(kernel, 6, sizeof(cl_mem), (void*) &op->d_extraH);
		err |= clSetKernelArg(kernel, 7, sizeof(cl_mem), (void*) &op->d_busV_h);
		err |= clSetKernelArg(kernel, 8, sizeof(cl_mem), (void*) &op->d_busV_e);
		err |= clSetKernelArg(kernel, 9, sizeof(cl_mem), (void*) &op->d_busV_o);
		err |= clSetKernelArg(kernel, 10, sizeof(cl_mem), (void*) &op->d_loadColumnH);
		err |= clSetKernelArg(kernel, 11, sizeof(cl_mem), (void*) &op->d_loadColumnE);
		err |= clSetKernelArg(kernel, 12, sizeof(cl_mem), (void*) &op->d_flushColumnH);
		err |= clSetKernelArg(kernel, 13, sizeof(cl_mem), (void*) &op->d_flushColumnE);
		err |= clSetKernelArg(kernel, 14, sizeof(cl_mem), (void*) &op->d_split_m);
		err |= clSetKernelArg(kernel, 15, sizeof(cl_mem), (void*) &op->t_seq0);
		err |= clSetKernelArg(kernel, 16, sizeof(cl_mem), (void*) &op->t_seq1);

		if (err != CL_SUCCESS)
			exit(0);

		//kernel_single_phase<<< grid, block, 0>>>(i0, i1, step, cutBlock, cuda->d_blockResult, cuda->d_busH, cuda->d_extraH, cuda->d_busV_h, cuda->d_busV_e, cuda->d_busV_o, cuda->d_loadColumnH, cuda->d_loadColumnE, cuda->d_flushColumnH, cuda->d_flushColumnE);
		err = clEnqueueNDRangeKernel(commands, kernel, 1, NULL, &global, &local, 0, NULL, NULL);
		if (err != CL_SUCCESS)
			exit(0);

	} else {

		step--;

		err = 0;
		err  = clSetKernelArg(kernel2, 0, sizeof(i0), &i0);
		err |= clSetKernelArg(kernel2, 1, sizeof(i1), &i1);
		err |= clSetKernelArg(kernel2, 2, sizeof(step), &step);
		err |= clSetKernelArg(kernel2, 3, sizeof(cutBlock), &cutBlock);
		err |= clSetKernelArg(kernel2, 4, sizeof(cl_mem), (void*) &op->d_blockResult);
		err |= clSetKernelArg(kernel2, 5, sizeof(cl_mem), (void*) &op->d_busH);
		err |= clSetKernelArg(kernel2, 6, sizeof(cl_mem), (void*) &op->d_extraH);
		err |= clSetKernelArg(kernel2, 7, sizeof(cl_mem), (void*) &op->d_busV_h);
		err |= clSetKernelArg(kernel2, 8, sizeof(cl_mem), (void*) &op->d_busV_e);
		err |= clSetKernelArg(kernel2, 9, sizeof(cl_mem), (void*) &op->d_busV_o);
		err |= clSetKernelArg(kernel2, 10, sizeof(cl_mem), (void*) &op->d_split_m);
		err |= clSetKernelArg(kernel2, 11, sizeof(cl_mem), (void*) &op->t_seq0);
		err |= clSetKernelArg(kernel2, 12, sizeof(cl_mem), (void*) &op->t_seq1);

		if (err != CL_SUCCESS)
			exit(0);

		global = blocks * threads;
		local = threads;

		err = clGetKernelWorkGroupInfo(kernel2, device, CL_KERNEL_WORK_GROUP_SIZE, sizeof(max_work_groups), &max_work_groups, NULL);
		if (err != CL_SUCCESS)
			exit(0);
        if (local > max_work_groups)
        	local = max_work_groups;

		//kernel_long_phase<<< grid, threads, 0>>>(i0, i1, step-1, cutBlock, cuda->d_blockResult, cuda->d_busH, cuda->d_extraH, cuda->d_busV_h, cuda->d_busV_e, cuda->d_busV_o);
		err = clEnqueueNDRangeKernel(commands, kernel2, 1, NULL, &global, &local, 0, NULL, &k_event);
		if (err != CL_SUCCESS)
			exit(0);

		err = clWaitForEvents(1,&k_event);
		err = clReleaseEvent(k_event);

		step++;

        err = clEnqueueBarrier(commands);
        clFlush(commands);

		err = 0;
		err |= clSetKernelArg(kernel3, 0, sizeof(i0), &i0);
		err |= clSetKernelArg(kernel3, 1, sizeof(i1), &i1);
		err |= clSetKernelArg(kernel3, 2, sizeof(step), &step);
		err |= clSetKernelArg(kernel3, 3, sizeof(cutBlock), &cutBlock);
		err |= clSetKernelArg(kernel3, 4, sizeof(cl_mem), (void*) &op->d_blockResult);
		err |= clSetKernelArg(kernel3, 5, sizeof(cl_mem), (void*) &op->d_busH);
		err |= clSetKernelArg(kernel3, 6, sizeof(cl_mem), (void*) &op->d_extraH);
		err |= clSetKernelArg(kernel3, 7, sizeof(cl_mem), (void*) &op->d_busV_h);
		err |= clSetKernelArg(kernel3, 8, sizeof(cl_mem), (void*) &op->d_busV_e);
		err |= clSetKernelArg(kernel3, 9, sizeof(cl_mem), (void*) &op->d_busV_o);
		err |= clSetKernelArg(kernel3, 10, sizeof(cl_mem), (void*) &op->d_loadColumnH);
		err |= clSetKernelArg(kernel3, 11, sizeof(cl_mem), (void*) &op->d_loadColumnE);
		err |= clSetKernelArg(kernel3, 12, sizeof(cl_mem), (void*) &op->d_flushColumnH);
		err |= clSetKernelArg(kernel3, 13, sizeof(cl_mem), (void*) &op->d_flushColumnE);
		err |= clSetKernelArg(kernel3, 14, sizeof(cl_mem), (void*) &op->d_split_m);
		err |= clSetKernelArg(kernel3, 15, sizeof(cl_mem), (void*) &op->t_seq0);
		err |= clSetKernelArg(kernel3, 16, sizeof(cl_mem), (void*) &op->t_seq1);

		if (err != CL_SUCCESS)
			exit(0);

		global = blocks * threads;
		local = threads;
		if (local > max_work_groups)
		  	local = max_work_groups;


		err = clGetKernelWorkGroupInfo(kernel3, device, CL_KERNEL_WORK_GROUP_SIZE, sizeof(max_work_groups), &max_work_groups, NULL);
		if (err != CL_SUCCESS)
			exit(0);

		//kernel_short_phase<<< grid, threads, 0>>>(i0, i1, step, cutBlock, cuda->d_blockResult, cuda->d_busH, cuda->d_extraH, cuda->d_busV_h, cuda->d_busV_e, cuda->d_busV_o, cuda->d_loadColumnH, cuda->d_loadColumnE, cuda->d_flushColumnH, cuda->d_flushColumnE);
		err = clEnqueueNDRangeKernel(commands, kernel3, 1, NULL, &global, &local, 0, NULL, NULL);
		if (err != CL_SUCCESS)
			exit(0);
	}
	clFlush(commands);
	clFinish(commands);
	clReleaseKernel(kernel);
	clReleaseKernel(kernel2);
	clReleaseKernel(kernel3);

}





