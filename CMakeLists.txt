# Project name.
cmake_minimum_required(VERSION 2.8)
project(Tracklib)

# OpenCV dependency.
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake-modules)
find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

# Compiler settings.
set(CMAKE_CXX_FLAGS "-g -Wall")

# Subdirectory list.
set(SUB_DIRS tl_core)

# Include source files.
aux_source_directory(. SRC_LIST)
foreach(SUB_DIR ${SUB_DIRS})
  add_subdirectory(${SUB_DIR})
  aux_source_directory(${SUB_DIR} SRC_LIST)
endforeach(SUB_DIR ${SUB_DIRS})

# Define exectuable.
add_executable(${PROJECT_NAME} ${SRC_LIST})

# Link libraries.
target_link_libraries(${PROJECT_NAME} ${OpenCV_LIBS})
