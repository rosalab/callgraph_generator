# callgraph_generator

Dependencies:

for this project, I am using latest version of
LLVM (15)


#### LLVM build process

`cd llvm-project && mkdir build`


`cmake -DLLVM_TARGET_ARCH="X86" \
			-DLLVM_TARGETS_TO_BUILD="ARM;X86;AArch64" \
			-DLLVM_EXPERIMENTAL_TARGETS_TO_BUILD=WebAssembly \
			-DCMAKE_BUILD_TYPE=Release \
			-DLLVM_ENABLE_PROJECTS="clang;lldb;lld" \
			-G "Unix Makefiles" \
			../llvm`

`make`

`mkdir  ../prefix`

`cmake -DCMAKE_INSTALL_PREFIX=../prefix -P cmake_install.cmake`

#### Generating Call Graph from bitcode file generated

- created the CMake and MakeFile by using
  [TyPM](https://github.com/sidchintamaneni/typm/tree/main)
  repo as reference
- Checkout the MakeFile to see how to run the callgraph
  - Once I resolve all the issue I will document that as well.

#### Documenting the decisions along the way

- chose ThinLTO
  - should check the effects of NO-LTO and FULL LTO
- Currently doing static analysis on defconfig
  - Roop go insane stack depth when did static analysis with
    allyesconfig without indirectcall and loops
  - the stack depth greatly depends on the config file that the
    kernel uses
 


 
