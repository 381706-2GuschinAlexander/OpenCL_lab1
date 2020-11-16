#include <CL/cl.h>
#include <omp.h>
#include <iostream>
#include <fstream> 
#include <random>
#include <string>

cl_int CL_Run(void* data_X, void* data_Y, void* result , int count, int n, int m, int type) {
  const size_t size = count;
  
  cl_int error_code;
  cl_uint pl_count;
  if (clGetPlatformIDs(256, NULL, &pl_count) != CL_SUCCESS) {
    std::cout << "No platform avaible \n";
    return 1;
  }
  cl_platform_id* pl_id = new cl_platform_id[pl_count];
  if (clGetPlatformIDs(pl_count, pl_id, &pl_count) != CL_SUCCESS) {
    std::cout << "Cant write platform id \n";
    return 2;
  }


  //for (cl_uint i = 0; i < pl_count; ++i) {
  //  char platformName[128];
  //  clGetPlatformInfo(pl_id[i], CL_PLATFORM_NAME,
  //    128, platformName, nullptr);
  //  std::cout << platformName
  //    << "\nCount =  " << count
  //    << "\nGroup size = " << group_size
  //    << "\nType = " << (type == 1 ? "float" : "double")
  //    <<  std::endl;
  //}


  cl_platform_id platform = pl_id[0];
  delete[] pl_id;

  cl_uint dev_count;
  if (clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, 256, NULL, &dev_count) != CL_SUCCESS) {
    std::cout << "No device avaible \n";
    return 1;
  }
  cl_device_id* dev_id = new cl_device_id[dev_count];
  if (clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, 256, dev_id, &dev_count) != CL_SUCCESS) {
    std::cout << "Cant write device id \n";
    return 2;
  }

  cl_device_id device = dev_id[0];


  cl_context_properties properties[3] = {
    CL_CONTEXT_PLATFORM,(cl_context_properties)(platform), 0
  };

  cl_context context = clCreateContextFromType(
    properties,
    CL_DEVICE_TYPE_GPU,
    NULL,
    NULL,
    &error_code);


  if (error_code != CL_SUCCESS)
    std::cout << error_code;



  cl_command_queue queue = clCreateCommandQueue(
    context,
    device,
    0,
    &error_code);

  if (error_code != CL_SUCCESS)
    std::cout << error_code;

  std::ifstream source("../../../modules/lab3/cl.cl", std::ios_base::in);

  std::string line, cont;
  if (source.is_open())
  {
    while (source.good())
    {
      std::getline(source, line);
      cont += line + (char)10;
    }
  }

  const size_t length = cont.length();
  const char* csource = cont.c_str();


  //std::cout << cont;

  source.close();

  cl_program program = clCreateProgramWithSource(
    context,
    1,
    &csource,
    &length,
    &error_code);


  if (error_code != CL_SUCCESS)
    std::cout << "Create error: " << error_code;

  error_code = clBuildProgram(
    program,
    1,
    &device,
    NULL,
    NULL,
    NULL);

  if (error_code != CL_SUCCESS)
    std::cout << "Build error:" << error_code;

  cl_kernel kernel = clCreateKernel(
    program,
    (type == 1 ? "matrix_mul" : "matrix_mul_optimized"),
    NULL);

  cl_mem input_X = clCreateBuffer(
    context,
    CL_MEM_READ_ONLY,
    sizeof(float) * size,
    NULL,
    NULL);

  cl_mem input_Y = clCreateBuffer(
    context,
    CL_MEM_READ_ONLY,
    sizeof(float) * size,
    NULL,
    NULL);

  cl_mem output = clCreateBuffer(
    context,
    CL_MEM_WRITE_ONLY,
    sizeof(float) * size,
    NULL,
    NULL);


  clEnqueueWriteBuffer(
    queue,
    input_X,
    CL_TRUE,
    0,
    sizeof(float) * size,
    data_X,
    0,
    NULL,
    NULL);

  clEnqueueWriteBuffer(
    queue,
    input_Y,
    CL_TRUE,
    0,
    sizeof(float) * size,
    data_Y,
    0,
    NULL,
    NULL);

  clSetKernelArg(
    kernel,
    0,
    sizeof(cl_mem),
    &input_X
  );

  clSetKernelArg(
    kernel,
    1,
    sizeof(cl_mem),
    &input_Y
  );

  clSetKernelArg(
    kernel,
    2,
    sizeof(cl_mem),
    &output
  );

  clSetKernelArg(
    kernel,
    3,
    sizeof(const unsigned int),
    &n
  );

  clSetKernelArg(
    kernel,
    4,
    sizeof(const unsigned int),
    &m
  );

  size_t * grid_size = new size_t[2];
  size_t * group = new size_t[2];
  grid_size[0] = n;
  grid_size[1] = m;
  group[0] = 16;
  group[1] = 16;

  auto start = omp_get_wtime();
  clEnqueueNDRangeKernel(
    queue,
    kernel,
    2,
    NULL,
    grid_size,
    group,
    0,
    NULL,
    NULL);
  clFinish(queue);
  auto end = omp_get_wtime();
  std::cout << (type == 1 ? "float gpu: " : "double gpu: ")<< end - start << std::endl;

  clEnqueueReadBuffer(
    queue,
    output,
    CL_TRUE,
    0,
    sizeof(float) * size,
    result,
    0,
    NULL,
    NULL);



  delete[] group;
  delete[] grid_size;
  clReleaseMemObject(input_X);
  clReleaseMemObject(input_Y);
  clReleaseMemObject(output);
  clReleaseProgram(program);
  clReleaseKernel(kernel);
  clReleaseCommandQueue(queue);
  clReleaseContext(context);

  return 0;
}

int main() {
  int n = 16 * 128;
  int m = 16 * 128;
  int count = n * m;
  float* X = new float[count];
  float* Y = new float[count];
  float* CL_C = new float[count];

  for (int i = 0; i < count; ++i) {
    X[i] = 1;
    Y[i] = 2;
  }

  CL_Run(X, Y, CL_C, count, n, m, 1);

  //for (int i = 0; i < count; ++i) {
  //  std::cout << CL_C[i] << "\n";
  //}

  float* MP_C = new float[count];
  

  auto start = omp_get_wtime();
  #pragma omp parallel for num_threads(6)
  for(int i = 0; i < n; ++i)
    for (int j = 0; j < m; ++j) {
      float sum = 0;
      for (int k = 0; k < m; ++k)
        sum += X[j * m + k] * Y[j + k * n];
      MP_C[i + j * m] = sum;
    }
  auto end = omp_get_wtime();
  std::cout << "omp time: " << end - start << std::endl;

  for (int i = 0; i < count; ++i)
    if (abs(CL_C[i] - MP_C[i]) > .001)
      printf("%d %f %f\n", i, CL_C[i], MP_C[i]);

  delete[] MP_C;

  float* C = new float[count];

  start = omp_get_wtime();
  for (int i = 0; i < n; ++i)
    for (int j = 0; j < m; ++j) {
      float sum = 0;
      for (int k = 0; k < m; ++k)
        sum += X[j * m + k] * Y[j + k * n];
      C[i + j * m] = sum;
    }
  end = omp_get_wtime();
  std::cout << "omp time: " << end - start << std::endl;

  for (int i = 0; i < count; ++i)
    if (abs(CL_C[i] - C[i]) > .001)
      printf("%d %f %f\n", i, CL_C[i], C[i]);
  

  //CL_Run(CL_X_d, CL_Y_d, count, 2, GROUP_SIZE, &fx_d, 1, 1);
  
  delete[] X;
  delete[] Y;
  delete[] CL_C;

  return 0;
}

