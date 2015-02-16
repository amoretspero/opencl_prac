// Kernel code for Image rotation respect to Origin.
// dest_data : image from which rotation will be done.
// src_data : rotated image
// W, H : width and height of source image and also the output image.
// sinTheta, cosTheta : value of trigonal function sine and cosine of given angle theta(given in radian).
__kernel void img_rotate(__global float* dest_data, __global float* src_data, int W, int H, float cosTheta, float sinTheta)
{
	// Work items get its index from index space.
	const int ix = get_global_id(0);
	const int iy = get_global_id(1);

	// Calculate the coordinate (ix, iy) where rotated pixel should be.
	// Input decomposition.
	float x0 = W/2.0f;
	float y0 = H/2.0f;

	// Calculate offset respect to center of image calculated by W/2.0f, H/2.0f.
	float xoff = ix - x0;
	float yoff = iy - y0;

	// Calculate rotated coordinate.
	int xpos = (int)(xoff * cosTheta + yoff * sinTheta + x0);
	int ypos = (int)(yoff * cosTheta - xoff * sinTheta + y0);

	// Check if rotated pixel is in boundary of original image. If not, discard.
	if (((int)xpos >= 0) && ((int)xpos < W) && ((int)ypos >= 0) && ((int)ypos < H))
	{
		// Read the value correspond to (ix, iy) in src_data and store it to space correspond to (xpos, ypos).
		// Rotation is done respect to origin, so linear transformation is not needed anymore.
		dest_data[iy * W + ix] = src_data[ypos * W + xpos];
	}
}
