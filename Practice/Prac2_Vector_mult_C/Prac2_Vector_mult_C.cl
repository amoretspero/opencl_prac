// kernel code for simple matrix multiplication.
// For valid matrix multiplication, set widthA = heightB.
__kernel void simpleMultiply (__global float* outputC, int widthA, int heightA, int widthB, int heightB, __global float* inputA, __global float* inputB)
{
	// Get Y-direction global position.
	int row = get_global_id(1);

	// Get X-direction global position.
	int col = get_global_id(0);

	float sum = 0.0f;

	// Calculate the result for matrix C's one element.
	int i;
	for (i=0; i < widthA; i++)
	{
		sum += inputA[row * widthA + i] * inputB[i * widthB + col];
	}

	outputC[row * widthB + col] = sum;
}