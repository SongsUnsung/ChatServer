cmake_minimum_required(VERSION 3.0)
project(main)

set(CMAKE_CXX_FLAG ${CMAKE_CXX_FLAG} -g)

#add_subdirectory(testmuduo)

set(SRC_LIST ./server.cpp)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

add_executable(server ${SRC_LIST})

target_link_libraries(server muduo_net muduo_base pthread)