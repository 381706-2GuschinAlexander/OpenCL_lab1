cmake_minimum_required(VERSION 3.7)

get_filename_component(ProjectId ${CMAKE_CURRENT_SOURCE_DIR} NAME)
string(REPLACE " " "_" ProjectId ${ProjectId})
project(${ProjectId})

add_executable(${ProjectId} main.cpp)

find_package(OpenCL REQUIRED)
target_link_libraries(${PROJECT_NAME} OpenCL::OpenCL)