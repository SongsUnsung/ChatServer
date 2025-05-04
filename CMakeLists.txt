cmake_minimum_required(VERSION 3.0)
project(chat)

set(CMAKE_CXX_FLAG ${CMAKE_CXX_FLAGS} -g)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)


include_directories(${PROJECT_SOURCE_DIR}/include)
include_directories($(PROJECT_SOURCE_DIR)/include/server)

add_subdirectory(src)


