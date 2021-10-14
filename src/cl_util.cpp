/*******************************************************************************
 *
 * Copyright (c) 2010-2014   Edans Sandes / Marco Figueiredo
 *
 * This file is part of CUDAlign - OpenCL.
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

#include <CL/opencl.h>
//#include <CL20/opencl.h>

#define DEBUG (0)
//#define PLAT CL_DEVICE_TYPE_CPU
#define PLAT CL_DEVICE_TYPE_GPU
#define ARRAYSIZE (600)

#include "cl_util.h"

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <math.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <iostream>
#include <fstream>
#include <sstream>
using namespace std;

#include "config.h"

static void clgetDeviceProperties(cl_platform_id platform, cl_int dev=-1);
cl_device_id clutGetMaxGflopsDeviceId();

/**
 * Allocates and copies sequence data in GPU.
 *
 * @param data the sequence data string.
 * @param len the length of the sequence.
 * @param padding_len extra padding length.
 * @param padding_char character to be used as padding.
 *
 * @return the pointer to the GPU memory allocated for the sequence.
 */
cl_mem clallocSeq(cl_context context, cl_device_id device, const char* data, int len, const int padding_len, char * padding_char) {

	cl_int errNum;
	cl_command_queue command;

	command = clCreateCommandQueue(context, device, 0, &errNum);

	if (!command)
		exit(0);
	cl_mem buffer = clalloc0(context, device, len+padding_len);

	errNum = clEnqueueWriteBuffer (command, buffer, CL_TRUE, 0, sizeof(char) * len, data, 0, NULL, NULL);
	for (cl_uint i=0; i<padding_len; i++)
		errNum = clEnqueueWriteBuffer (command, buffer, CL_TRUE, len+i, sizeof(char), padding_char, 0, NULL, NULL);


	clFinish(command);
	clReleaseCommandQueue(command);
	return (buffer);
}


/**
 * Allocates a vector in GPU and initializes it to zero.
 *
 * @param size length of the vector to be allocated.
 * @return the pointer to the GPU allocated memory.
 */
cl_mem clalloc0(cl_context context, cl_device_id device, int size) {
    cl_mem buffer;
    cl_int errNum;
    cl_command_queue commands;

    int * data;
    data = (int *)malloc(sizeof(int)*size);
    for (int i=0; i<size; i++)
       data[i] = 0;

    buffer = clCreateBuffer(
    		context,
    		CL_MEM_READ_WRITE,
    		sizeof(cl_int) * size,
    		NULL,
    		&errNum);

    if (errNum != CL_SUCCESS)
    	return (NULL);
    commands = clCreateCommandQueue(context, device, 0, &errNum);

    errNum = clEnqueueWriteBuffer(commands, buffer, CL_TRUE, 0, sizeof(cl_int) * size, data, 0, NULL, NULL);


    if (errNum != CL_SUCCESS)
        	return (NULL);
    clFinish(commands);
	if (DEBUG) printf("allocCuda(%d): %p\n", size, buffer);
	clReleaseCommandQueue(commands);
    return (buffer);
}

/**
 * Returns the number of multiprocessors in the selected GPU.
 * @return the number of multiprocessors in the selected GPU.
 */
cl_int clgetGPUMultiprocessors(cl_device_id deviceId) {

	cl_uint maxComputeUnits;

    if (deviceId != 0)
	  { clutilSafeCall(clGetDeviceInfo(deviceId,CL_DEVICE_MAX_COMPUTE_UNITS,sizeof(cl_uint),&maxComputeUnits,NULL));
	       return(maxComputeUnits);
	  }
    else
	   return(0);

}

/**
 * Prints the device properties.
 * @param file the file to print out the properties.
 */
