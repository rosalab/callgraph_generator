#include "llvm/Pass.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/BasicBlock.h" 
#include "llvm/IR/Instruction.h"
#include "llvm/IR/Instructions.h"
#include "llvm/Support/Debug.h"
#include "llvm/IR/DebugInfo.h"
#include "llvm/IR/InstIterator.h"
#include "llvm/IR/Constants.h"
#include "llvm/ADT/StringExtras.h"
#include "llvm/Analysis/CallGraph.h"
#include "llvm/Support/raw_ostream.h"  
#include "llvm/IR/InstrTypes.h" 
#include "llvm/Analysis/LoopInfo.h"
#include "llvm/Analysis/LoopPass.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/Transforms/Utils/BasicBlockUtils.h" 
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/CFG.h" 
#include "llvm/IR/Instructions.h" 
#include "llvm/IR/InstIterator.h"
#include "llvm/IR/IntrinsicInst.h"
#include <map> 
#include <vector> 

#include "CallGraph.h"

using namespace llvm;

void CustomCallGraph::recursiveResolver(Function *F, int depth){
	errs() << "Function: "<< F->getName()<<", depth: "<< depth <<"\n";
		for(auto iter : Ctx->ccMap[F]){
			if(iter.second == false){
				iter.second = true;
				CustomCallGraph::
					recursiveResolver(iter.first, depth++);
				iter.second = false;
			}	
		}

		for(auto iter : Ctx->indirCCMap[F]){
			errs()<<iter<<"\n";
		}
}

void CustomCallGraph::callGraphResolver(std::string helper_function){
	// what should be the algorithm now?
	
	errs()<< "Generating Call Graph for helper "
		<< helper_function << "\n";
	Function *tmp = NULL;
	for(Function *F: Ctx->fList){
		if(F->getName() == helper_function) {
			tmp = F;
		}
	}

	if(tmp != NULL){
		recursiveResolver(tmp, 0);
	}

}

void CustomCallGraph::doModulePass(Module *M) {
		
	// process functions
	for(Module::iterator f = M->begin(), fe = M->end();
			f != fe; f++){
		Function *F = &*f;
		if(F->isDeclaration() || F->isIntrinsic())
			continue;
		//errs() << "Function: "<<F->getName()<<"\n";	
		// adding functions to the FunctionList
		Ctx->fList.push_back(F);
		Ctx->ffMap[F] = Ctx->ModuleMaps[M];
		for(inst_iterator i = inst_begin(F), e = inst_end(F);
				i != e; i++){
			if(CallInst *CI = dyn_cast<CallInst>(&*i)){
				Value *CV = CI->getCalledOperand();
				Function *CF = dyn_cast<Function>(CV);
				if(!CF || CF-> isIntrinsic()){
					auto intrinsicID = CI->getIntrinsicID();
					if (intrinsicID !=0) {
						continue;
					}
				}
				if(CI->isIndirectCall()){
					//errs() << "Found Indirect Call: "<<CI->getIntrinsicID()
					//	<<"\n";
					if (const DebugLoc &DbgLoc = i->getDebugLoc()) {
						unsigned Line = DbgLoc.getLine();
						StringRef File = DbgLoc->getFilename();
						if(Ctx->ccMap.find(F)!= Ctx->ccMap.end()){
							Ctx->indirCCMap[F].push_back(
								(File.str() + ":" + std::to_string(Line)));
						} else{
							Ctx->indirCCMap[F] = std::vector<std::string>
							{File.str() + ":" + std::to_string(Line)};
						}
					//	errs() << "\t" << " (*" << 
					//	Ctx->ModuleMaps.find(M)->second.str() << 
					//	") at " << File << ":" << Line << "\n";
					}
					if(CF){
					//	errs() << "\t" << *CF << " (*"
					//		<< Ctx->ModuleMaps.find(M)->second.str()<<")\n";
					}
				} else {
					if(CF){
						if(Ctx->ccMap.find(F) != Ctx->ccMap.end()){
							Ctx->ccMap[F].push_back({CF, false});
						} else{
							Ctx->ccMap[F] = std::vector<std::pair<
								llvm::Function *, bool>> {{CF, false}};
						}
					//	errs() << "\t" << CF->getName() << " ("
					//		<< Ctx->ModuleMaps.find(M)->second.str()<<")\n";
					}
				}
			}
		}
	}

}
