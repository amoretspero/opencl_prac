#define __CL_ENABLE_EXCEPTIONS

#include "stdafx.h"



int main()
{
	const int N_ELEMENTS = 1024;
	int* A = new int[N_ELEMENTS];
	int* B = new int[N_ELEMENTS];
	int* C = new int[N_ELEMENTS];
	for (int i = 0; i < N_ELEMENTS; i++)
	{
		A[i] = i;
		B[i] = i;
	}

	try
	{
		// Query for platforms.
		std::vector<cl::Platform>platforms;
		cl::Platform::get(&platforms);
		if (platforms.size() == 0) {
			std::cout << "Platform size 0\n";
			return -1;
		}

		// Get the device list of this platform.
		std::vector<cl::Device>devices;
		platforms[0].getDevices(CL_DEVICE_TYPE_GPU, &devices);
		//cl_context_properties properties[] = { CL_CONTEXT_PLATFORM, (cl_context_properties)(platforms[0])(), 0 };
		//cl::Context context(CL_DEVICE_TYPE_GPU, properties);
		//std::vector<cl::Device>devices = context.getInfo<CL_CONTEXT_DEVICES>();

		// Create context for devices.
		cl::Context context(devices);

		// Create command queue for first device.
		cl::CommandQueue queue = cl::CommandQueue(context, devices[0]);

		// Create memory buffers.
		cl::Buffer bufferA = cl::Buffer(context, CL_MEM_READ_ONLY, N_ELEMENTS * sizeof(int));
		cl::Buffer bufferB = cl::Buffer(context, CL_MEM_READ_ONLY, N_ELEMENTS * sizeof(int));
		cl::Buffer bufferC = cl::Buffer(context, CL_MEM_WRITE_ONLY, N_ELEMENTS * sizeof(int));

		// Using command queue for first device, copy input data to input buffer.
		queue.enqueueWriteBuffer(bufferA, CL_TRUE, 0, N_ELEMENTS * sizeof(int), A);
		queue.enqueueWriteBuffer(bufferB, CL_TRUE, 0, N_ELEMENTS * sizeof(int), B);

		// Read program source.
		std::ifstream sourceFile("vector_add_kernel.cl");
		std::string sourceCode(
			std::istreambuf_iterator<char>(sourceFile),
			(std::istreambuf_iterator<char>()));
		cl::Program::Sources source(1, std::make_pair(sourceCode.c_str(), sourceCode.length() + 1));

		// Create program from source code.
		cl::Program program = cl::Program(context, source);

		// Build program for devices.
		program.build(devices);

		// Create kernel.
		cl::Kernel vecadd_kernel(program, "vector_add");

		// Configure arguments for kernel.
		vecadd_kernel.setArg(0, bufferA);
		vecadd_kernel.setArg(1, bufferB);
		vecadd_kernel.setArg(2, bufferC);

		// Run kernel.
		cl::NDRange global(N_ELEMENTS);
		cl::NDRange local(256);
		queue.enqueueNDRangeKernel(vecadd_kernel, cl::NullRange, global, local);

		// Copy output data to host.
		queue.enqueueReadBuffer(bufferC, CL_TRUE, 0, N_ELEMENTS * sizeof(int), C);

		// Verify results.
		bool result = true;
		for (int i = 0; i < N_ELEMENTS; i++)
		{
			if (C[i] != A[i] + B[i])
			{
				result = false;
				break;
			}
		}

		if (result)
			std::cout << "Success!" << std::endl;
		else
			std::cout << "Failed!" << std::endl;
	}
	catch (cl::Error error)
	{
		std::cout << error.what() << "(" << error.err() << ")" << std::endl;
	}
	return 0;
}
