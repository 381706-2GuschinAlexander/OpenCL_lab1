__kernel void print(__global float * X, __global float *Y, const unsigned int count, const float fx)
{
    int i = get_global_id(0);
    if(i < count){
        Y[i] = Y[i] + fx * X[i];
    }
}