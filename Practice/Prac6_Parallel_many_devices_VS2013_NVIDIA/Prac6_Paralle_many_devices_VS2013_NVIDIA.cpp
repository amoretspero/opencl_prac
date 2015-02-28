#include <stdio.h>
#include <stdlib.h>
#include <time.h>
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
	// Print device list.
	print_devices();

	// Set host data.-----------------------------

	// Input data a.
	int size_a = 100;
	int mem_size_a = sizeof(float)*size_a;
	float* a = (float*)malloc(mem_size_a);

	// Input data b.
	int size_b = 100;
	int mem_size_b = sizeof(float)*size_b;
	float* b = (float*)malloc(mem_size_b);

	// Input data c.
	int size_c = 100;
	int mem_size_c = sizeof(float)*size_c;
	float* c = (float*)malloc(mem_size_c);

	// Input data d.
	int size_d = 100;
	int mem_size_d = sizeof(float)*size_d;
	float* d = (float*)malloc(mem_size_d);

	// Input/Output data e.
	int size_e = 100;
	int mem_size_e = sizeof(float)*size_e;
	float* e = (float*)malloc(mem_size_e);

	// Input/Output data f.
	int size_f = 100;
	int mem_size_f = sizeof(float)*size_f;
	float* f = (float*)malloc(mem_size_f);

	// Output data g.
	int size_g = 100;
	int mem_size_g = sizeof(float)*size_g;
	float* g = (float*)malloc(mem_size_g);

	// Initialize initial input data a, b, c, d.
	int cnt;
	srand(time(NULL));
	for (cnt = 0; cnt < size_a; cnt++)
	{
		a[cnt] = rand() / (float)RAND_MAX;
	}
	for (cnt = 0; cnt < size_b; cnt++)
	{
		b[cnt] = rand() / (float)RAND_MAX;
	}
	for (cnt = 0; cnt < size_c; cnt++)
	{
		c[cnt] = rand() / (float)RAND_MAX;
	}
	for (cnt = 0; cnt < size_d; cnt++)
	{
		d[cnt] = rand() / (float)RAND_MAX;
	}

	printf("\n\n Initial Vectors : \n");
	printf("Vector a :\n");
	for (cnt = 0; cnt < size_a; cnt++)
	{
		printf("%f ", a[cnt]);
		if ((cnt + 1) % 10 == 0)
		{
			printf("\n");
		}
	}
	printf("Vector b :\n");
	for (cnt = 0; cnt < size_b; cnt++)
	{
		printf("%f ", b[cnt]);
		if ((cnt + 1) % 10 == 0)
		{
			printf("\n");
		}
	}
	printf("Vector c :\n");
	for (cnt = 0; cnt < size_c; cnt++)
	{
		printf("%f ", c[cnt]);
		if ((cnt + 1) % 10 == 0)
		{
			printf("\n");
		}
	}
	printf("Vector d :\n");
	for (cnt = 0; cnt < size_d; cnt++)
	{
		printf("%f ", d[cnt]);
		if ((cnt + 1) % 10 == 0)
		{
			printf("\n");
		}
	}

	// Set OpenCL Environment.
	cl_int status;

	cl_device_id* devices;
	cl_device_id devices_lst[3];
	cl_device_id device_0, device_1, device_2;
	cl_uint num_devices;
	cl_platform_id* platforms;
	cl_platform_id platform_0, platform_1, platform_2;
	cl_uint num_platforms;
	cl_uint platform_idx;

	int selected_platform_0;
	int selected_device_0;
	int selected_platform_1;
	int selected_device_1;
	int selected_platform_2;
	int selected_device_2;

	printf("Select platform and device to use for Addition of vector a and b.\n");
	scanf_s("%d %d", &selected_platform_0, &selected_device_0);

	printf("Select platform and device to use for Addition of vector c and d.(Should be different with above choice.)\n");
	scanf_s("%d %d", &selected_platform_1, &selected_device_1);

	printf("Select platform and device to use for final addition of vector a+b and c+d.\n");
	scanf_s("%d %d", &selected_platform_2, &selected_device_2);

	// Get device information.
	status = clGetPlatformIDs(0, NULL, &num_platforms);
	chk(status, "clGetPlatformIDs");
	platforms = (cl_platform_id*)malloc(sizeof(cl_platform_id)*num_platforms);
	status = clGetPlatformIDs(num_platforms, platforms, NULL);
	chk(status, "clGetPlatformIDs");
	platform_0 = platforms[selected_platform_0 - 1];
	platform_1 = platforms[selected_platform_1 - 1];
	platform_2 = platforms[selected_platform_2 - 1];

	// Get device information.
	status = clGetDeviceIDs(platform_0, CL_DEVICE_TYPE_ALL, 0, NULL, &num_devices);
	chk(status, "clGetDeviceIDs");
	devices = (cl_device_id*)malloc(sizeof(cl_device_id)*num_devices);
	status = clGetDeviceIDs(platform_0, CL_DEVICE_TYPE_ALL, num_devices, devices, NULL);
	chk(status, "clGetDeviceIDs");
	devices_lst[0] = devices[selected_device_0 - 1];

	status = clGetDeviceIDs(platform_1, CL_DEVICE_TYPE_ALL, 0, NULL, &num_devices);
	chk(status, "clGetDeviceIDs");
	devices = (cl_device_id*)malloc(sizeof(cl_device_id)*num_devices);
	status = clGetDeviceIDs(platform_1, CL_DEVICE_TYPE_ALL, num_devices, devices, NULL);
	chk(status, "clGetDeviceIDs");
	devices_lst[1] = devices[selected_device_1 - 1];

	status = clGetDeviceIDs(platform_2, CL_DEVICE_TYPE_ALL, 0, NULL, &num_devices);
	chk(status, "clGetDeviceIDs");
	devices = (cl_device_id*)malloc(sizeof(cl_device_id)*num_devices);
	status = clGetDeviceIDs(platform_2, CL_DEVICE_TYPE_ALL, num_devices, devices, NULL);
	chk(status, "clGetDeviceIDs");
	devices_lst[2] = devices[selected_device_2 - 1];

	// Print out selected platform and device info.
	char* value;
	int* device_type;
	size_t valueSize;

	status = clGetDeviceInfo(devices_lst[0], CL_DEVICE_NAME, 0, NULL, &valueSize);
	chk(status, "clGetDeviceInfo");
	value = (char*)malloc(valueSize);
	status = clGetDeviceInfo(devices_lst[0], CL_DEVICE_NAME, valueSize, value, NULL);
	chk(status, "clGetDeviceInfo");
	printf("Selected Device 1 (for a + b) : \nPlatform - %d, Device - %d\nDevice name - %s\n", selected_platform_0, selected_device_0, value);
	free(value);
	status = clGetDeviceInfo(devices_lst[0], CL_DEVICE_OPENCL_C_VERSION, 0, NULL, &valueSize);
	chk(status, "clGetDeviceInfo");
	value = (char*)malloc(valueSize);
	status = clGetDeviceInfo(devices_lst[0], CL_DEVICE_OPENCL_C_VERSION, valueSize, value, NULL);
	chk(status, "clGetDeviceInfo");
	printf("OpenCL C version - %s\n", value);
	free(value);
	status = clGetDeviceInfo(devices_lst[0], CL_DEVICE_TYPE, 0, NULL, &valueSize);
	chk(status, "clGetDeviceInfo");
	device_type = (int*)malloc(valueSize);
	status = clGetDeviceInfo(devices_lst[0], CL_DEVICE_TYPE, valueSize, device_type, NULL);
	chk(status, "clGetDeviceInfo");
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

	status = clGetDeviceInfo(devices_lst[1], CL_DEVICE_NAME, 0, NULL, &valueSize);
	chk(status, "clGetDeviceInfo");
	value = (char*)malloc(valueSize);
	status = clGetDeviceInfo(devices_lst[1], CL_DEVICE_NAME, valueSize, value, NULL);
	chk(status, "clGetDeviceInfo");
	printf("Selected Device 2 (for c + d) : \nPlatform - %d, Device - %d\nDevice name - %s\n", selected_platform_1, selected_device_1, value);
	free(value);
	status = clGetDeviceInfo(devices_lst[1], CL_DEVICE_OPENCL_C_VERSION, 0, NULL, &valueSize);
	chk(status, "clGetDeviceInfo");
	value = (char*)malloc(valueSize);
	status = clGetDeviceInfo(devices_lst[1], CL_DEVICE_OPENCL_C_VERSION, valueSize, value, NULL);
	chk(status, "clGetDeviceInfo");
	printf("OpenCL C version - %s\n", value);
	free(value);
	status = clGetDeviceInfo(devices_lst[1], CL_DEVICE_TYPE, 0, NULL, &valueSize);
	chk(status, "clGetDeviceInfo");
	device_type = (int*)malloc(valueSize);
	status = clGetDeviceInfo(devices_lst[1], CL_DEVICE_TYPE, valueSize, device_type, NULL);
	chk(status, "clGetDeviceInfo");
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

	status = clGetDeviceInfo(devices_lst[2], CL_DEVICE_NAME, 0, NULL, &valueSize);
	chk(status, "clGetDeviceInfo");
	value = (char*)malloc(valueSize);
	status = clGetDeviceInfo(devices_lst[2], CL_DEVICE_NAME, valueSize, value, NULL);
	chk(status, "clGetDeviceInfo");
	printf("Selected Device 3 (for final addition) : \nPlatform - %d, Device - %d\nDevice name - %s\n", selected_platform_2, selected_device_2, value);
	free(value);
	status = clGetDeviceInfo(devices_lst[2], CL_DEVICE_OPENCL_C_VERSION, 0, NULL, &valueSize);
	chk(status, "clGetDeviceInfo");
	value = (char*)malloc(valueSize);
	status = clGetDeviceInfo(devices_lst[2], CL_DEVICE_OPENCL_C_VERSION, valueSize, value, NULL);
	chk(status, "clGetDeviceInfo");
	printf("OpenCL C version - %s\n", value);
	free(value);
	status = clGetDeviceInfo(devices_lst[2], CL_DEVICE_TYPE, 0, NULL, &valueSize);
	chk(status, "clGetDeviceInfo");
	device_type = (int*)malloc(valueSize);
	status = clGetDeviceInfo(devices_lst[2], CL_DEVICE_TYPE, valueSize, device_type, NULL);
	chk(status, "clGetDeviceInfo");
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

	
	cl_context_properties properties_0[3] = { CL_CONTEXT_PLATFORM, (cl_context_properties)platform_0, 0 };
	cl_context_properties properties_1[3] = { CL_CONTEXT_PLATFORM, (cl_context_properties)platform_1, 0 };
	cl_context_properties properties_2[3] = { CL_CONTEXT_PLATFORM, (cl_context_properties)platform_2, 0 };
	cl_context context_0, context_1, context_2;
	context_0 = clCreateContext(properties_0, 1, &devices_lst[0], NULL, NULL, &status);
	chk(status, "clCreateContext");
	context_1 = clCreateContext(properties_1, 1, &devices_lst[1], NULL, NULL, &status);
	chk(status, "clCreateContext");
	context_2 = clCreateContext(properties_2, 1, &devices_lst[2], NULL, NULL, &status);
	chk(status, "clCreateContext");

	// Create command queue.
	cl_command_queue queue_0, queue_1, queue_2;
	queue_0 = clCreateCommandQueue(context_0, devices_lst[0], 0, &status);
	chk(status, "clCreateCommandQueue");
	queue_1 = clCreateCommandQueue(context_1, devices_lst[1], 0, &status);
	chk(status, "clCreateCommandQueue");
	queue_2 = clCreateCommandQueue(context_2, devices_lst[2], 0, &status);
	chk(status, "clCreateCommandQueue");


	float* intermediate_e_output_local = (float*)malloc(mem_size_e);
	float* intermediate_f_output_local = (float*)malloc(mem_size_f);
	float* test = (float*)malloc(mem_size_g);
	// Declarations of Buffers
	cl_mem input_a = clCreateBuffer(context_0, CL_MEM_READ_ONLY, mem_size_a, 0, 0);
	cl_mem input_b = clCreateBuffer(context_0, CL_MEM_READ_ONLY, mem_size_b, 0, 0);
	cl_mem input_c = clCreateBuffer(context_1, CL_MEM_READ_ONLY, mem_size_c, 0, 0);
	cl_mem input_d = clCreateBuffer(context_1, CL_MEM_READ_ONLY, mem_size_d, 0, 0);
	cl_mem intermediate_e_output = clCreateBuffer(context_0, CL_MEM_READ_WRITE, mem_size_e, 0, 0);
	cl_mem intermediate_f_output = clCreateBuffer(context_1, CL_MEM_READ_WRITE, mem_size_f, 0, 0);
	cl_mem intermediate_e_input = clCreateBuffer(context_2, CL_MEM_READ_WRITE, mem_size_e, 0, 0);
	cl_mem intermediate_f_input = clCreateBuffer(context_2, CL_MEM_READ_WRITE, mem_size_f, 0, 0);
	cl_mem output_g = clCreateBuffer(context_2, CL_MEM_READ_WRITE, mem_size_g, 0, 0);	
	

	// Read source from external file for kernel.
	FILE* opencl_kernel_file;
	char* kernel_source;
	size_t kernel_source_size;

	fopen_s(&opencl_kernel_file, "Prac6_Parallel_many_devices_VS2013_NVIDIA.cl", "r");
	if (!opencl_kernel_file)
	{
		printf("Failed to load kernel.\n");
		exit(-1);
	}

	kernel_source = (char*)malloc(0x100000);
	kernel_source_size = fread(kernel_source, 1, 0x100000, opencl_kernel_file);
	fclose(opencl_kernel_file);

	// Create program object with source.
	cl_program program_0, program_1, program_2;
	program_0 = clCreateProgramWithSource(context_0, 1, (const char**)&kernel_source, (const size_t*)&kernel_source_size, &status);
	chk(status, "clCreateProgramWithSource");
	program_1 = clCreateProgramWithSource(context_1, 1, (const char**)&kernel_source, (const size_t*)&kernel_source_size, &status);
	chk(status, "clCreateProgramWithSource");
	program_2 = clCreateProgramWithSource(context_2, 1, (const char**)&kernel_source, (const size_t*)&kernel_source_size, &status);
	chk(status, "clCreateProgramWithSource");

	// Build program.
	status = clBuildProgram(program_0, 1, &devices_lst[0], NULL, NULL, NULL);
	if (status == CL_BUILD_PROGRAM_FAILURE)
	{
		size_t log_size;
		char* log_str;
		clGetProgramBuildInfo(program_0, devices_lst[0], CL_PROGRAM_BUILD_LOG, 0, NULL, &log_size);
		log_str = (char*)malloc(log_size);
		clGetProgramBuildInfo(program_0, devices_lst[0], CL_PROGRAM_BUILD_LOG, log_size, log_str, NULL);
		printf("Build log : \n%s\n", log_str);
		free(log_str);
	}
	chk(status, "clBuildProgram");

	status = clBuildProgram(program_1, 1, &devices_lst[1], NULL, NULL, NULL);
	if (status == CL_BUILD_PROGRAM_FAILURE)
	{
		size_t log_size;
		char* log_str;
		clGetProgramBuildInfo(program_1, devices_lst[1], CL_PROGRAM_BUILD_LOG, 0, NULL, &log_size);
		log_str = (char*)malloc(log_size);
		clGetProgramBuildInfo(program_1, devices_lst[1], CL_PROGRAM_BUILD_LOG, log_size, log_str, NULL);
		printf("Build log : \n%s\n", log_str);
		free(log_str);
	}
	chk(status, "clBuildProgram");

	status = clBuildProgram(program_2, 1, &devices_lst[2], NULL, NULL, NULL);
	if (status == CL_BUILD_PROGRAM_FAILURE)
	{
		size_t log_size;
		char* log_str;
		clGetProgramBuildInfo(program_2, devices_lst[2], CL_PROGRAM_BUILD_LOG, 0, NULL, &log_size);
		log_str = (char*)malloc(log_size);
		clGetProgramBuildInfo(program_2, devices_lst[2], CL_PROGRAM_BUILD_LOG, log_size, log_str, NULL);
		printf("Build log : \n%s\n", log_str);
		free(log_str);
	}
	chk(status, "clBuildProgram");

	// Create kernel.
	cl_kernel kernel_0, kernel_1, kernel_2;
	kernel_0 = clCreateKernel(program_0, "vector_add", &status);
	chk(status, "clCreateKernel");
	kernel_1 = clCreateKernel(program_1, "vector_add", &status);
	chk(status, "clCreateKernel");
	kernel_2 = clCreateKernel(program_2, "vector_add", &status);
	chk(status, "clCreateKernel");

	cl_event event_init[2];

	clEnqueueWriteBuffer(queue_0, input_a, CL_TRUE, 0, mem_size_a, (void*)a, 0, NULL, NULL);
	clEnqueueWriteBuffer(queue_0, input_b, CL_TRUE, 0, mem_size_b, (void*)b, 0, NULL, NULL);
	clEnqueueWriteBuffer(queue_1, input_c, CL_TRUE, 0, mem_size_c, (void*)c, 0, NULL, NULL);
	clEnqueueWriteBuffer(queue_1, input_d, CL_TRUE, 0, mem_size_d, (void*)d, 0, NULL, NULL);
	

	clSetKernelArg(kernel_0, 0, sizeof(cl_mem), (void*)&input_a);
	clSetKernelArg(kernel_0, 1, sizeof(cl_mem), (void*)&input_b);
	clSetKernelArg(kernel_0, 2, sizeof(cl_mem), (void*)&intermediate_e_output);
	clSetKernelArg(kernel_1, 0, sizeof(cl_mem), (void*)&input_c);
	clSetKernelArg(kernel_1, 1, sizeof(cl_mem), (void*)&input_d);
	clSetKernelArg(kernel_1, 2, sizeof(cl_mem), (void*)&intermediate_f_output);
	clSetKernelArg(kernel_2, 0, sizeof(cl_mem), (void*)&intermediate_e_input);
	clSetKernelArg(kernel_2, 1, sizeof(cl_mem), (void*)&intermediate_f_input);
	clSetKernelArg(kernel_2, 2, sizeof(cl_mem), (void*)&output_g);
	

	size_t localws[1] = { 4 };
	size_t globalws[1] = { 100 };

	cl_event event_copy_read[2];
	cl_event event_copy_write[2];
	cl_event event_fin;

	clEnqueueNDRangeKernel(queue_0, kernel_0, 1, NULL, globalws, localws, 0, NULL, &event_init[0]);
	clEnqueueNDRangeKernel(queue_1, kernel_1, 1, NULL, globalws, localws, 0, NULL, &event_init[1]);

	clEnqueueReadBuffer(queue_0, intermediate_e_output, CL_TRUE, 0, mem_size_e, (void*)intermediate_e_output_local, 1, &event_init[0], &event_copy_read[0]);
	clEnqueueReadBuffer(queue_1, intermediate_f_output, CL_TRUE, 0, mem_size_f, (void*)intermediate_f_output_local, 1, &event_init[1], &event_copy_read[1]);


	clEnqueueWriteBuffer(queue_2, intermediate_e_input, CL_TRUE, 0, mem_size_e, (void*)intermediate_e_output_local, 1, &event_copy_read[0], &event_copy_write[0]);
	clEnqueueWriteBuffer(queue_2, intermediate_f_input, CL_TRUE, 0, mem_size_f, (void*)intermediate_f_output_local, 1, &event_copy_read[1], &event_copy_write[1]);

	
	clEnqueueNDRangeKernel(queue_2, kernel_2, 1, NULL, globalws, localws, 2, event_copy_write, &event_fin);

	clEnqueueReadBuffer(queue_2, intermediate_f_input, CL_TRUE, 0, mem_size_g, (void*)test, 1, &event_fin, NULL);

	for (cnt = 0; cnt < size_g; cnt++)
	{
		printf("%f\t", intermediate_e_output_local[cnt]);
		if ((cnt + 1) % 10 == 0)
		{
			printf("\n");
		}
	}

	for (cnt = 0; cnt < size_g; cnt++)
	{
		printf("%f\t", intermediate_f_output_local[cnt]);
		if ((cnt + 1) % 10 == 0)
		{
			printf("\n");
		}
	}

	printf("Result : \n");
	for (cnt = 0; cnt < size_g; cnt++)
	{
		printf("%f\t", test[cnt]);
		if ((cnt + 1) % 10 == 0)
		{
			printf("\n");
		}
	}

	clReleaseMemObject(input_a);
	clReleaseMemObject(input_b);
	clReleaseMemObject(input_c);
	clReleaseMemObject(input_d);
	clReleaseMemObject(intermediate_e_output);
	clReleaseMemObject(intermediate_e_input);
	clReleaseMemObject(intermediate_f_output);
	clReleaseMemObject(intermediate_f_input);
	clReleaseMemObject(output_g);

	clReleaseKernel(kernel_0);
	clReleaseKernel(kernel_1);
	clReleaseKernel(kernel_2);
	clReleaseProgram(program_0);
	clReleaseProgram(program_1);
	clReleaseProgram(program_2);
	clReleaseCommandQueue(queue_0);
	clReleaseCommandQueue(queue_1);
	clReleaseCommandQueue(queue_2);

	free(kernel_source);

	

	system("pause");
	return 0;
}

