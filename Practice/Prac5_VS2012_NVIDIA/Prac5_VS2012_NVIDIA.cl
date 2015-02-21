/*__kernel void simplekernel(__global int* a, __global int* b, __local int* l)
{
	l[get_local_id(0)] = a[get_global_id(0)];
	barrier(CLK_LOCAL_MEM_FENCE);
	unsigned int otherAddress = (get_local_id(0) + 1) % get_local_size(0);
	b[get_local_id(0)] = l[get_local_id(0)] + l[otherAddress];
}*/
__kernel void simplekernel(__global int* a, __global int* b)
{
	int address = get_global_id(0) + (get_global_id(1) * get_global_size(0));
	b[address] = a[address] * 2;
}