void clprintDevProp(FILE * fp, cl_device_id deviceId) {
	char saida[30];
    unsigned long resplong;
    unsigned int respint;
    size_t workitem_size[3];
    unsigned int ws;
    int errnum;
    size_t workgroup_size;


	if (deviceId != 0)
	     {
		   errnum = clGetDeviceInfo(deviceId,CL_DRIVER_VERSION, 30,&saida,NULL);
		   fprintf(fp, "Major.Minor revision number:         %s\n", saida);
		   errnum = clGetDeviceInfo(deviceId,CL_DEVICE_NAME, 30,&saida,NULL);
           fprintf(fp, "Name:                               %s\n", saida);
           errnum = clGetDeviceInfo(deviceId,CL_DEVICE_GLOBAL_MEM_SIZE, sizeof(cl_ulong),&resplong,NULL);
           fprintf(fp, "Total registers per block:     %lu\n", resplong);
           errnum = clGetDeviceInfo(deviceId,CL_DEVICE_LOCAL_MEM_SIZE, sizeof(cl_ulong),&resplong,NULL);
           fprintf(fp, "Total local memory per block:     %lu\n", resplong);
           errnum = clGetDeviceInfo(deviceId, CL_DEVICE_MAX_WORK_GROUP_SIZE, sizeof(workgroup_size), &workgroup_size, NULL);
           ws = workgroup_size;
           fprintf(fp, "Maximum threads per block:     %u\n", ws);
           errnum = clGetDeviceInfo(deviceId, CL_DEVICE_MAX_WORK_ITEM_SIZES, sizeof(workitem_size), &workitem_size, NULL);
           for (int i = 0; i < 3; ++i)
              { ws = workitem_size[i];
        	   fprintf(fp, "Maximum dimension %n of block:   %u\n", &i,ws);
              }
           errnum = clGetDeviceInfo(deviceId,CL_DEVICE_MAX_CLOCK_FREQUENCY, sizeof(cl_uint),&respint,NULL);
           fprintf(fp, "Clock rate:                    %u\n", respint);
           errnum = clGetDeviceInfo(deviceId,CL_DEVICE_MAX_CONSTANT_BUFFER_SIZE, sizeof(cl_ulong),&resplong,NULL);
           fprintf(fp, "Total constant memory:     %lu\n", resplong);
           errnum = clGetDeviceInfo(deviceId,CL_DEVICE_MAX_COMPUTE_UNITS,sizeof(cl_uint),&respint,NULL);
           fprintf(fp, "Number of multiprocessors:     %u\n", respint);
           fflush(fp);
           return;
	}

}

/**
 * Returns the compute capability of the selected GPU.
 * @return the compute capability in the integer format (210 means
 * version 2.1)
 */
int clgetDevCapability(cl_device_id deviceIdx) {
	char * pos;
	char * majorstr;
	cl_uint major;
	char * minorstr;
	cl_uint minor;
	cl_uint j = 0;
	cl_uint l = 0;
	char saida[10240];
	size_t size;
	cl_int status;

	if (deviceIdx != 0)
	 {
		status = clGetDeviceInfo(deviceIdx,CL_DRIVER_VERSION, sizeof(saida),saida,&size);
		major = saida[0] - '0';
	   minor = saida[2] - '0';
	   return (major*100+minor*10);
	  }

}


/**
 * Returns the compiled compute capability.
 * @return the compute capability in the integer format (210 means
 * version 2.1)
 */
int clgetCompiledCapability() {
	return (120);
}

/**
 * Prints all available GPUs.
 * @param file the file to print out the properties.
 */
