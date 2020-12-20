__kernel void matrix_mul(__global float * X, __global float *Y, __global float *C, const unsigned int n, const unsigned int m)
{
	int i = get_global_id(0);
	int j = get_global_id(1);
	int ind = i * n + j;
	float sum = 0;
	for(int k = 0; k  < m; ++k){
		sum += X[i * m + k] * Y[j + k * n];
	}
	C[ind] = sum;
	//printf("%f %f\n", sum, C[ind]);
}

__kernel void matrix_mul_optimized(__global float * X, __global float *Y, __global float *C, const unsigned int n, const unsigned int m)
{
	__local float A[16*16];
	__local float B[16*16];

    int i_global = get_global_id(0);
	int j_global = get_global_id(1);
	int i_local = get_local_id(0);
	int j_local = get_local_id(1);
	float sum = 0;
	for(int k = 0; k  < m / 16; ++k){
		A[i_local * 16 + j_local] = X[i_global * m + (k * 16 + j_local)];
		B[i_local * 16 + j_local] = Y[(k * 16 + j_local) * n + i_local + j_global - j_global % 16];
		barrier(CLK_LOCAL_MEM_FENCE);
		//if(k == 0){
		//printf("X %d \n", i_global * m + (k * 16 + j_local));
		//printf("A %d %d %f X %d Y %d\n",i_local, j_local, A[i_local * 16 + j_local], i_global, j_global);}
		for(int i = 0; i < 16; ++i){
			sum += A[i_local * 16 + i] * B[j_local * 16 + i];
		}
		barrier(CLK_LOCAL_MEM_FENCE);
	}
	C[i_global * n + j_global] = sum;
}