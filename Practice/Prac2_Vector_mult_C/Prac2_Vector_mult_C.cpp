#include <stdio.h>
#include <stdlib.h>

#include <CL/cl.h>
#include <CL/opencl.h>

#define width_A 4096
#define height_A 4096
#define width_B 4096
#define height_B 4096
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

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <CL/cl.h>
#include <math.h>
#include <float.h>

void chk(cl_int err, const char* cmd)
{
	if (err != CL_SUCCESS)
	{
		printf("%s failed(%d)", cmd, err);
		exit(-1);
	}
}

int print_devices()
{
	// Set variables.
	int i;
	int j;
	char* value;
	int* device_type;
	size_t valueSize;
	cl_uint platformCount;
	cl_platform_id* platforms;
	cl_uint deviceCount;
	cl_device_id* devices;
	cl_uint maxComputeUnits;

	// Get all of the platforms.

	clGetPlatformIDs(0, NULL, &platformCount);
	printf("Platform count : %d\n\n", platformCount);
	platforms = (cl_platform_id*)malloc(sizeof(cl_platform_id) * platformCount);
	clGetPlatformIDs(platformCount, platforms, NULL);

	for (i = 0; i < platformCount; i++)
	{
		// Get all devices in specified platform.
		clGetDeviceIDs(platforms[i], CL_DEVICE_TYPE_ALL, 0, NULL, &deviceCount);
		devices = (cl_device_id*)malloc(sizeof(cl_device_id) * deviceCount);
		clGetDeviceIDs(platforms[i], CL_DEVICE_TYPE_ALL, deviceCount, devices, NULL);

		// For each device, print critical informations.
		for (j = 0; j < deviceCount; j++)
		{
			// Device name.
			clGetDeviceInfo(devices[j], CL_DEVICE_NAME, 0, NULL, &valueSize);
			value = (char*)malloc(valueSize);
			clGetDeviceInfo(devices[j], CL_DEVICE_NAME, valueSize, value, NULL);
			printf("Platform %d - Device %d : Name -> \t\t\t%s\n", i + 1, j + 1, value);
			free(value);

			// Device profile.
			clGetDeviceInfo(devices[j], CL_DEVICE_PROFILE, 0, NULL, &valueSize);
			value = (char*)malloc(valueSize);
			clGetDeviceInfo(devices[j], CL_DEVICE_PROFILE, valueSize, value, NULL);
			printf("Platform %d - Device %d : profile -> \t\t\t%s\n", i + 1, j + 1, value);
			free(value);

			// Hardware version.
			clGetDeviceInfo(devices[j], CL_DEVICE_VERSION, 0, NULL, &valueSize);
			value = (char*)malloc(valueSize);
			clGetDeviceInfo(devices[j], CL_DEVICE_VERSION, valueSize, value, NULL);
			printf("Platform %d - Device %d : Hardware version -> \t\t%s\n", i + 1, j + 1, value);
			free(value);

			// Software driver version.
			clGetDeviceInfo(devices[j], CL_DRIVER_VERSION, 0, NULL, &valueSize);
			value = (char*)malloc(valueSize);
			clGetDeviceInfo(devices[j], CL_DRIVER_VERSION, valueSize, value, NULL);
			printf("Platform %d - Device %d : Driver version -> \t\t%s\n", i + 1, j + 1, value);
			free(value);

			// OpenCL C version.
			clGetDeviceInfo(devices[j], CL_DEVICE_OPENCL_C_VERSION, 0, NULL, &valueSize);
			value = (char*)malloc(valueSize);
			clGetDeviceInfo(devices[j], CL_DEVICE_OPENCL_C_VERSION, valueSize, value, NULL);
			printf("Platform %d - Device %d : OpenCL C version -> \t\t%s\n", i + 1, j + 1, value);
			free(value);

			// Device type.
			clGetDeviceInfo(devices[j], CL_DEVICE_TYPE, 0, NULL, &valueSize);
			device_type = (int*)malloc(valueSize);
			clGetDeviceInfo(devices[j], CL_DEVICE_TYPE, valueSize, device_type, NULL);
			if (*device_type == CL_DEVICE_TYPE_CPU)
			{
				printf("Platform %d - Device %d : Device type -> \t\t\t%s\n", i + 1, j + 1, "CPU");
			}
			else if (*device_type == CL_DEVICE_TYPE_GPU)
			{
				printf("Platform %d - Device %d : Device type -> \t\t\t%s\n", i + 1, j + 1, "GPU");
			}
			else if (*device_type == CL_DEVICE_TYPE_ACCELERATOR)
			{
				printf("Platform %d - Device %d : Device type -> \t\t\t%s\n", i + 1, j + 1, "GPU");
			}
			else
			{
				printf("Platform %d - Device %d : Device type -> \t\t\t%s\n", i + 1, j + 1, "Unknown type");
			}
			free(device_type);

			// Parallel compute units.
			clGetDeviceInfo(devices[j], CL_DEVICE_MAX_COMPUTE_UNITS, sizeof(maxComputeUnits), &maxComputeUnits, NULL);
			printf("Platform %d - Device %d : Max parallel compute units -> \t%d\n\n", i + 1, j + 1, maxComputeUnits);
		}
		free(devices);
	}
	free(platforms);
	return 0;
}