void clprintGPUDevices(FILE * file) {
    int count;

    cl_uint numDevices;
	cl_device_id deviceIds[10];
	size_t size;
	cl_uint numPlatforms;
	cl_uint maxComputeUnits;
	cl_platform_id * platform;
    cl_int errnum;
    cl_uint devid;
    cl_ulong resplong;
    char saida[30];


	clutilSafeCall(clGetPlatformIDs(0, NULL, &numPlatforms));
	platform = (cl_platform_id *)alloca(sizeof(cl_platform_id) * numPlatforms);

    for (cl_uint i = 0; i < numPlatforms; i++)
		 {  errnum = clGetDeviceIDs(platform[i], PLAT,	0,	NULL,	&numDevices);
		    if (errnum == CL_DEVICE_NOT_FOUND  || errnum CL_DEVICE_NOT_AVAILABLE || numDevices < 1)
                fprintf (file,"No available GPU device found \n");
            else
            	{ fprintf (file,"Detected GPUs: %u", numDevices);
            	  errnum = clGetDeviceIDs(platform[i], PLAT,	0,	deviceIds,	&numDevices);
            	  fprintf (file,"ID: NAME (RAM)\n");
            	  fprintf (file,"---------------------------\n");
		          for (int j=1; j<numDevices; j++)
		            { bool compatible = (clgetCompiledCapability() <= clgetDevCapability(deviceIds[j]));
			          if (compatible)
			        	  fprintf (file,"%n",(&j+1));
			          else
				         fprintf(file,  "    ");
			          errnum = clGetDeviceInfo(deviceIds[j],CL_DEVICE_NAME, 30,&saida,NULL);
			          errnum = clGetDeviceInfo(deviceIds[j],CL_DEVICE_GLOBAL_MEM_SIZE, sizeof(cl_ulong),&resplong,NULL);
			          fprintf (file,"%s %f MB \n",saida, (resplong/1024.0/1024.0));
					}
		         fprintf(file,"\n");
                }
         }
    fflush(file);
}


/**
 * Returns the GPU weights for all available GPUs.
 *
 * @param proportion vector that will receive the weights.
 * @param n maximum number of weights to be stored in the vector.
 * @return the number of weights stored in the vector.
 */
int clgetGPUWeights(int* proportion, int n) {
	/*
	 * When any CUDA runtime function is called, the CUDA context is initialized.
	 * If we call a fork after this initialization, the same context is shared
	 * among the processes, what causes initialization errors and abnormal
	 * execution. The getGPUWeights function are called before the fork procedure.
	 * in the libmasa_entry_point, so, we must obtain all the GPU weights/proportion
	 * using another process. This method fork a process only to obtain these
	 * CUDA dependent values and the child process dies with its own CUDA context.
	 * This context is not shared with the parent process, so we can continue
	 * the Aligner execution without any problem.
	 */

	/* Communication using PIPE */
	int pipe_fd[2];
	cl_device_id deviceIds[ARRAYSIZE];
	char * minorstr;
	char * majorstr;
	cl_uint major;
	cl_uint minor;
	cl_platform_id * platform;
	cl_uint j = 0;
	cl_uint l = 0;
	char saida[30];

	if (pipe(pipe_fd)) {
		fprintf(stderr, "ERROR: GPU weights could not be obtained (1).\n");
		exit(-1);
	}

	int pid = fork();
	if (pid == 0) {
		/* Child */
		close(pipe_fd[0]);
		cl_uint numPlatforms;
		cl_uint numDevices;
		cl_uint clockrate;
		cl_uint mproccount;

		clutilSafeCall(clGetPlatformIDs(0, NULL, &numPlatforms));
		platform = (cl_platform_id *)alloca(sizeof(cl_platform_id) * numPlatforms);

		for (cl_uint i = 0; i < numPlatforms; i++)
		  { clutilSafeCall(clGetDeviceIDs(platform[i], PLAT,	0,	deviceIds,	&numDevices));
		    int cores;
	        clutilSafeCall(clGetDeviceInfo(deviceIds[1],CL_DRIVER_VERSION, 30,saida,NULL));
	        printf("\n\n %s \n\n",saida);
	        while (saida[j]!='.')
	             { majorstr[j]=saida[j];
	               j++;
	             }
	        majorstr[l]='\0';
	        major = atoi(majorstr);
	        j++;
	        while (saida[j] != '\0')
	           { minorstr[l]=saida[j];
	        	 l++;
	        	 j++;
	           }
	        minorstr[l] = '\0';
	        minor = atoi(minorstr);

	        if (major <= 1) {
	        	cores = 8;
	        } else if (major == 2 && minor == 0) {
	        	cores = 32;
	        } else if (major == 2 && minor == 1) {
	        	cores = 48;
	        } else {
	        	cores = 192;
	        }
	        if (clgetCompiledCapability() <= (major*100+minor*10))
	          { int speed;
	            clutilSafeCall(clGetDeviceInfo(deviceIds[0],CL_DEVICE_MAX_CLOCK_FREQUENCY, sizeof(cl_uint),&clockrate,NULL));
	            clutilSafeCall(clGetDeviceInfo(deviceIds[0],CL_DEVICE_MAX_COMPUTE_UNITS,sizeof(cl_uint),&mproccount,NULL));
	            speed = clockrate*mproccount*cores/1000;
	        	if (write(pipe_fd[1], &speed, sizeof(speed)) == -1) {
	    			fprintf(stderr, "ERROR: GPU weights could not be obtained (2).\n");
	    			exit(1);
	        	}

	        }
		}
		//cudaDeviceReset();
		close(pipe_fd[1]);
		exit(7);
	} else {
		/* Parent */
		close(pipe_fd[1]);
		int val;
		int count = 0;
		while (read(pipe_fd[0], &val, sizeof(val)) > 0) {
			if (count < n) {
				proportion[count++] = val;
			}
		}
		close(pipe_fd[0]);
		waitpid(pid, NULL, 0); // Join processes
		return count;
	}

}


