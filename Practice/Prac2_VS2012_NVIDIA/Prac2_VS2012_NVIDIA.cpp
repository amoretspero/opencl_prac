#include <stdio.h>
#include <stdlib.h>

#include <CL/cl.h>
#include <CL/opencl.h>

#define width_A 1024
#define height_A 1024
#define width_B 1024
#define height_B 1024
#define width_C width_B
#define height_C height_A
#define MAX_SOURCE_SIZE 0x100000
#define _CRT_SECURE_NO_WARNINGS
#define CL_LOG_ERRORS = stdout

void randomInit(float* data, int size)
{
	int i;
	for (i = 0; i < size; ++i)
	{
		data[i] = rand() / (float)RAND_MAX;
	}
}

char* kernel_source_hard_coded =
"// kernel code for simple matrix multiplication.\n"
"// For valid matrix multiplication, set widthA = heightB.\n"
"__kernel void simpleMultiply (__global float* outputC, int widthA, int heightA, int widthB, int heightB, __global float* inputA, __global float* inputB)\n"
"{\n"
"	// Get Y-direction global position.\n"
"	int row = get_global_id(1);\n"
"	// Get X-direction global position.\n"
"	int col = get_global_id(0);\n"
"   float sum = 0.0f;\n"
"   // Calculate the result for matrix C's one element.\n"
"   int i;\n"
"   for (i=0; i < widthA; i++)\n"
"   {\n"
"		sum += inputA[row * widthA + i] * inputB[i * widthB + col];\n"
"   }\n"
"   outputC[row * widthB + col] = sum;\n"
"}\n";

