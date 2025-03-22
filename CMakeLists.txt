cmake_minimum_required(VERSION 3.5)

project(rpc)

# set(CMAKE_CXX_STANDARD 17)
set(CMAKE_BUILD_TYPE "Debug")

include_directories(${PROJECT_SOURCE_DIR}/src/include)
include_directories(${PROJECT_SOURCE_DIR}/example)
include_directories(/usr/local/include/google/protobuf)
include_directories(/usr/local/include)
include_directories(/usr/local/include/zookeeper)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)

add_subdirectory(src)
add_subdirectory(example)
