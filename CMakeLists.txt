cmake_minimum_required( VERSION 3.9 )

project( CL_Lab )

find_package(OpenCL REQUIRED)

##include_directories( ${OPENCL_INCLUDE_DIRS} )

add_executable(${PROJECT_NAME} src/main.cpp )

target_link_libraries(${PROJECT_NAME} OpenCL::OpenCL)