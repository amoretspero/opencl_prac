// Kernel for image convolution.
__kernel void convolution(__read_only image2d_t sourceImage, __write_only image2d_t outputImage, int rows, int cols, __constant float* filter, int filterWidth, sampler_t sampler)
{
	// Store each work item's unique column and row.
	int column = get_global_id(0);
	int row = get_global_id(1);

	// To use later for indexing memory, we need the value which is half of filter width.
	int halfWidth = (int)(filterWidth/2);

	// All access to image returns 4-dimensional vector(float4). In this code, only 'x' element contains meaningful information.
	float4 sum = {0.0f, 0.0f, 0.0f, 0.0f};

	// Iterator for filter.
	int filterIdx = 0;

	// Each work item relies on filter size.
	// Repeat same work to local scope.
	int2 coords; // Coordinate for accessing image.

	// Repeat in accordance with filter's row.
	for (int i = -halfWidth; i <= halfWidth; i++)
	{
		coords.y = row + i;

		// Repeat in accordance with filter's column.
		for (int j = -halfWidth; j <= halfWidth; j++)
		{
			coords.x = column + j;

			float4 pixel;
			// Read pixel from image.
			// single channel image stores the 'x' element(1st element) of return vector(4-dimensional).
			pixel = read_imagef(sourceImage, sampler, coords);
			sum.x += pixel.x * filter[filterIdx++];
		}
	}

	// Only stores the data to output image if work item is in given interval.
	if (row < rows && column < cols)
	{
		coords.x = column;
		coords.y = row;
		write_imagef(outputImage, coords, sum);
	}
}