# callgraph_generator

Dependencies:

for this project, I am using latest version of
LLVM (18.1.1)


#### LLVM build process

`cd llvm-project && mkdir build`


`cmake -DLLVM_TARGET_ARCH="X86" \
			-DLLVM_TARGETS_TO_BUILD="ARM;X86;AArch64" \
			-DLLVM_EXPERIMENTAL_TARGETS_TO_BUILD=WebAssembly \
			-DCMAKE_BUILD_TYPE=Release \
			-DLLVM_ENABLE_PROJECTS="clang;lldb" \
			-G "Unix Makefiles" \
			../llvm`

`make -j `nproc``

`mkdir  ../prefix`

`cmake -DCMAKE_INSTALL_PREFIX=../prefix -P cmake_install.cmake`

#### Generating Call Graph from bitcode file generated


