#ifndef GENERATOR_H

#define GENERATOR_H

#include <llvm/IR/DebugInfo.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Instructions.h>
#include <llvm/ADT/DenseMap.h>
#include <llvm/ADT/SmallPtrSet.h>
#include <llvm/ADT/StringExtras.h>
#include <llvm/Support/Path.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/Analysis/AliasAnalysis.h>
#include "llvm/Support/CommandLine.h"
#include <map>
#include <unordered_map>
#include <set>
#include <unordered_set>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>


// what is the idea?
// should store map<functionName, is_resolved> in a map
// ^^^^ this will help in writing the algorithm
// don't create callgraph for the function unless all the
// caller functions are resolved
// should store map<functionName, filePath> in a map
// ^^^^ this will help to distinguish between two functions
// if there are more than one with same name


typedef std::vector<std::pair<llvm::Module *, llvm:: StringRef>> ModuleList;
typedef std::unordered_map<llvm::Module *, llvm:: StringRef> ModuleNameMap;

// Currently using string to uniquely identify a function
// what if multiple files contain same function name
// decided to use function pointers
typedef std::vector<llvm::Function *> FunctionList;
struct directCallStruct {
	llvm::Function *F;
	bool visited;
	llvm::StringRef funcPath;
};

typedef directCallStruct directCallInfo;
typedef std::unordered_map<llvm::Function *, 
		std::vector<directCallInfo>> CallerCalleeMap;
typedef std::unordered_map<llvm::Function *, 
		std::vector<std::string>> IndirCallerCalleeMap;
typedef std::unordered_map<llvm::Function *, llvm:: StringRef> FunctionFileMap;

struct GlobalContext {
	GlobalContext() {}

	ModuleList Modules;
	ModuleNameMap ModuleMaps;
	
	FunctionList fList;
	CallerCalleeMap ccMap;
	IndirCallerCalleeMap indirCCMap;
	FunctionFileMap ffMap;
};


class CallGraphResolver {
	
	protected:
		GlobalContext *Ctx;
	public:
		CallGraphResolver(GlobalContext *Ctx_):
			Ctx(Ctx_) {}

};

#endif
