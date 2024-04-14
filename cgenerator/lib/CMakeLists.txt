cmake_minimum_required(VERSION 3.13.4)

find_package(LLVM REQUIRED CONFIG)

include_directories(${LLVM_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINITIONS})


message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

SET(CMAKE_CXX_FLAGS "-Wall -fno-rtti")
SET(CMAKE_BUILD_TYPE Debug)
#add_library(FuncCallGraph SHARED FuncCallGraph.cc)
#add_library(MPCallGraph SHARED MPCallGraph.cc)

add_executable(cgenerator Generator.cc 
						  Generator.h
						  CallGraph.cc
						  CallGraph.h 
              FunctionStackUsage.h)

#llvm_map_components_to_libnames(llvm_libs all)

# Should figure out what this path
# set(EXECUTABLE_OUTPUT_PATH ${GENERATOR_BINARY_DIR})

#target_link_libraries(FuncCallGraph ${llvm_libs})
#target_link_libraries(cgenerator ${llvm_libs})
target_link_libraries(cgenerator 
						LLVMAsmParser 
						LLVMSupport 
						LLVMCore 
						LLVMAnalysis
						LLVMIRReader)
#target_link_libraries(MPCallGraph ${llvm_libs})
