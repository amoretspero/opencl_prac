#pragma OPENCL EXTENSION cl_amd_printf : enable

__kernel void vector_add (__global float* inputA, __global float* inputB, __global float* output)
{
	int index = get_global_id(0);
	output[index] = inputA[index] + inputB[index];
}