/**
 * Returns a vector containing the IDs of the supported GPUs.
 *
 * @param ids vector that will receive the IDs.
 * @param n maximum number of IDs to be stored in the vector.
 * @return the number of IDs stored in the vector.
 */
int clgetAvailableGPU(int* ids, int n) {
	cl_uint count = 0;
	cl_uint numDevices;
	cl_device_id deviceIds[ARRAYSIZE];
	size_t size;
	cl_uint numPlatforms;
	cl_uint maxComputeUnits;
	cl_platform_id * platform;
    int j = 0;

	clutilSafeCall(clGetPlatformIDs(0, NULL, &numPlatforms));
	platform = (cl_platform_id *)alloca(sizeof(cl_platform_id) * numPlatforms);

	for (cl_uint i = 0; i < numPlatforms; i++)
	  { clutilSafeCall(clGetDeviceIDs(platform[i], PLAT,	0,	NULL,	&numDevices));
	    if (numDevices > 0)
	    	{ for (count=1; count < numDevices; count++)
			     { if ((clgetCompiledCapability() <= clgetDevCapability(deviceIds[count])) && (numDevices < n))
			    	 ids[j++] = count;
			     }
			  return(j);
	    	}

	  }
	}

/**
 * Selects a GPU for the CUDA execution.
 * @param id id of the selected GPU.
 */
cl_device_id clselectGPU(cl_device_id id) {
	cl_device_id deviceId;
	cl_platform_id glplatform;

    if (id == NULL) {
        //deviceId = clutGetMaxGflopsDeviceId();
    	glplatform = clSetupPlatform();
    	deviceId = clSetupDevice(glplatform);

    } else {
        deviceId = id;
    }

    return (deviceId);
}




///**
// * Returns the memory usage statistics.
// *
// * @param usedMem receives the current used memory.
// * @param totalMem receives the total memory of the device.
// */
//void getMemoryUsage(size_t* usedMem, size_t* totalMem) {
//	size_t myFreeMem;
//	size_t myTotalMem;
//	cudaFree(0); // Ensures the CUDA context creation. Otherwise cuMemGetInfo (driver API) will return zero.
//	cuMemGetInfo(&myFreeMem, &myTotalMem);  ///// not implemented in OpenCL
//	if (usedMem != NULL) {
//		*usedMem = myTotalMem-myFreeMem;
//	}
//	if (totalMem != NULL) {
//		*totalMem = myTotalMem;
//	}
//}
//


