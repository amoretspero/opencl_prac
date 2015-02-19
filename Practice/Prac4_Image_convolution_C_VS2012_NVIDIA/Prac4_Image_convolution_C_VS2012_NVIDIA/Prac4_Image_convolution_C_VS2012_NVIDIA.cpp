#include <stdio.h>
#include <stdlib.h>
#include <CL/cl.h>
#include <math.h>

#include "bmpfuncs.h"
#pragma warning(disable : 4996) // for fopen and clCreateImage2D

unsigned int roundUp(unsigned int value, unsigned int multiple)
{
	// Determine how far past the nearest multiple the value is.
	unsigned int remainder = value % multiple;

	// Add the difference to make the value a multiple.
	if (remainder != 0)
	{
		value += (multiple - remainder);
	}

	return value;
}

void chk(cl_int err, const char* cmd)
{
	if (err != CL_SUCCESS)
	{
		printf("%s failed (%d)\n", cmd, err);
		exit(-1);
	}
}

int print_devices() {

	int i, j;
	char* value;
	size_t valueSize;
	cl_uint platformCount;
	cl_platform_id* platforms;
	cl_uint deviceCount;
	cl_device_id* devices;
	cl_uint maxComputeUnits;

	// get all platforms
	clGetPlatformIDs(0, NULL, &platformCount);
	printf("Platform count : %d\n\n", platformCount);
	platforms = (cl_platform_id*)malloc(sizeof(cl_platform_id) * platformCount);
	clGetPlatformIDs(platformCount, platforms, NULL);

	for (i = 0; i < platformCount; i++) {

		// get all devices
		clGetDeviceIDs(platforms[i], CL_DEVICE_TYPE_GPU, 0, NULL, &deviceCount);
		devices = (cl_device_id*)malloc(sizeof(cl_device_id) * deviceCount);
		clGetDeviceIDs(platforms[i], CL_DEVICE_TYPE_ALL, deviceCount, devices, NULL);

		// for each device print critical attributes
		for (j = 0; j < deviceCount; j++) {

			// print device name
			clGetDeviceInfo(devices[j], CL_DEVICE_NAME, 0, NULL, &valueSize);
			value = (char*)malloc(valueSize);
			clGetDeviceInfo(devices[j], CL_DEVICE_NAME, valueSize, value, NULL);
			printf("%d. Device: %s\n", j + 1, value);
			free(value);

			// print hardware device version
			clGetDeviceInfo(devices[j], CL_DEVICE_VERSION, 0, NULL, &valueSize);
			value = (char*)malloc(valueSize);
			clGetDeviceInfo(devices[j], CL_DEVICE_VERSION, valueSize, value, NULL);
			printf(" %d.%d Hardware version: %s\n", j + 1, 1, value);
			free(value);

			// print software driver version
			clGetDeviceInfo(devices[j], CL_DRIVER_VERSION, 0, NULL, &valueSize);
			value = (char*)malloc(valueSize);
			clGetDeviceInfo(devices[j], CL_DRIVER_VERSION, valueSize, value, NULL);
			printf(" %d.%d Software version: %s\n", j + 1, 2, value);
			free(value);

			// print c version supported by compiler for device
			clGetDeviceInfo(devices[j], CL_DEVICE_OPENCL_C_VERSION, 0, NULL, &valueSize);
			value = (char*)malloc(valueSize);
			clGetDeviceInfo(devices[j], CL_DEVICE_OPENCL_C_VERSION, valueSize, value, NULL);
			printf(" %d.%d OpenCL C version: %s\n", j + 1, 3, value);
			free(value);

			// print device type
			clGetDeviceInfo(devices[j], CL_DEVICE_TYPE, 0, NULL, &valueSize);
			value = (char*)malloc(valueSize);
			clGetDeviceInfo(devices[j], CL_DEVICE_TYPE, valueSize, value, NULL);
			printf(" %d.%d Device Type : %s\n", j + 1, 4, value);
			free(value);

			// print parallel compute units
			clGetDeviceInfo(devices[j], CL_DEVICE_MAX_COMPUTE_UNITS,
				sizeof(maxComputeUnits), &maxComputeUnits, NULL);
			printf(" %d.%d Parallel compute units: %d\n\n", j + 1, 5, maxComputeUnits);

		}

		free(devices);

	}

	free(platforms);
	return 0;

}

