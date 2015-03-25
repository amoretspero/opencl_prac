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
	int i, j;
	char* value;
	int* device_type;
	size_t valueSize;
	cl_uint platformCount;
	cl_platform_id* platforms; // Since there aren't only one platform.
	cl_uint deviceCount;
	cl_device_id* devices; // Since there aren't only one device in given platform.
	cl_uint maxComputeUnits;

	// Get all of the platforms.

	clGetPlatformIDs(0, NULL, &platformCount); // Since 'platforms' is NULL, num_entries should be 0 and it returns number of platforms.
	printf("Platform count : %d\n\n", platformCount);
	platforms = (cl_platform_id*)malloc(sizeof(cl_platform_id) * platformCount);
	clGetPlatformIDs(platformCount, platforms, NULL); // Since 'num_platforms' is NULL, it returns platforms in form of array of platforms.

	for (i = 0; i < platformCount; i++)
	{
		// Get all devices in specified platform.
		clGetDeviceIDs(platforms[i], CL_DEVICE_TYPE_ALL, 0, NULL, &deviceCount); // Gets the number of devices that are in given platform, regardless of their types.
		devices = (cl_device_id*)malloc(sizeof(cl_device_id) * deviceCount);
		clGetDeviceIDs(platforms[i], CL_DEVICE_TYPE_ALL, deviceCount, devices, NULL); // Gets the devices in given platform, regardless of their type, in form of array of devices.

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
			printf("Platform %d - Device %d : Hardware version -> \t\t %s\n", i + 1, j + 1, value);
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
				printf("Platform %d - Device %d : Device type -> \t\t\t%s\n", i + 1, j + 1, "ACCELERATOR");
			}
			else
			{
				printf("Platform %d - Device %d : Device type -> \t\t\t%s\n", i + 1, j + 1, "Unknown type");
			}
			free(device_type);

			// Parallel compute units.
			clGetDeviceInfo(devices[j], CL_DEVICE_MAX_COMPUTE_UNITS, sizeof(maxComputeUnits), &maxComputeUnits, NULL);
			printf("Platform %d - Device %d : Max paralle compute units -> \t%d\n\n", i + 1, j + 1, maxComputeUnits);
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

	// Set host data.---------------------------------------------

	// Initialize input data.
	float mat[16];
	float vec[4];
	float result[4];

	float correct[4] = {0.0f, 0.0f, 0.0f, 0.0f};

	for(i = 0; i < 16; i++)
	{
		mat[i] = i * 2.0f;
	}

	for (i = 0; i < 4; i++)
	{
		vec[i] = i * 3.0f;
		correct[0] += mat[i] * vec[i];
		correct[1] += mat[i+4] * vec[i];
		correct[2] += mat[i+8] * vec[i];
		correct[3] += mat[i+12] * vec[i];
	}

	// Set OpenCL Environment.
	cl_int status;
	cl_device_id* devices;
	cl_device_id device_selected; // Indicates the selected device.
	cl_uint num_devices;
	cl_platform_id* platforms;
	cl_platform_id platform_selected; // Indicates the selected platform.
	cl_uint num_platforms;

	// Indications for platform, device selection.
	int selected_platform_number;
	int selected_device_number;

	// User selection part.
	printf("Select platform and device to use.\n");
	scanf_s("%d %d", &selected_platform_number, &selected_device_number);

	// Get platform information.
	status = clGetPlatformIDs(0, NULL, &num_platforms);
	chk(status, "clGetPlatformIDs");
	platforms = (cl_platform_id*)malloc(sizeof(cl_platform_id) * num_platforms);
	status = clGetPlatformIDs(num_platforms, platforms, NULL);
	chk(status, "clGetPlatformIDs");
	platform_selected = platforms[selected_platform_number - 1];

	// Get device information.
	status = clGetDeviceIDs(platform_selected, CL_DEVICE_TYPE_ALL, 0, NULL, &num_devices);
	chk(status, "clGetDeviceIDs");
	devices = (cl_Device_id*)malloc(sizeof(cl_device_id)*num_devices);
	status = clGetDeviceIDs(platform_selected, CL_DEVICE_TYPE_ALL, num_devices, devices, NULL);
	chk(status, "clGetDeviceIDs");
	device_selected = devices[selected_device_number - 1];

	// Print out selected platform and device info.(Information printed : Platform number, Device number, Device name, Device OpenCL C version, Device type)
	char* value;
	int* device_type;
	size_t valueSize;

	status = clGetDeviceInfo(device_selected, CL_DEVICE_NAME, 0, NULL, &valueSize);
	chk(status, "clGetDeviceInfo");
	value = (char*)malloc(valueSize);
	status = clGetDeviceInfo(device_selected, CL_DEVICE_NAME, valueSize, value, NULL);
	chk(status, "clGetDeviceInfo");
	printf("Selected Device : \nPlatform - %d, Device - %d\nDevice name - %s\n", selected_platform_number, selected_device_number, value);
	free(value);
	status = clGetDeviceInfo(device_selected, CL_DEVICE_OPENCL_C_VERSION, 0, NULL, &valueSize);
	chk(status, "clGetDeviceInfo");
	value = (char*)malloc(valueSize);
	status = clGetDeviceInfo(device_selected, CL_DEVICE_OPENCL_C_VERSION, valueSize, value, NULL);
	chk(status, "clGetDeviceInfo");
	printf("OpenCL C version - %s\n", value);
	free(value);
	status = clGetDeviceInfo(device_selected, CL_DEVICE_TYPE, 0, NULL, &valueSize);
	chk(status, "clGetDeviceInfo");
	device_type = (int*)malloc(valueSize);
	status = clGetDeviceInfo(device_selected, CL_DEVICE_TYPE, valueSize, device_type, NULL);
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

	

}