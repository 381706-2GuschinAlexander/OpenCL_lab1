#include <memory>
#include <CL/cl.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys\types.h>
#include <sys\stat.h>
#include <io.h> 


#define MAX_SRC_SIZE (0x100000)



int main() {
  int         i;
  const int   arr_size = 1024;                               // размер наших массивов

  int* A = (int*)malloc(sizeof(int) * arr_size);    // выдел€ем место под массивы ј и ¬
  int* B = (int*)malloc(sizeof(int) * arr_size);

  for (i = 0; i < arr_size; i++)                      // наполн€ем массивы данными дл€ отправки в кернель OpenCL
  {
    A[i] = i;
    B[i] = arr_size - i;
  }


  int         fd;
  char* src_str;              // сюда будет записан исходный код кернел€
  size_t      src_size;

  src_str = (char*)malloc(MAX_SRC_SIZE);     // выдел€ем пам€ть дл€ исходного кода
  fd = open("vector_add.cl", O_RDONLY);       // открываем файл с кодом кернел€
  if (fd <= 0)
  {
    fprintf(stderr, "Ќе получилось считать исходный код кернел€.\n");
    exit(1);
  }
  src_size = read(fd, src_str, MAX_SRC_SIZE);   // записываем исходный код в src_str
  close(fd);

  cl_platform_id        platform_id = NULL;    // обратите внимание на типы данных
  cl_device_id          device_id = NULL;
  cl_uint ret_num_devices;
  cl_uint ret_num_platforms;
  cl_int  ret;                   // сюда будут записыватьс€ сообщени€ об ошибках

  ret = clGetPlatformIDs(1, &platform_id, &ret_num_platforms);
  ret = clGetDeviceIDs(platform_id, CL_DEVICE_TYPE_GPU, 1,
    &device_id, &ret_num_devices);

  cl_context             context;
  cl_command_queue       command_queue;

  context = clCreateContext(NULL, 1, &device_id, NULL, NULL, &ret);
  command_queue = clCreateCommandQueue(context, device_id, 0, &ret);

  return 0;
}