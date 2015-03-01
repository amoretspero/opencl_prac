#include <stdio.h>
#include <stdlib.h>
#include <CL/cl.h>

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
	// Print device informations.
	print_devices();

	// Set host data.--------------------
	int size_a = 10 * 10;
	int mem_size_a = sizeof(int)*size_a;
	int* a = (int*)malloc(mem_size_a); // Input data.
	int size_b = 10 * 10;
	int mem_size_b = sizeof(int)*size_b;
	int* b = (int*)malloc(mem_size_b); // Output data.

	// Initialize intput data.
	int cnt;
	srand(2015);
	for (cnt = 0; cnt < size_a; cnt++)
	{
		a[cnt] = rand() % 10000;
	}
	printf("\n\nInitial : \n");
	for (cnt = 0; cnt < size_a; cnt++)
	{
		printf("%d\t", a[cnt]);
		if ((cnt + 1) % 10 == 0)
		{
			printf("\n");
		}
	}

	// Set OpenCL Environment.--------------------
	cl_int status;

	cl_device_id* devices;
	cl_device_id device;
	cl_uint num_devices;
	cl_platform_id* platforms;
	cl_platform_id platform;
	cl_uint num_platforms;
	cl_uint platform_idx;

	int selected_platform;
	int selected_device;

	printf("Select platform and device to use.\n");
	scanf_s("%d %d", &selected_platform, &selected_device);

	// Get platform information.
	status = clGetPlatformIDs(0, NULL, &num_platforms);
	chk(status, "clGetPlatformIDs");
	platforms = (cl_platform_id*)malloc(sizeof(cl_platform_id)*num_platforms);
	status = clGetPlatformIDs(num_platforms, platforms, NULL);
	chk(status, "clGetPlatformIDs");
	platform = platforms[selected_platform - 1];

	// Get device information.
	status = clGetDeviceIDs(platform, CL_DEVICE_TYPE_ALL, 0, NULL, &num_devices);
	chk(status, "clGetDeviceIDs");
	devices = (cl_device_id*)malloc(sizeof(cl_device_id)*num_devices);
	status = clGetDeviceIDs(platform, CL_DEVICE_TYPE_ALL, num_devices, devices, NULL);
	chk(status, "clGetDeviceIDs");
	device = devices[selected_device - 1];

	// Print out selected platform and device info.
	char* value;
	int* device_type;
	size_t valueSize;
	status = clGetDeviceInfo(device, CL_DEVICE_NAME, 0, NULL, &valueSize);
	chk(status, "clGetDeviceInfo");
	value = (char*)malloc(valueSize);
	status = clGetDeviceInfo(device, CL_DEVICE_NAME, valueSize, value, NULL);
	chk(status, "clGetDeviceInfo");
	printf("Selected Device :\nPlatform - %d\nDevice name - %s\n", selected_platform, value);
	free(value);
	status = clGetDeviceInfo(device, CL_DEVICE_OPENCL_C_VERSION, 0, NULL, &valueSize);
	chk(status, "clGetDeviceInfo");
	value = (char*)malloc(valueSize);
	status = clGetDeviceInfo(device, CL_DEVICE_OPENCL_C_VERSION, valueSize, value, NULL);
	chk(status, "clGetDeviceInfo");
	printf("OpenCL C version - %s\n\n", value);
	free(value);

	// Create context for device.
	cl_context_properties properties[3] = { CL_CONTEXT_PLATFORM, (cl_context_properties)platform, 0 };
	cl_context context;
	context = clCreateContext(properties, 1, &device, NULL, NULL, &status);
	chk(status, "clCreateContext");

	// Create command queue.
	cl_command_queue queue;
	queue = clCreateCommandQueue(context, device, 0, &status);
	chk(status, "clCreateCommandQueue");

	// Declarations of buffers, images and data transfers.--------------------
	// Create space for source buffer on device.
	cl_mem input = clCreateBuffer(context, CL_MEM_READ_ONLY, mem_size_a, 0, 0);
	cl_mem intermediate = clCreateBuffer(context, CL_MEM_READ_WRITE, mem_size_b, 0, 0);
	cl_mem output = clCreateBuffer(context, CL_MEM_WRITE_ONLY, mem_size_b, 0, 0);

	// Compile kernel for runtime.--------------------
	// Read source from external file for kernel.
	FILE* opencl_kernel_file;
	char* kernel_source;
	size_t kernel_source_size;

	fopen_s(&opencl_kernel_file, "Prac5_VS2013_NVIDIA.cl", "r");
	if (!opencl_kernel_file)
	{
		printf("Failed to load kernel.\n");
		exit(-1);
	}

	kernel_source = (char*)malloc(0x100000);
	kernel_source_size = fread(kernel_source, 1, 0x100000, opencl_kernel_file);
	fclose(opencl_kernel_file);

	// Create program object with source.
	cl_program program;
	program = clCreateProgramWithSource(context, 1, (const char**)&kernel_source, (const size_t*)&kernel_source_size, &status);
	chk(status, "clCreateProgramWithSource");

	// Build program.
	status = clBuildProgram(program, 1, &device, NULL, NULL, NULL);
	if (status == CL_BUILD_PROGRAM_FAILURE)
	{
		size_t log_size;
		clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG, 0, NULL, &log_size);
		char* log_str = (char*)malloc(log_size);
		//memset(log_str, 0, log_size);
		clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG, log_size, log_str, NULL);
		printf("Build log : \n%s\n", log_str);
	}
	chk(status, "clBuildProgram");

	// Create kernel.
	cl_kernel kernel;
	kernel = clCreateKernel(program, "simplekernel", &status);
	chk(status, "clCreateKernel");

	clEnqueueWriteBuffer(queue, input, CL_TRUE, 0, mem_size_a, (void*)a, 0, NULL, NULL);

	// Set the kernel arguments.
	clSetKernelArg(kernel, 0, sizeof(cl_mem), &input);
	clSetKernelArg(kernel, 1, sizeof(cl_mem), &intermediate);
	//clSetKernelArg(kernel, 2, 2 * sizeof(int), 0);

	size_t localws[2] = { 1, 1 };
	size_t globalws[2] = { 10, 10 };

	clEnqueueNDRangeKernel(queue, kernel, 2, NULL, globalws, localws, 0, NULL, NULL);

	clSetKernelArg(kernel, 0, sizeof(cl_mem), (void*)&intermediate);
	clSetKernelArg(kernel, 1, sizeof(cl_mem), (void*)&output);

	clEnqueueNDRangeKernel(queue, kernel, 2, NULL, globalws, localws, 0, NULL, NULL);
	clEnqueueReadBuffer(queue, output, CL_TRUE, 0, mem_size_b, (void*)b, 0, NULL, NULL);

	// Print out the results.
	printf("Result(should every element be 4 timed) : \n");
	for (cnt = 0; cnt < size_b; cnt++)
	{
		printf("%d\t", b[cnt]);
		if ((cnt + 1) % 10 == 0)
		{
			printf("\n");
		}
	}

	for (cnt = 0; cnt < size_a; cnt++)
	{
		if (a[cnt] * 4 != b[cnt])
		{
			printf("Result is incorrect at %dth element!\n");
			exit(-1);
		}
	}
	printf("\nResult is correct!\n");
	// Release memory.
	clReleaseMemObject(input);
	clReleaseMemObject(intermediate);
	clReleaseMemObject(output);

	clReleaseKernel(kernel);
	clReleaseProgram(program);
	clReleaseCommandQueue(queue);

	free(kernel_source);
	//free(a);
	//free(b);

	system("pause");
	return 0;
}