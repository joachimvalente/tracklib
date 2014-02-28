# Project name.
cmake_minimum_required(VERSION 2.8)
project(Tracklib)

# Cuda dependency.
#find_package(CUDA)
if(DEFINED CUDA_INCLUDE_DIRS)
  include_directories(${CUDA_INCLUDE_DIRS})
endif()

# OpenCV dependency.
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake-modules)
find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

# Compiler settings.
set(CMAKE_CXX_FLAGS "-g -Wall -Wno-overloaded-virtual -std=c++11")

# Subdirectory list.
set(SUB_DIRS tl_core
             tl_detectors
             tl_util)

# Include source files.
include_directories(.)
set(SRC_LIST ${SRC_LIST} common.h tracklib.h)
aux_source_directory(. SRC_LIST)
foreach(SUB_DIR ${SUB_DIRS})
  add_subdirectory(${SUB_DIR})
  aux_source_directory(${SUB_DIR} SRC_LIST)
endforeach(SUB_DIR ${SUB_DIRS})

# Include GPU souce files if CUDA was found.
if(DEFINED CUDA_INCLUDE_DIRS)
  add_definitions(-DTL_CUDA)
  add_subdirectory(tl_gpu)
  aux_source_directory(tl_gpu SRC_LIST)
endif()

# Define exectuable.
add_executable(${PROJECT_NAME} ${SRC_LIST})

# Link libraries.
if(DEFINED CUDA_INCLUDE_DIRS)
  target_link_libraries(${PROJECT_NAME} ${OpenCV_LIBS} ${CUDA_LIBRARIES})
else()
  target_link_libraries(${PROJECT_NAME} ${OpenCV_LIBS})
endif()