///**
// * Returns the cudaDeviceProp structure with the properties of a given GPU.
// *
// * @param dev id of the GPU to be queried.
// * @return the cudaDeviceProp structure containing the properties of the GPU.
// */
clDeviceProp clgetDeviceProperties(cl_platform_id platform, cl_device_id deviceId) {
    clDeviceProp devProp;
    char saida[30];
    cl_ulong resplong;
    cl_uint respint;
    size_t workitem_size[3];
    cl_int errnum;
    size_t workgroup_size;

    if (deviceId == NULL) {
    	deviceId = clSetupDevice(platform);
    }

    errnum = clGetDeviceInfo(deviceId,CL_DRIVER_VERSION, 30,&saida,NULL);
    errnum = clGetDeviceInfo(deviceId,CL_DEVICE_NAME, 30,&saida,NULL);
    strcpy (devProp.name, saida);
    errnum = clGetDeviceInfo(deviceId,CL_DEVICE_GLOBAL_MEM_SIZE, sizeof(cl_ulong),&resplong,NULL);
    devProp.totalGlobalMem = resplong;
    errnum = clGetDeviceInfo(deviceId,CL_DEVICE_LOCAL_MEM_SIZE, sizeof(cl_ulong),&resplong,NULL);
    devProp.sharedMemPerBlock = resplong;
    errnum = clGetDeviceInfo(deviceId, CL_DEVICE_MAX_WORK_GROUP_SIZE, sizeof(workgroup_size), &workgroup_size, NULL);
    devProp.regsPerBlock = workgroup_size;
    errnum = clGetDeviceInfo(deviceId, CL_DEVICE_MAX_WORK_ITEM_SIZES, sizeof(workitem_size), &workitem_size, NULL);
    for (int i = 0; i < 3; ++i)
    	devProp.maxThreadsDim[i] = workitem_size[i];
    errnum = clGetDeviceInfo(deviceId,CL_DEVICE_MAX_CLOCK_FREQUENCY, sizeof(cl_uint),&respint,NULL);
    devProp.clockRate = respint;
    errnum = clGetDeviceInfo(deviceId,CL_DEVICE_MAX_CONSTANT_BUFFER_SIZE, sizeof(cl_ulong),&resplong,NULL);
    devProp.totalConstMem = resplong;
    errnum = clGetDeviceInfo(deviceId,CL_DEVICE_MAX_COMPUTE_UNITS,sizeof(cl_uint),&respint,NULL);
    devProp.multiProcessorCount = respint;
    return (devProp);
}



/**
 *  This function returns the best GPU (with maximum GFLOPS).
 *  @remarks copied from cutil_inline_runtime.h - Copyright 1993-2010 NVIDIA Corporation.
 */
