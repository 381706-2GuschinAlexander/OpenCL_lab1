#include <CL/cl.h>
#include <iostream>


int main() {
  cl_uint pl_count;
  if (clGetPlatformIDs(256, NULL, &pl_count) != CL_SUCCESS) {
    std::cout << "No platform avaible \n";
    return 1;
  }
  std::unique_ptr<cl_platform_id> pl_id(new cl_platform_id[pl_count]);
  if(clGetPlatformIDs(pl_count, pl_id.get(), &pl_count) != CL_SUCCESS) {
    std::cout << "Cant write platform id \n";
      return 2;
  }

  cl_uint dev_count;
  if (clGetDeviceIDs(pl_id.get()[0], CL_DEVICE_TYPE_GPU, 256, NULL, &dev_count) != CL_SUCCESS) {
    std::cout << "No device avaible \n";
    return 1;
  }
  std::unique_ptr<cl_device_id> dev_id(new cl_device_id[dev_count]);
  if (clGetDeviceIDs(pl_id.get()[0], CL_DEVICE_TYPE_GPU, 256, dev_id.get(), &dev_count) != CL_SUCCESS) {
    std::cout << "Cant write device id \n";
    return 2;
  }

  cl_context_properties properties[3] = {
    CL_CONTEXT_PLATFORM,(cl_context_properties)*pl_id, 0
  };

  cl_context context = clCreateContextFromType(
    properties,
    CL_DEVICE_TYPE_GPU,
    NULL,
    NULL,
    NULL);

  size_t size = 0;

  return 0;
}