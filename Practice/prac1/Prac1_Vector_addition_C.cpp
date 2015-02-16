#include "stdafx.h"

const char* programSource =
"__kernel\n"
"void vecadd(__global int* A, __global int* B, __global int* C)\n"
"{\n"
"\n"
"	// Get workitem's unique ID.\n"
"	int idx = get_global_id(0);\n"
"\n"
"	// Add 'A's and 'B's element of given place and save it to 'C's appropriate place.\n"
"	C[idx] = A[idx]+B[idx];\n"
"}\n"
;
int main() {
	// This code runs at OpenCL host

	// Host Data.
	int* A = NULL; // Input array.
	int* B = NULL; // Input array.
	int* C = NULL; // Output array.

	// Each array's elements.
	const int elements = 2048;

	// Calculate the size of data.
	size_t datasize = sizeof(int)*elements;

	// Space allocation for input/output data.
	A = (int*)malloc(datasize);
	B = (int*)malloc(datasize);
	C = (int*)malloc(datasize);

	// Initialization of input data.
	int i;
	for (i = 0; i < elements; i++)
	{
		A[i] = i;
		B[i] = i;
	}

	// Will be used for each API call check.
	cl_int status;

	// Get number of platforms.
	cl_uint numPlatforms = 0;
	status = clGetPlatformIDs(0, NULL, &numPlatforms);

	// Allocate enough space for each platform.
	cl_platform_id *platforms = NULL;
	platforms = (cl_platform_id*)malloc(numPlatforms*sizeof(cl_platform_id));

	// Get platform information.
	status = clGetPlatformIDs(numPlatforms, platforms, NULL);

	// Get number of devices.
	cl_uint numDevices = 0;
	status = clGetDeviceIDs(platforms[0], CL_DEVICE_TYPE_ALL, 0, NULL, &numDevices);

	// Allocate enough space for each device.
	cl_device_id* devices;
	devices = (cl_device_id*)malloc(numDevices*sizeof(cl_device_id));

	// Get device information.
	status = clGetDeviceIDs(platforms[0], CL_DEVICE_TYPE_ALL, numDevices, devices, NULL);

	// Make context and connect with device.
	cl_context context;
	context = clCreateContext(NULL, numDevices, devices, NULL, NULL, &status);

	// Make command queue and connect with device.
	cl_command_queue cmdQueue;
	cmdQueue = clCreateCommandQueue(context, devices[0], 0, &status);

	// Create buffer object contains data from host array A.
	cl_mem bufA;
	bufA = clCreateBuffer(context, CL_MEM_READ_ONLY, datasize, NULL, &status);

	// Create buffer object contains data from host array B.
	cl_mem bufB;
	bufB = clCreateBuffer(context, CL_MEM_READ_ONLY, datasize, NULL, &status);

	// Create buffer object for storing output data.
	cl_mem bufC;
	bufC = clCreateBuffer(context, CL_MEM_WRITE_ONLY, datasize, NULL, &status);

	// Write input array A to device buffer A.
	status = clEnqueueWriteBuffer(cmdQueue, bufA, CL_FALSE, 0, datasize, A, 0, NULL, NULL);

	// Write input array B to device buffer B.
	status = clEnqueueWriteBuffer(cmdQueue, bufB, CL_FALSE, 0, datasize, B, 0, NULL, NULL);

	// Create program that has source code.
	cl_program program = clCreateProgramWithSource(context, 1, (const char**)&programSource, NULL, &status);

	// Build(compile) program for devices.
	status = clBuildProgram(program, numDevices, devices, NULL, NULL, NULL);

	// Create vector addition kernel.
	cl_kernel kernel;
	kernel = clCreateKernel(program, "vecadd", &status);

	// Connect input/output buffer with kernel.
	status = clSetKernelArg(kernel, 0, sizeof(cl_mem), &bufA);
	status = clSetKernelArg(kernel, 1, sizeof(cl_mem), &bufB);
	status = clSetKernelArg(kernel, 2, sizeof(cl_mem), &bufC);

	// Define 'index space'(global work size) of work item, for run.
	// Work group's size is not necessary but it can be used.
	size_t globalWorkSize[1];

	// 'element' exists in work item.
	globalWorkSize[0] = elements;

	// To run the code, run the kernel.
	status = clEnqueueNDRangeKernel(cmdQueue, kernel, 1, NULL, globalWorkSize, NULL, 0, NULL, NULL);

	// Read from device output buffer to host output buffer.
	clEnqueueReadBuffer(cmdQueue, bufC, CL_TRUE, 0, datasize, C, 0, NULL, NULL);

	// Verification of output.
	int result = 1;
	for (i = 0; i < elements; i++)
	{
		if (C[i] != i + i)
		{
			result = 0;
			break;
		}
	}

	if (result)
	{
		printf("Output is correct\n");
	}
	else
	{
		printf("Output is incorrect\n");
	}

	// Release OpenCL resources.
	clReleaseKernel(kernel);
	clReleaseProgram(program);
	clReleaseCommandQueue(cmdQueue);
	clReleaseMemObject(bufA);
	clReleaseMemObject(bufB);
	clReleaseMemObject(bufC);
	clReleaseContext(context);

	// Release host resources.
	free(A);
	free(B);
	free(C);
	free(platforms);
	free(devices);

	return 0;

}