cl_device_id clutGetMaxGflopsDeviceId() {
	int current_device   = 0, sm_per_multiproc = 0;
	int max_compute_perf = 0, max_perf_device  = 0;
	int device_count     = 0, best_SM_arch     = 0;
    int arch_cores_sm[3] = { 1, 8, 32 };
    cl_platform_id platform;
    cl_int errnum;
    cl_uint numDevices;
    cl_uint respint;
    cl_device_id deviceIds[ARRAYSIZE];
    char saida[30];
    char * majorstr;
    char * minorstr;
    int major, minor;
    int l, j=0;
    cl_uint multiProcessorCount, clockRate;
    int compute_perf;


    platform = clSetupPlatform();

    errnum = clGetDeviceIDs(platform, PLAT,	0,	NULL,	&numDevices);
    if (errnum == CL_DEVICE_NOT_FOUND  || errnum == CL_DEVICE_NOT_AVAILABLE || numDevices < 1)
       return (NULL);

	clutilSafeCall(clGetDeviceIDs(platform, PLAT,	ARRAYSIZE,	deviceIds,	&numDevices));

	// Find the best major SM Architecture GPU device
	cout << "\n  numDevices:" << numDevices;
	for (int i=0; i<numDevices; i++) {
    	clutilSafeCall(clGetDeviceInfo(deviceIds[i],CL_DRIVER_VERSION, 30,saida,NULL));

    	cout << "\n\n" << saida << "\n\n";

    	major = saida[0] - '0';
    	minor = saida[2] - '0';

    	cout << "\n Cheguei aqui...Major:" << major << " minor: " << minor << " best: " << best_SM_arch;

		if (major > 0 && major < 9999)
			if (best_SM_arch < major)
				best_SM_arch = major;

		cout << "\n Cheguei aqui...Major:" << major << " minor: " << minor << " best: " << best_SM_arch;

		cout << "\n  i:" << major;
		cout << "\n  numDevices:" << major;
		cout << "Cheguei aqui..." << major;
		cout << "Cheguei aqui...";
		cout << "Cheguei aqui...";
    }

    cout << "Cheguei aqui...";

    // Find the best CUDA capable GPU device
	current_device = 0;
	j=0; l=0;
	while( current_device < numDevices ) {
		clutilSafeCall(clGetDeviceInfo(deviceIds[current_device],CL_DRIVER_VERSION, 30,saida,NULL));
		while (saida[j]!='.')
		   		{ majorstr[j]=saida[j];
		   		  j++;
		  		  }
		majorstr[l]='\0';
		major = atoi(majorstr);
		major = saida[0] - '0';
		j++;
		while (saida[j] != '\0')
		    { minorstr[l]=saida[j];
		      l++;
		      j++;
		     }
		 minorstr[l] = '\0';
		 minor = saida[2] - '0';

		 cout << "\n\n Major: " << major << "\n Minor: " << minor;

		if (major == 9999 && minor == 9999) {
		    sm_per_multiproc = 1;
		} else if (major <= 2) {
			sm_per_multiproc = arch_cores_sm[major];
		} else {
			sm_per_multiproc = arch_cores_sm[2];
		}
		errnum = clGetDeviceInfo(deviceIds[current_device],CL_DEVICE_MAX_COMPUTE_UNITS,sizeof(cl_uint),&respint,NULL);
		multiProcessorCount = respint;
		errnum = clGetDeviceInfo(deviceIds[current_device],CL_DEVICE_MAX_CLOCK_FREQUENCY, sizeof(cl_uint),&respint,NULL);
		clockRate = respint;
		compute_perf  = multiProcessorCount * sm_per_multiproc * clockRate;
		if( compute_perf  > max_compute_perf ) {
            // If we find GPU with SM major > 2, search only these
			if ( best_SM_arch > 2 ) {
				// If our device==dest_SM_arch, choose this, or else pass
				if (major == best_SM_arch) {
					max_compute_perf  = compute_perf;
					max_perf_device   = current_device;
				}
			} else {
				max_compute_perf  = compute_perf;
				max_perf_device   = current_device;
			}
		}
		++current_device;
	}
	return (deviceIds[max_perf_device]);
}

//Create context for the informed Platform - type GPU
cl_context CreateContext(cl_platform_id PlatformId)
{
    cl_int errNum;
    cl_uint numPlatforms;
    cl_context context = NULL;

    cl_context_properties contextProperties[] =
    {
        CL_CONTEXT_PLATFORM,
        (cl_context_properties)PlatformId,
        0
    };
    context = clCreateContextFromType(contextProperties, PLAT,NULL, NULL, &errNum);
    if (errNum != CL_SUCCESS)
       std::cout << "Could not create GPU context..." << std::endl;

    return (context);
}

///
//  Create a command queue on the first device available on the context
//
cl_command_queue CreateCommandQueue(cl_context context, cl_device_id *device)
{
    cl_int errNum;
    cl_device_id *devices;
    cl_command_queue commandQueue = NULL;
    size_t deviceBufferSize = -1;

    // First get the size of the devices buffer
    errNum = clGetContextInfo(context, CL_CONTEXT_DEVICES, 0, NULL, &deviceBufferSize);
    if (errNum != CL_SUCCESS)
    {
        std::cerr << "Failed call to clGetContextInfo(...,GL_CONTEXT_DEVICES,...)";
        return NULL;
    }

    if (deviceBufferSize <= 0)
    {
        std::cerr << "No devices available.";
        return NULL;
    }

    // Allocate memory for the devices buffer
    devices = new cl_device_id[deviceBufferSize / sizeof(cl_device_id)];
    errNum = clGetContextInfo(context, CL_CONTEXT_DEVICES, deviceBufferSize, devices, NULL);
    if (errNum != CL_SUCCESS)
    {
        delete [] devices;
        std::cerr << "Failed to get device IDs";
        return NULL;
    }

    // In this example, we just choose the first available device.  In a
    // real program, you would likely use all available devices or choose
    // the highest performance device based on OpenCL device queries
    commandQueue = clCreateCommandQueue(context, devices[0], 0, NULL);
    if (commandQueue == NULL)
    {
        delete [] devices;
        std::cerr << "Failed to create commandQueue for device 0";
        return NULL;
    }

    *device = devices[0];
    delete [] devices;
    return commandQueue;
}

