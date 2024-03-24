.PHONY: all cgenerator clean
CUR_DIR = $(shell pwd)
LLVM_BUILD := ${CUR_DIR}/llvm-project/prefix
GENERATOR_DIR := ${CUR_DIR}/cgenerator
GENERATOR_BUILD := ${CUR_DIR}/build


NPROC := $(shell nproc)

build_generator_func = \
	(mkdir -p ${2} \
		&& cd ${2} \
		&& PATH=${LLVM_BUILD}/bin:${PATH} \
			LLVM_TOOLS_BINARY_DIR=${LLVM_BUILD}/bin \
			LLVM_LIBRARY_DIRS=${LLVM_BUILD}/lib \
			LLVM_INCLUDE_DIRS=${LLVM_BUILD}/include \
			CC=clang CXX=clang++ \
			cmake ${1}	\
				-DCMAKE_BUILD_TYPE=Build \
				-DLLVM_ENABLE_ASSERTIONS=ON \
		&& make -j${NPROC})


all: cgenerator 

cgenerator:
	$(call build_generator_func, ${GENERATOR_DIR}, ${GENERATOR_BUILD})

#execute-FuncCallGraph:
#	./llvm-project/prefix/bin/opt -enable-new-pm=0 -load ./build/lib/libFuncCallGraph.so -cg ../linux/kernel/bpf/helpers.bc -o out 2>&1

execute:
	build/lib/cgenerator @bc.list > output 2>&1
#execute-MPCallGraph:
#	./llvm-project/prefix/bin/opt -enable-new-pm=0 -load ./build/lib/libMPCallGraph.so -mp-callgraph ../linux/kernel/bpf/helpers.bc -o out 2>&1
	
clean:
	rm -rf ${GENERATOR_BUILD}
