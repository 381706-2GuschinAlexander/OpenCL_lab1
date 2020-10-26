__kernel void saxpy(__global float * X, __global float *Y, const unsigned int count, const float fx,const int incX, const int incY)
{
    int i = get_global_id(0);
    if(i < count){
        Y[i * incY] = Y[i * incY] + fx * X[i * incX];
    }
}

__kernel void daxpy(__global double * X, __global double *Y, const unsigned int count, const double fx,const int incX, const int incY)
{
    int i = get_global_id(0);
    if(i < count){
        Y[i * incY] = Y[i * incY] + fx * X[i * incX];
    }
}