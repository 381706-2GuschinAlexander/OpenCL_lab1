__kernel void matrix_mul(__global float * X, __global float *Y, __global float *C, const unsigned int n, const unsigned int m)
{
	int i = get_global_id(0);
	int j = get_global_id(1);
	int ind = i + j * m;
	float sum = 0;
	for(int k = 0; k  < m; ++k){
		sum += X[j * m + k] * Y[j + k * n];
	}
	C[ind] = sum;
}

__kernel void matrix_mul_optimized(__global float * X, __global float *Y, const unsigned int n, const unsigned int m, const unsigned int k)
{
    int i = get_global_id(0);
}