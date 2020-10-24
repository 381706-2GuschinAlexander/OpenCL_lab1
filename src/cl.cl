__kernel void print(__global int * A, __global int *B, const unsigned int count)
{
    int i = get_global_id(0);
	int g = get_group_id(0);
	int l = get_local_id(0);
	if(i < count){
		printf("%d %d %d\n", g, l, i);
		B[i] = A[i] + i;
	}
}