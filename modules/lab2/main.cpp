#include <CL/cl.h>
#include <omp.h>
#include <iostream>
#include <fstream> 
#include <random>
#include <string>

int Type_Size(int type) {
  if (type == 1)
    return sizeof(float);
  if (type == 2)
    return sizeof(double);
}

cl_int CL_Run(void* data, void* result, int count, int type, int group_size, void* fx, int incx, int incy) {
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


  for (cl_uint i = 0; i < pl_count; ++i) {
    char platformName[128];
    clGetPlatformInfo(pl_id[i], CL_PLATFORM_NAME,
      128, platformName, nullptr);
    std::cout << platformName << std::endl;
  }


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

  std::ifstream source("../../../modules/lab2/cl.cl", std::ios_base::in);

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


  std::cout << cont;

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
    (type ==1 ? "saxpy" : "daxpy"),
    NULL);

  cl_mem input = clCreateBuffer(
    context,
    CL_MEM_READ_ONLY,
    Type_Size(type) * size,
    NULL,
    NULL);

  cl_mem output = clCreateBuffer(
    context,
    CL_MEM_READ_WRITE,
    Type_Size(type) * size,
    NULL,
    NULL);


  clEnqueueWriteBuffer(
    queue,
    input,
    CL_TRUE,
    0,
    Type_Size(type)* size,
    data,
    0,
    NULL,
    NULL);


  clSetKernelArg(
    kernel,
    0,
    sizeof(cl_mem),
    &input
  );

  clSetKernelArg(
    kernel,
    1,
    sizeof(cl_mem),
    &output
  );

  clSetKernelArg(
    kernel,
    2,
    sizeof(unsigned int),
    &count
  );

  clSetKernelArg(
    kernel,
    3,
    Type_Size(type),
    fx
  );

  clSetKernelArg(
    kernel,
    4,
    sizeof(int),
    &incx
  );

  clSetKernelArg(
    kernel,
    5,
    sizeof(int),
    &incy
  );

  size_t group = 0;

  clGetKernelWorkGroupInfo(
    kernel,
    device,
    CL_KERNEL_WORK_GROUP_SIZE,
    sizeof(size_t),
    &group,
    NULL);

  clEnqueueNDRangeKernel(
    queue,
    kernel,
    1,
    NULL,
    &size,
    &group,
    0,
    NULL,
    NULL);

  clEnqueueReadBuffer(
    queue,
    output,
    CL_TRUE,
    0,
    Type_Size(type) * size,
    result,
    0,
    NULL,
    NULL);

  if(type == 1)
    for (int i = 0; i < count; ++i)
      std::cout << (reinterpret_cast<float*>(data))[i] << "  " << (reinterpret_cast<float*>(result))[i] << "\n";
  else
    for (int i = 0; i < count; ++i)
      std::cout << (reinterpret_cast<double*>(data))[i] << "  " << (reinterpret_cast<double*>(result))[i] << "\n";

  clReleaseMemObject(input);
  clReleaseMemObject(output);
  clReleaseProgram(program);
  clReleaseKernel(kernel);
  clReleaseCommandQueue(queue);
  clReleaseContext(context);

  return 0;
}











int main() {
  //int size = 1024;
  int count = 1024;
  int incx = 1, incy = 1;

  float* X = new float[count];
  float* Y = new float[count];

  double* X_d = new double[count];
  double* Y_d = new double[count];

  for (int i = 0; i < count; ++i) {
    X[i] = i;
    X_d[i] = i;
    Y[i] = 2.1;
    Y_d[i] = 2.1;
  }
  float fx = 3.1;
  double fx_d = 3.1;


  CL_Run(X, Y, count, 1, 256, &fx, 1, 1);
  
  CL_Run(X_d, Y_d, count, 2, 256, &fx_d, 1, 1);


  for (int i = 0; i < count; ++i) {
    X[i] = i;
    X_d[i] = i;
    Y[i] = 2.1;
    Y_d[i] = 2.1;
  }

  /*#pragma omp parallel for num_threads(4)
  {
    for(int i = 0; i < count; ++i)
      
  }*/
  //for (int i = 0; i < count; ++i)
  //  std::cout <<Y[i] << "\n";

  delete[] X;
  delete[] Y;
  delete[] X_d;
  delete[] Y_d;

  return 0;
}

