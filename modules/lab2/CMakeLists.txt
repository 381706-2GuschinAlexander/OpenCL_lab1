cmake_minimum_required(VERSION 3.9)

get_filename_component(ProjectId ${CMAKE_CURRENT_SOURCE_DIR} NAME)
string(REPLACE " " "_" ProjectId ${ProjectId})
project(${ProjectId})

add_executable(${ProjectId} main.cpp)

find_package(OpenCL REQUIRED)
target_link_libraries(${PROJECT_NAME} OpenCL::OpenCL)

find_package(OpenMP REQUIRED)
target_link_libraries(${PROJECT_NAME} OpenMP::OpenMP_CXX)
