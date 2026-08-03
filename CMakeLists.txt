cmake_minimum_required(VERSION 3.20.0)
project(zomlang VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(zstd REQUIRED)

find_package(LLVM REQUIRED CONFIG)

add_definitions(${LLVM_DEFINITIONS})
include_directories(${LLVM_INCLUDE_DIRS})

add_executable(zomlang main.cpp)

llvm_map_components_to_libnames(llvm_libs core support native)
target_link_libraries(zomlang PRIVATE ${llvm_libs})