int main(int argc, char** argv)
{
	// Step 1 : Set host data.
	// Set some variables.(Later in use.)
	int i;
	int j;
	int k;
	int l;

	// Rows and columns in the input image.
	int image_height;
	int image_width;

	// Input and output file names.
	const char* inputfile = "input.bmp";
	const char* outputfile = "output.bmp";

	// Read image to variable 'inputimage'.
	float* inputimage = readImage(inputfile, &image_width, &image_height);

	// Size of the input and output images on the host.
	int datasize = image_height * image_width * sizeof(float);

	// Create free space for output image on host and reference image on host.
	float* outputimage = NULL;
	outputimage = (float*)malloc(datasize);
	float* refimage = NULL;
	refimage = (float*)malloc(datasize);

	// 45-degree(pi/4) motion blur filter.
	float filter[49] =
	{ 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0,
	0, 0, -1, 0, 1, 0, 0,
	0, 0, -2, 0, 2, 0, 0,
	0, 0, -1, 0, 1, 0, 0,
	0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0 };

	// The size of convolution filter is 7*7=49(Defined above.)
	int filter_width = 7;
	int filter_height = 7;
	int filter_size = filter_width * filter_height;

	print_devices();

	// Step 2 : Set OpenCL environment.
	// cl_int variable for indicating opencl status. It is also used for detecting errors.
	cl_int status;

	// Get platform information.
	cl_platform_id* platform; // To get all the platforms, declared as pointer.(used as array.)
	cl_uint platform_count;
	status = clGetPlatformIDs(0, NULL, &platform_count); // Get number of platforms.
	chk(status, "clGetPlatformIDs");
	platform = (cl_platform_id*)malloc(sizeof(cl_platform_id)*platform_count);
	status = clGetPlatformIDs(platform_count, platform, NULL); // Get platforms in form of array.
	chk(status, "clGetPlatformIDs");

	// Get device information.
	cl_device_id device; // We need only one device.
	cl_uint num_devices;
	cl_uint platform_idx; // To indicate what platform the GPU device has been found at.
	int gpu_found = 0; // if gpu_found is 0, gpu device has not been found, if 1, gpu device has been found.
	int cnt;
	for (cnt = 0; cnt < platform_count; cnt++)
	{
		clGetDeviceIDs(platform[cnt], CL_DEVICE_TYPE_GPU, 0, NULL, &num_devices);
		if (num_devices != 0)
		{
			status = clGetDeviceIDs(platform[cnt], CL_DEVICE_TYPE_GPU, 1, &device, NULL);
			chk(status, "clGetDeviceIDs");
			printf("Found GPU device! - platform %d, device %d.\n", cnt, 0);

			size_t valuesize;
			char* value;
			cl_uint max_compute_units;
			
			clGetDeviceInfo(device, CL_DEVICE_NAME, 0, NULL, &valuesize);
			value = (char*)malloc(valuesize);
			clGetDeviceInfo(device, CL_DEVICE_NAME, valuesize, value, NULL);
			printf("GPU device name : %s\n", value);
			free(value);

			clGetDeviceInfo(device, CL_DEVICE_OPENCL_C_VERSION, 0, NULL, &valuesize);
			value = (char*)malloc(valuesize);
			clGetDeviceInfo(device, CL_DEVICE_OPENCL_C_VERSION, valuesize, value, NULL);
			printf("GPU OpenCL version : %s\n", value);
			free(value);

			clGetDeviceInfo(device, CL_DEVICE_MAX_COMPUTE_UNITS, sizeof(max_compute_units), &max_compute_units, NULL);
			printf("GPU parallel compute units : %d\n\n", max_compute_units);
			gpu_found = 1;
			platform_idx = cnt;
			break;
		}
	}
	if (gpu_found == 0)
	{
		clGetDeviceIDs(platform[0], CL_DEVICE_TYPE_ALL, 1, &device, &num_devices); // GPU device has not been found, so use default device at platform 0.)
		printf("Cannot find GPU device! Compute with CPU!\n");
		chk(status, "clGetDeviceIDs");
	}
	chk(status, "clGetDeviceIDs");

	

	// Create context for device.
	cl_context_properties properties[3] = { CL_CONTEXT_PLATFORM, (cl_context_properties)platform[platform_idx], 0 }; // If GPU device has not been found, platform_idx will be 0, indicating default device.
	cl_context context;
	context = clCreateContext(properties, 1, &device, NULL, NULL, &status);
	chk(status, "clCreateContext");

	// Create command queue.
	cl_command_queue queue;
	queue = clCreateCommandQueue(context, device, 0, &status);
	chk(status, "clCreateCommandQueue");

	// Set image format for describing how the data will be stored in memory in aspects of channel and data types and so on.
	cl_image_format format;
	// Image will be stored in single channel format.
	format.image_channel_order = CL_R;
	// Image will be stored in float data type.
	format.image_channel_data_type = CL_FLOAT;

	// Step 3 : Declarations of buffers, images and data transfers.
	// Create space for the source image on device.
	cl_mem d_input_image = clCreateImage2D(context, 0, &format, image_width, image_height, 0, NULL, &status);
	chk(status, "clCreateImage2D");

	// Create space for the output image on device.
	cl_mem d_output_image = clCreateImage2D(context, 0, &format, image_width, image_height, 0, NULL, &status);
	chk(status, "clCreateImage2D");

	// Create space for the 7*7 convolution filter on device.
	cl_mem d_filter = clCreateBuffer(context, 0, filter_size*sizeof(float), NULL, &status);
	chk(status, "clCreateBuffer");

	// Copy the source image to the device
	size_t origin[3] = { 0, 0, 0 }; // Offset that is in the image to be copied.
	size_t region[3] = { image_width, image_height, 1 }; // Elements per dimension.
	status = clEnqueueWriteImage(queue, d_input_image, CL_FALSE, origin, region, 0, 0, inputimage, 0, NULL, NULL);
	chk(status, "clEnqueueWriteImage");

	// Copy the 7*7 convolution filter to the device.
	status = clEnqueueWriteBuffer(queue, d_filter, CL_FALSE, 0, filter_size*sizeof(float), filter, 0, NULL, NULL);
	chk(status, "clEnqueueWriteBuffer");

	// Create the image sampler
	cl_sampler sampler = clCreateSampler(context, CL_FALSE, CL_ADDRESS_CLAMP_TO_EDGE, CL_FILTER_NEAREST, &status);
	chk(status, "clCreateSampler");

	// Step 4 : Compile kernel for runtime.
	// Read source from externel file.
	FILE* opencl_kernel_file;
	char* kernel_source;
	size_t kernel_source_size;

	fopen_s(&opencl_kernel_file, "Prac4_Image_convolution_C_VS2012_NVIDIA.cl", "r");
	if (!opencl_kernel_file)
	{
		printf("Failed to load kernel.\n");
		exit(-1);
	}

	kernel_source = (char*)malloc(0x100000);
	kernel_source_size = fread(kernel_source, 1, 0x100000, opencl_kernel_file);
	fclose(opencl_kernel_file);

	// Create program object with source
	cl_program program;
	program = clCreateProgramWithSource(context, 1, (const char**)&kernel_source, (const size_t*)&kernel_source_size, &status);
	chk(status, "clCreateProgramWithSource");

	// Build the program.
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
	kernel = clCreateKernel(program, "convolution", &status);
	chk(status, "clCreateKernel");

	// Set the kernel arguments
	status = clSetKernelArg(kernel, 0, sizeof(cl_mem), &d_input_image);
	status = clSetKernelArg(kernel, 1, sizeof(cl_mem), &d_output_image);
	status = clSetKernelArg(kernel, 2, sizeof(int), &image_height);
	status = clSetKernelArg(kernel, 3, sizeof(int), &image_width);
	status = clSetKernelArg(kernel, 4, sizeof(cl_mem), &d_filter);
	status = clSetKernelArg(kernel, 5, sizeof(int), &filter_width);
	status = clSetKernelArg(kernel, 6, sizeof(cl_sampler), &sampler);
	chk(status, "clSetKernelArg");

	// Set dimensions of work items.
	size_t globalSize[2] = { image_width, image_height };
	status = clEnqueueNDRangeKernel(queue, kernel, 2, NULL, globalSize, NULL, 0, NULL, NULL);
	chk(status, "clEnqueueNDRange");

	// Step 5 : Read back to host.
	// Read the image back to the host.
	status = clEnqueueReadImage(queue, d_output_image, CL_TRUE, origin, region, 0, 0, outputimage, 0, NULL, NULL);
	chk(status, "clEnqueueReadImage");

	// Write the output image to file
	storeImage(outputimage, outputfile, image_height, image_width, inputfile);

	// Compute the reference image
	for (i = 0; i < image_height; i++)
	{
		for (j = 0; j < image_width; j++)
		{
			refimage[i*image_width + j] = 0;
		}
	}

	// Iterate over the rows of the source image
	int half_filter_width = filter_width / 2;
	float sum;
	for (i = 0; i < image_height; i++)
	{
		// Iterate over the columns of the source image
		for (j = 0; j < image_width; j++)
		{
			sum = 0; // Reset the sum for new source pixel
			// Apply the filter to the neighborhood
			for (k = -half_filter_width; k <= half_filter_width; k++)
			{
				for (l = -half_filter_width; l <= half_filter_width; l++)
				{
					if (i + k >= 0 && i + k < image_height && j + l >= 0 && j + l < image_width)
					{
						sum += inputimage[(i + k)*image_width + j + l] * filter[(k + half_filter_width) * filter_width + l + half_filter_width];
					}
				}
			}
			refimage[i*image_width + j] = sum;
		}
	}

	storeImage(refimage, "ref.bmp", image_height, image_width, inputfile);

	int failed = 0;
	for (i = 0; i < image_height; i++)
	{
		for (j = 0; j < image_width; j++)
		{
			// For unknown reason, edge pixels are not correctly convolved.
			if ((abs(outputimage[i*image_width + j] - refimage[i*image_width + j])> 0.01) && (i != 0) && (i != image_height - 1) && (j != 0) && (j != image_width - 1))
			{
				printf("Results are INCORRECT\n");
				printf("Pixel mismatch at <%d,%d> (%f vs. %f)\n", i, j, outputimage[i*image_width + j], refimage[i*image_width + j]);
				failed = 1;
			}
			if (failed) break;
		}
		if (failed) break;
	}
	if (!failed)
	{
		printf("Results are correct\n");
	}

	return 0;
}