int main(int argc, char** argv)
{

	// Allocate host memory for matrix A.
	int size_A = width_A * height_A;
	int mem_size_A = sizeof(float) * size_A;
	float* matrix_A = (float*)malloc(mem_size_A);

	// Allocate host memory for matrix B.
	int size_B = width_B * height_B;
	int mem_size_B = sizeof(float) * size_B;
	float* matrix_B = (float*)malloc(mem_size_B);

	// Initialize host memory to make matrix A and B to be initialized.
	randomInit(matrix_A, size_A);
	randomInit(matrix_B, size_B);

	// Print out matrices A and B to check.
	printf("\n\nMatrix_A\n");
	/*int cnt_A = 0;
	for (cnt_A = 0; cnt_A < size_A; cnt_A++)
	{
		printf("%f ", matrix_A[cnt_A]);
		if (((cnt_A + 1) % width_A) == 0)
		{
			printf("\n");
		}
	}*/
	printf("\n\nMatrix_B\n");
	/*int cnt_B = 0;
	for (cnt_B = 0; cnt_B < size_B; cnt_B++)
	{
		printf("%f ", matrix_B[cnt_B]);
		if (((cnt_B + 1) % width_B) == 0)
		{
			printf("\n");
		}
	}*/

	// Allocate host memory for the result matrix C.
	int size_C = width_C * height_C;
	int mem_size_C = sizeof(float) * size_C;
	float* matrix_C = (float*)malloc(mem_size_C);

	// Set seed for rand()
	srand(2015);

	// Integer for error indication.
	cl_int ciErrNum;

	// Use first platform.
	cl_uint numPlatforms = 0;
	ciErrNum = clGetPlatformIDs(0, NULL, &numPlatforms);
	cl_platform_id* platform = NULL;
	platform = (cl_platform_id*)malloc(numPlatforms*sizeof(cl_platform_id));
	ciErrNum = clGetPlatformIDs(numPlatforms, platform, NULL);
	//cl_platform platform;
	//ciErrNum = clGetPlatformIDs(1, &platform, NULL);

	// Use first device.
	cl_uint numDevices = 0;
	ciErrNum = clGetDeviceIDs(platform[0], CL_DEVICE_TYPE_ALL, 0, NULL, &numDevices);
	cl_device_id* devices;
	devices = (cl_device_id*)malloc(numDevices*sizeof(cl_device_id));
	ciErrNum = clGetDeviceIDs(platform[0], CL_DEVICE_TYPE_ALL, numDevices, devices, NULL);
	//cl_device_id device;
	//ciErrNum = clGetDeviceIDs(platform, CL_DEVICE_TYPE_ALL, 1, &device, NULL);

	cl_context_properties cps[3] = { CL_CONTEXT_PLATFORM, (cl_context_properties)platform[0], 0 };

	// Create context.
	cl_context ctx = clCreateContext(NULL, numDevices, devices, NULL, NULL, &ciErrNum);

	// Create command queue.
	cl_command_queue myqueue = clCreateCommandQueue(ctx, devices[0], 0, &ciErrNum);

	// Load the OpenCL kernel.
	FILE* opencl_kernel;
	char* kernel_source;
	size_t kernel_size;

	fopen_s(&opencl_kernel, "Prac2_VS2012_NVIDIA.cl", "r");
	if (!opencl_kernel)
	{
		printf("Failed to load kernel.\n");
		exit(1);
	}

	kernel_source = (char*)malloc(MAX_SOURCE_SIZE);
	kernel_size = fread(kernel_source, 1, MAX_SOURCE_SIZE, opencl_kernel);
	fclose(opencl_kernel);

	// Declare A, B, C and assumes that they are initialized float array. (Declared above.)
	// Locate spaces for matrix A on device.
	cl_mem bufferA = clCreateBuffer(ctx, CL_MEM_READ_ONLY, mem_size_A, NULL, &ciErrNum);

	// Copy matrix A to device.
	ciErrNum = clEnqueueWriteBuffer(myqueue, bufferA, CL_TRUE, 0, mem_size_A, (void*)matrix_A, 0, NULL, NULL);

	// Locate spaces for matrix B on device.
	cl_mem bufferB = clCreateBuffer(ctx, CL_MEM_READ_ONLY, mem_size_B, NULL, &ciErrNum);

	// Copy matrix B to device.
	ciErrNum = clEnqueueWriteBuffer(myqueue, bufferB, CL_TRUE, 0, mem_size_B, (void*)matrix_B, 0, NULL, NULL);

	// Locate spaces for matrix C on device.
	cl_mem bufferC = clCreateBuffer(ctx, CL_MEM_WRITE_ONLY, mem_size_C, NULL, &ciErrNum);

	cl_program matrix_mul_prog = clCreateProgramWithSource(ctx, 1, (const char**)&kernel_source, (const size_t*)&kernel_size, &ciErrNum);

	// Compile program.
	// Gives NULL to 'device_list' in order that compile should done to all devices in context.
	ciErrNum = clBuildProgram(matrix_mul_prog, numDevices, devices, NULL, NULL, NULL);

	// Build kernel.
	cl_kernel matrix_mul_kernel = clCreateKernel(matrix_mul_prog, "simpleMultiply", &ciErrNum);

	// Configure kernel arguments.
	int wA = width_A;
	int hA = height_A;
	int wB = width_B;
	int hB = height_B;
	clSetKernelArg(matrix_mul_kernel, 0, sizeof(cl_mem), (void*)&bufferC);
	clSetKernelArg(matrix_mul_kernel, 1, sizeof(cl_int), (void*)&wA);
	clSetKernelArg(matrix_mul_kernel, 2, sizeof(cl_int), (void*)&hA);
	clSetKernelArg(matrix_mul_kernel, 3, sizeof(cl_int), (void*)&wB);
	clSetKernelArg(matrix_mul_kernel, 4, sizeof(cl_int), (void*)&hB);
	clSetKernelArg(matrix_mul_kernel, 5, sizeof(cl_mem), (void*)&bufferA);
	clSetKernelArg(matrix_mul_kernel, 6, sizeof(cl_mem), (void*)&bufferB);

	// Set work group size of local and global.
	// Assume that matrix size is dividable by 16.
	size_t localws[2] = { 16, 16 };
	size_t globalws[2] = { width_C, height_C };

	// Run kernel.
	ciErrNum = clEnqueueNDRangeKernel(myqueue, matrix_mul_kernel, 2, NULL, globalws, localws, 0, NULL, NULL);

	// Read result data to host.
	ciErrNum = clEnqueueReadBuffer(myqueue, bufferC, CL_TRUE, 0, mem_size_C, (void*)matrix_C, 0, NULL, NULL);

	// Print out the results.
	printf("\n\nMatrix C (results)\n");
	/*int cnt_C = 0;
	for (cnt_C = 0; cnt_C < size_C; cnt_C++)
	{
		printf("%f ", matrix_C[cnt_C]);
		if (((cnt_C + 1) % width_C) == 0)
		{
			printf("\n");
		}
	}*/
	printf("\n");

	// Clean up the memory.

	clReleaseMemObject(bufferA);
	clReleaseMemObject(bufferB);
	clReleaseMemObject(bufferC);

	clReleaseKernel(matrix_mul_kernel);
	clReleaseProgram(matrix_mul_prog);
	clReleaseCommandQueue(myqueue);

	//free(device);
	free(kernel_source);
	free(matrix_A);
	free(matrix_B);
	free(matrix_C);

	return 0;
}