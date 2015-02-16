#define __CL_ENABLE_EXCEPTIONS

#include <utility>
#include <iostream>
#include <fstream>
#include <math.h>
#include <string>
#include <CL/cl.hpp>

// BMP utilities

#include "bmpfuncs.h"


int main(int argc, char** argv)
{
	try
	{
		cl_int err;
		// Step 0 : Set the host data.
		// Set theta as pi/6.
		float theta = 3.141592 / 6;

		// Variables for width and height of image.
		int W;
		int H;

		// Set the input and output file names.
		const char* inputFile = "input.bmp";
		const char* outputFile = "output.bmp";

		// Read a BMP data from file.
		float* ip = readImage(inputFile, &W, &H);
		float* op = new float[W*H];

		// Step 1 : Environment settings.
		// Get platform information.
		std::vector<cl::Platform>platforms;
		cl::Platform::get(&platforms);

		// Create context using first platform.
		cl_context_properties cps[3] = { CL_CONTEXT_PLATFORM, (cl_context_properties)(platforms[0])(), 0 };

		// Using platform above, create context for GPU type devices.
		cl::Context context(CL_DEVICE_TYPE_ALL, cps);

		// Get device list from context.
		std::vector<cl::Device>devices = context.getInfo<CL_CONTEXT_DEVICES>();

		// Create command queue at first device.
		cl::CommandQueue queue = cl::CommandQueue(context, devices[0], 0);

		// Step 2 : Declarations of buffers and data transfer.
		// Set buffers for input/output data.
		cl::Buffer d_ip = cl::Buffer(context, CL_MEM_READ_ONLY, W*H*sizeof(float));
		cl::Buffer d_op = cl::Buffer(context, CL_MEM_WRITE_ONLY, W*H*sizeof(float));

		// Copy input data to device.(Assume that input image is array ip.)
		queue.enqueueWriteBuffer(d_ip, CL_TRUE, 0, W*H*sizeof(float), ip);

		// Step 3 : Compile kernel for runtime.
		// Read program source.
		std::ifstream sourceFileName("Prac3_Image_rotation_C++.cl");
		std::string sourceFile(std::istreambuf_iterator<char>(sourceFileName), (std::istreambuf_iterator<char>()));
		cl::Program::Sources rotation_source(1, std::make_pair(sourceFile.c_str(), sourceFile.length() + 1));

		// Create program.
		cl::Program rotation_program(context, rotation_source);
		
		// Build program.
		rotation_program.build(devices);

		// Create kernel.
		cl::Kernel rotation_kernel(rotation_program, "img_rotate");

		// Step 4 : Run program.
		// Set rotation angle as theta.
		float cos_theta = cos(theta);
		float sin_theta = sin(theta);

		// Set kernel arguments.
		rotation_kernel.setArg(0, d_op);
		rotation_kernel.setArg(1, d_ip);
		rotation_kernel.setArg(2, W);
		rotation_kernel.setArg(3, H);
		rotation_kernel.setArg(4, cos_theta);
		rotation_kernel.setArg(5, sin_theta);

		// Set NDRange and size of work groups.
		cl::NDRange globalws(W, H);
		//cl::NDRange localws(16, 16);

		// Run kernel.
		queue.enqueueNDRangeKernel(rotation_kernel, cl::NullRange, globalws, cl::NullRange);

		// Step 5 : Read output data to host.
		// Read output buffer to host.
		queue.enqueueReadBuffer(d_op, CL_TRUE, 0, W*H*sizeof(float), op);

		// Store the rotated image to output file.
		storeImage(op, outputFile, H, W, inputFile);

		return 0;
	}
	catch (cl::Error err)
	{
		std::cout << err.what() << "(" << err.err() << ")" << std::endl;
	}
}