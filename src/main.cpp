#include <CL/cl.h>
#include <iostream>
#include <fstream> 
#include <string>





int main() {


  size_t size = 1024;
  size_t count = 1000;


  cl_int error_code;

  cl_uint pl_count;
  if (clGetPlatformIDs(256, NULL, &pl_count) != CL_SUCCESS) {
    std::cout << "No platform avaible \n";
    return 1;
  }
  cl_platform_id * pl_id = new cl_platform_id[pl_count];
  if(clGetPlatformIDs(pl_count, pl_id, &pl_count) != CL_SUCCESS) {
    std::cout << "Cant write platform id \n";
      return 2;
  }

  cl_platform_id platform = pl_id[0];
  delete[] pl_id;

  cl_uint dev_count;
  if (clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, 256, NULL, &dev_count) != CL_SUCCESS) {
    std::cout << "No device avaible \n";
    return 1;
  }
  cl_device_id * dev_id = new cl_device_id[dev_count];
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
 
  std::ifstream source("../src/cl.cl", std::ios_base::in);

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

  //for (int i = 0; i < length; ++i)
  //  std::cout << csource[i];

  source.close();

 cl_program program = clCreateProgramWithSource(
    context,
    1,
    &csource,
    &length,
    &error_code);


 if (error_code != CL_SUCCESS)
   std::cout <<"Create error: " <<error_code;

  error_code = clBuildProgram(
    program,
    1,
    &device,
    NULL,
    NULL,
    NULL);

  if (error_code != CL_SUCCESS)
    std::cout<< "Build error:" << error_code;

  cl_kernel kernel = clCreateKernel(
    program,
    "print",
    NULL);

  int* data = new int[size];
  int* result = new int[size]; 
  for (int i = 0; i < count; ++i)
    data = 0;


  cl_mem input = clCreateBuffer(
    context,
    CL_MEM_READ_ONLY,
    sizeof(int) * size,
    NULL,
    NULL);

  cl_mem output = clCreateBuffer(
    context,
    CL_MEM_WRITE_ONLY,
    sizeof(int) * size,
    NULL,
    NULL);


  clEnqueueWriteBuffer(
    queue,
    input,
    CL_TRUE,
    0,
    sizeof(float)* size,
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
    sizeof(int) * size,
    result,
    0,
    NULL,
    NULL);


  for (int i = 0; i < count; ++i)
    std::cout << result[i] << "\n";

  delete[] result;
  delete[] data;
  clReleaseMemObject(input);
  clReleaseMemObject(output);
  clReleaseProgram(program);
  clReleaseKernel(kernel);
  clReleaseCommandQueue(queue);
  clReleaseContext(context);

  return 0;
}

