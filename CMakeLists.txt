cmake_minimum_required( VERSION 3.9 )

project( CL_Lab )

file(GLOB SUBDIRS "${CMAKE_CURRENT_SOURCE_DIR}/modules/*")

foreach(subdir ${SUBDIRS})
  add_subdirectory(${subdir})
endforeach()