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

int main (int argc, char** argv)
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
	device_0 = devices[selected_device_0 - 1];

	status = clGetDeviceIDs(platform_1, CL_DEVICE_TYPE_ALL, 0, NULL, &num_devices);
	chk(status, "clGetDeviceIDs");
	devices = (cl_device_id*)malloc(sizeof(cl_device_id)*num_devices);
	status = clGetDeviceIDs(platform_1, CL_DEVICE_TYPE_ALL, num_devices, devices, NULL);
	chk(status, "clGetDeviceIDs");
	device_1 = devices[selected_device_1 - 1];

	status = clGetDeviceIDs(platform_2, CL_DEVICE_TYPE_ALL, 0, NULL, &num_devices);
	chk(status, "clGetDeviceIDs");
	devices = (cl_device_id*)malloc(sizeof(cl_device_id)*num_devices);
	status = clGetDeviceIDs(platform_2, CL_DEVICE_TYPE_ALL, num_devices, devices, NULL);
	chk(status, "clGetDeviceIDs");
	device_2 = devices[selected_device_2 - 1];

	// Print out selected platform and device info.
	char* value;
	int* device_type;
	size_t valueSize;

	status = clGetDeviceInfo(device_0, CL_DEVICE_NAME, 0, NULL, &valueSize);
	chk(status, "clGetDeviceInfo");
	value = (char*)malloc(valueSize);
	status = clGetDeviceInfo(device_0, CL_DEVICE_NAME, valueSize, value, NULL);
	chk(status, "clGetDeviceInfo");
	printf("Selected Device 1 (for a + b) : \nPlatform - %d, Device - %d\nDevice name - %s\n", selected_platform_0, selected_device_0, value);
	free(value);
	status = clGetDeviceInfo(device_0, CL_DEVICE_OPENCL_C_VERSION, 0, NULL, &valueSize);
	chk(status, "clGetDeviceInfo");
	value = (char*)malloc(valueSize);
	status = clGetDeviceInfo(device_0, CL_DEVICE_OPENCL_C_VERSION, valueSize, value, NULL);
	chk(status, "clGetDeviceInfo");
	printf("OpenCL C version - %s\n", value);
	free(value);
	status = clGetDeviceInfo(device_0, CL_DEVICE_TYPE, 0, NULL, &valueSize);
	chk(status, "clGetDeviceInfo");
	device_type = (int*)malloc(valueSize);
	status = clGetDeviceInfo(device_0, CL_DEVICE_TYPE, valueSize, device_type, NULL);
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

	status = clGetDeviceInfo(device_1, CL_DEVICE_NAME, 0, NULL, &valueSize);
	chk(status, "clGetDeviceInfo");
	value = (char*)malloc(valueSize);
	status = clGetDeviceInfo(device_1, CL_DEVICE_NAME, valueSize, value, NULL);
	chk(status, "clGetDeviceInfo");
	printf("Selected Device 2 (for c + d) : \nPlatform - %d, Device - %d\nDevice name - %s\n", selected_platform_1, selected_device_1, value);
	free(value);
	status = clGetDeviceInfo(device_1, CL_DEVICE_OPENCL_C_VERSION, 0, NULL, &valueSize);
	chk(status, "clGetDeviceInfo");
	value = (char*)malloc(valueSize);
	status = clGetDeviceInfo(device_1, CL_DEVICE_OPENCL_C_VERSION, valueSize, value, NULL);
	chk(status, "clGetDeviceInfo");
	printf("OpenCL C version - %s\n", value);
	free(value);
	status = clGetDeviceInfo(device_1, CL_DEVICE_TYPE, 0, NULL, &valueSize);
	chk(status, "clGetDeviceInfo");
	device_type = (int*)malloc(valueSize);
	status = clGetDeviceInfo(device_1, CL_DEVICE_TYPE, valueSize, device_type, NULL);
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

	status = clGetDeviceInfo(device_2, CL_DEVICE_NAME, 0, NULL, &valueSize);
	chk(status, "clGetDeviceInfo");
	value = (char*)malloc(valueSize);
	status = clGetDeviceInfo(device_2, CL_DEVICE_NAME, valueSize, value, NULL);
	chk(status, "clGetDeviceInfo");
	printf("Selected Device 3 (for final addition) : \nPlatform - %d, Device - %d\nDevice name - %s\n", selected_platform_2, selected_device_2, value);
	free(value);
	status = clGetDeviceInfo(device_2, CL_DEVICE_OPENCL_C_VERSION, 0, NULL, &valueSize);
	chk(status, "clGetDeviceInfo");
	value = (char*)malloc(valueSize);
	status = clGetDeviceInfo(device_2, CL_DEVICE_OPENCL_C_VERSION, valueSize, value, NULL);
	chk(status, "clGetDeviceInfo");
	printf("OpenCL C version - %s\n", value);
	free(value);
	status = clGetDeviceInfo(device_2, CL_DEVICE_TYPE, 0, NULL, &valueSize);
	chk(status, "clGetDeviceInfo");
	device_type = (int*)malloc(valueSize);
	status = clGetDeviceInfo(device_2, CL_DEVICE_TYPE, valueSize, device_type, NULL);
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

	getchar();
	return 0;
}