///
//  Create an OpenCL program from the kernel source file
//
cl_program CreateProgram(cl_context context, cl_device_id device, const char* fileName)
{
    cl_int errNum;
    cl_program program;

    std::ifstream kernelFile(fileName, std::ios::in);
    if (!kernelFile.is_open())
    {
        std::cerr << "Failed to open file for reading: " << fileName << std::endl;
        return NULL;
    }

    std::ostringstream oss;
    oss << kernelFile.rdbuf();

    std::string srcStdStr = oss.str();
    const char *srcStr = srcStdStr.c_str();
    program = clCreateProgramWithSource(context, 1,
                                        (const char**)&srcStr,
                                        NULL, NULL);
    if (program == NULL)
    {
        std::cerr << "Failed to create CL program from source." << std::endl;
        return NULL;
    }

    errNum = clBuildProgram(program, 0, NULL, NULL, NULL, NULL);
    if (errNum != CL_SUCCESS)
    {
        // Determine the reason for the error
        char buildLog[16384];
        clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG,
                              sizeof(buildLog), buildLog, NULL);

        std::cerr << "Error in kernel: " << std::endl;
        std::cerr << buildLog;
        clReleaseProgram(program);
        return NULL;
    }

    return program;
}

///
//  Cleanup any created OpenCL resources
//
void Cleanup(cl_context context, cl_program program)
{

    if (program != 0)
        clReleaseProgram(program);

    if (context != 0)
        clReleaseContext(context);

}

/**
 * Detect and return the id of the first GPU device
 *
 * @return id of the GPU device.
 */

cl_platform_id clSetupPlatform() {

	size_t size;
	cl_uint numPlatforms;
	cl_uint maxComputeUnits;
	cl_platform_id platformIds[ARRAYSIZE];
	//cl_platform_id * platformIds;
	cl_uint numDevices;

	clutilSafeCall(clGetPlatformIDs(0, NULL, &numPlatforms));
	clutilSafeCall(clGetPlatformIDs(numPlatforms, platformIds, NULL));
	for (cl_uint i = 0; i < numPlatforms; i++)
		 { clutilSafeCall(clGetDeviceIDs(platformIds[i], PLAT,	0,	NULL,	&numDevices));
		   if (numDevices > 0)
		     return(platformIds[i]);
		  }

	return(0);

}

/**
 * Detect and return the id of the first GPU Platform
 *
 * @return id of the GPU platform.
 */


cl_device_id clSetupDevice(cl_platform_id platform) {

	cl_uint numDevices;
	cl_device_id deviceIds[ARRAYSIZE];
	size_t size;

	if (platform != 0)
		     { clutilSafeCall(clGetDeviceIDs(platform, PLAT,1, &deviceIds[0],NULL));
		       return(deviceIds[0]);
		     }

	 return(0);

}


