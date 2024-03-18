cmake_minimum_required(VERSION 3.13.4)

find_package(LLVM REQUIRED CONFIG)

include_directories(${LLVM_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINITIONS})


message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

SET(CMAKE_CXX_FLAGS "-Wall -fno-rtti")

add_library(CallGraph SHARED CallGraph.cc)


llvm_map_components_to_libnames(llvm_libs all)

target_link_libraries(CallGraph ${llvm_libs})