int main(int argc, char** argv)
{
	print_devices();

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
	/*printf("\n\nMatrix_A\n");
	int cnt_A = 0;
	for (cnt_A = 0; cnt_A < size_A; cnt_A++)
	{
		printf("%f ", matrix_A[cnt_A]);
		if (((cnt_A + 1) % width_A) == 0)
		{
			printf("\n");
		}
	}
	printf("\n\nMatrix_B\n");
	int cnt_B = 0;
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

	// User selection values
	cl_platform_id selected_platform;
	cl_device_id selected_device;
	int selected_platform_id;
	int selected_device_id;

	printf("Select platform and device to use for Multiplication of Matrix A and B.\n");
	scanf_s("%d %d", &selected_platform_id, &selected_device_id);

	// Use first platform.
	cl_uint numPlatforms = 0;
	ciErrNum = clGetPlatformIDs(0, NULL, &numPlatforms);
	cl_platform_id* platform = NULL;
	platform = (cl_platform_id*)malloc(numPlatforms*sizeof(cl_platform_id));
	ciErrNum = clGetPlatformIDs(numPlatforms, platform, NULL);
	selected_platform = platform[selected_platform_id - 1];
	//cl_platform platform;
	//ciErrNum = clGetPlatformIDs(1, &platform, NULL);

	// Use first device.
	cl_uint numDevices = 0;
	ciErrNum = clGetDeviceIDs(selected_platform, CL_DEVICE_TYPE_ALL, 0, NULL, &numDevices);
	cl_device_id* devices;
	devices = (cl_device_id*)malloc(numDevices*sizeof(cl_device_id));
	ciErrNum = clGetDeviceIDs(selected_platform, CL_DEVICE_TYPE_ALL, numDevices, devices, NULL);
	selected_device = devices[selected_device_id - 1];
	//cl_device_id device;
	//ciErrNum = clGetDeviceIDs(platform, CL_DEVICE_TYPE_ALL, 1, &device, NULL);

	// Print out selected platform and device info.(Information printed : Platform number, Device number, Device name, Device OpenCL C version, Device type)
	char* value;
	int* device_type;
	size_t valueSize;

	ciErrNum = clGetDeviceInfo(selected_device, CL_DEVICE_NAME, 0, NULL, &valueSize);
	chk(ciErrNum, "clGetDeviceInfo");
	value = (char*)malloc(valueSize);
	ciErrNum = clGetDeviceInfo(selected_device, CL_DEVICE_NAME, valueSize, value, NULL);
	chk(ciErrNum, "clGetDeviceInfo");
	printf("Selected Device 1 (for a + b) : \nPlatform - %d, Device - %d\nDevice name - %s\n", selected_platform_id, selected_device_id, value);
	free(value);
	ciErrNum = clGetDeviceInfo(selected_device, CL_DEVICE_OPENCL_C_VERSION, 0, NULL, &valueSize);
	chk(ciErrNum, "clGetDeviceInfo");
	value = (char*)malloc(valueSize);
	ciErrNum = clGetDeviceInfo(selected_device, CL_DEVICE_OPENCL_C_VERSION, valueSize, value, NULL);
	chk(ciErrNum, "clGetDeviceInfo");
	printf("OpenCL C version - %s\n", value);
	free(value);
	ciErrNum = clGetDeviceInfo(selected_device, CL_DEVICE_PREFERRED_VECTOR_WIDTH_FLOAT, 0, NULL, &valueSize);
	chk(ciErrNum, "clGetDeviceInfo");
	value = (char*)malloc(valueSize);
	ciErrNum = clGetDeviceInfo(selected_device, CL_DEVICE_PREFERRED_VECTOR_WIDTH_FLOAT, valueSize, value, NULL);
	printf("Preferred vector width for float - %f\n", value);
	free(value);
	ciErrNum = clGetDeviceInfo(selected_device, CL_DEVICE_TYPE, 0, NULL, &valueSize);
	chk(ciErrNum, "clGetDeviceInfo");
	device_type = (int*)malloc(valueSize);
	ciErrNum = clGetDeviceInfo(selected_device, CL_DEVICE_TYPE, valueSize, device_type, NULL);
	chk(ciErrNum, "clGetDeviceInfo");
	if (*device_type == CL_DEVICE_TYPE_CPU)
	{
		printf("Device type - %s\n\n", "CPU");
	}
	else if (*device_type == CL_DEVICE_TYPE_GPU)
	{
		printf("Device type - %s\n\n", "GPU");
	}
	else if (*device_type == CL_DEVICE_TYPE_ACCELERATOR)
	{
		printf("Device type - %s\n\n", "ACCELERATOR");
	}
	else
	{
		printf("Device type - %s\n\n", "UNKNOWN_TYPE");
	}
	free(device_type);

	cl_context_properties cps[3] = { CL_CONTEXT_PLATFORM, (cl_context_properties)selected_platform, 0 };

	// Create context.
	cl_context ctx = clCreateContext(cps, 1, &selected_device, NULL, NULL, &ciErrNum);

	// Create command queue.
	cl_command_queue myqueue = clCreateCommandQueue(ctx, selected_device, 0, &ciErrNum);

	// Load the OpenCL kernel.
	FILE* opencl_kernel;
	char* kernel_source;
	size_t kernel_size;

	fopen_s(&opencl_kernel, "Prac2_Vector_mult_C.cl", "r");
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
	ciErrNum = clBuildProgram(matrix_mul_prog, 1, &selected_device, NULL, NULL, NULL);

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
	size_t localws[2] = { 16,16 };
	size_t globalws[2] = { width_C, height_C };

	clock_t start_OpenCL = clock();
	printf("clock : %d\n", start_OpenCL);

	// Run kernel.
	ciErrNum = clEnqueueNDRangeKernel(myqueue, matrix_mul_kernel, 2, NULL, globalws, localws, 0, NULL, NULL);

	// Read result data to host.
	ciErrNum = clEnqueueReadBuffer(myqueue, bufferC, CL_TRUE, 0, mem_size_C, (void*)matrix_C, 0, NULL, NULL);

	clock_t end_OpenCL = clock();
	printf("clock : %d\n", end_OpenCL);

	// Print out the results.
	/*printf("\n\nMatrix C (results)\n");
	int cnt_C = 0;
	for (cnt_C = 0; cnt_C < size_C; cnt_C++)
	{
		printf("%f ", matrix_C[cnt_C]);
		if (((cnt_C + 1) % width_C) == 0)
		{
			printf("\n");
		}
	}*/
	printf("\n");

	/*clock_t start_C = clock();
	printf("clock : %d\n", start_C);
	float* sum;
	sum = (float*)malloc(sizeof(float)*width_A*height_A);
	float* C_res;
	C_res = (float*)malloc(sizeof(float)*width_C*height_C);
	for (int i = 0; i < width_C*height_C; i++)
	{
		float temp = 0.0;
		for (int j = 0; j < width_A; j++)
		{
			temp += matrix_A[(i / width_C)*height_A + j] * matrix_B[(j * (i / width_C)) + (i % width_C)];
		}
		C_res[i] = temp;
	}
	clock_t end_C = clock();
	printf("clock : %d\n", end_C);*/
	printf("OpenCL used %d clock cycles\n", (end_OpenCL - start_OpenCL));
	//printf("C Used %d clock cycles\n", (end_C - start_C));

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

	system("pause");

	return 0;
}