const char * clgetErrorStr(cl_int e)
{
  switch (e)
  {
    case CL_SUCCESS: return "success";
    case CL_DEVICE_NOT_FOUND: return "device not found";
    case CL_DEVICE_NOT_AVAILABLE: return "device not available";
#if !(defined(CL_PLATFORM_NVIDIA) && CL_PLATFORM_NVIDIA == 0x3001)
    case CL_COMPILER_NOT_AVAILABLE: return "device compiler not available";
#endif
    case CL_MEM_OBJECT_ALLOCATION_FAILURE: return "mem object allocation failure";
    case CL_OUT_OF_RESOURCES: return "out of resources";
    case CL_OUT_OF_HOST_MEMORY: return "out of host memory";
    case CL_PROFILING_INFO_NOT_AVAILABLE: return "profiling info not available";
    case CL_MEM_COPY_OVERLAP: return "mem copy overlap";
    case CL_IMAGE_FORMAT_MISMATCH: return "image format mismatch";
    case CL_IMAGE_FORMAT_NOT_SUPPORTED: return "image format not supported";
    case CL_BUILD_PROGRAM_FAILURE: return "build program failure";
    case CL_MAP_FAILURE: return "map failure";

    case CL_INVALID_VALUE: return "invalid value";
    case CL_INVALID_DEVICE_TYPE: return "invalid device type";
    case CL_INVALID_PLATFORM: return "invalid platform";
    case CL_INVALID_DEVICE: return "invalid device";
    case CL_INVALID_CONTEXT: return "invalid context";
    case CL_INVALID_QUEUE_PROPERTIES: return "invalid queue properties";
    case CL_INVALID_COMMAND_QUEUE: return "invalid command queue";
    case CL_INVALID_HOST_PTR: return "invalid host ptr";
    case CL_INVALID_MEM_OBJECT: return "invalid mem object";
    case CL_INVALID_IMAGE_FORMAT_DESCRIPTOR: return "invalid image format descriptor";
    case CL_INVALID_IMAGE_SIZE: return "invalid image size";
    case CL_INVALID_SAMPLER: return "invalid sampler";
    case CL_INVALID_BINARY: return "invalid binary";
    case CL_INVALID_BUILD_OPTIONS: return "invalid build options";
    case CL_INVALID_PROGRAM: return "invalid program";
    case CL_INVALID_PROGRAM_EXECUTABLE: return "invalid program executable";
    case CL_INVALID_KERNEL_NAME: return "invalid kernel name";
    case CL_INVALID_KERNEL_DEFINITION: return "invalid kernel definition";
    case CL_INVALID_KERNEL: return "invalid kernel";
    case CL_INVALID_ARG_INDEX: return "invalid arg index";
    case CL_INVALID_ARG_VALUE: return "invalid arg value";
    case CL_INVALID_ARG_SIZE: return "invalid arg size";
    case CL_INVALID_KERNEL_ARGS: return "invalid kernel args";
    case CL_INVALID_WORK_DIMENSION: return "invalid work dimension";
    case CL_INVALID_WORK_GROUP_SIZE: return "invalid work group size";
    case CL_INVALID_WORK_ITEM_SIZE: return "invalid work item size";
    case CL_INVALID_GLOBAL_OFFSET: return "invalid global offset";
    case CL_INVALID_EVENT_WAIT_LIST: return "invalid event wait list";
    case CL_INVALID_EVENT: return "invalid event";
    case CL_INVALID_OPERATION: return "invalid operation";
    case CL_INVALID_GL_OBJECT: return "invalid gl object";
    case CL_INVALID_BUFFER_SIZE: return "invalid buffer size";
    case CL_INVALID_MIP_LEVEL: return "invalid mip level";

#if defined(cl_khr_gl_sharing) && (cl_khr_gl_sharing >= 1)
    case CL_INVALID_GL_SHAREGROUP_REFERENCE_KHR: return "invalid gl sharegroup reference number";
#endif

#ifdef CL_VERSION_1_1
    case CL_MISALIGNED_SUB_BUFFER_OFFSET: return "misaligned sub-buffer offset";
    case CL_EXEC_STATUS_ERROR_FOR_EVENTS_IN_WAIT_LIST: return "exec status error for events in wait list";
    case CL_INVALID_GLOBAL_WORK_SIZE: return "invalid global work size";
#endif

    default: return "invalid/unknown error code";
  }
}


inline void __clSafeCall( cl_int err, const char *file, const int line )
{
    if( CL_SUCCESS != err) {
		fprintf(stderr, "%s(%i) : clSafeCall() Runtime API error: %s. \n",
                file, line, clgetErrorStr(err));
        exit(-1);
    }
}

void clgetMemoryUsage(size_t* freeMem, size_t* totalMem)
{
 return;
}

