#include "llvm/Analysis/CallGraph.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/Pass.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/InitializePasses.h"
#include "llvm/Transforms/IPO/PassManagerBuilder.h"

using namespace llvm;

class MPCallGraph : public ModulePass {
	public:
	
		static char ID;
		MPCallGraph() : ModulePass(ID) {}

 	    virtual bool runOnModule(Module &M) override{
 	      auto &CG = getAnalysis<CallGraphWrapperPass>().getCallGraph();

 	      errs() << "Call Graph:\n";
 	      for (auto &CGNodePair : CG) {
 	        CallGraphNode *CGNode = CGNodePair.second.get();
 	        if (auto *F = CGNode->getFunction()) {
 	          errs() << "Function: " << F->getName() << "\n";
 	          for (auto &CallRecord : *CGNode) {
 	            if (auto *CalledFunction = CallRecord.second->getFunction()) {
 	              errs() << "  calls: " << CalledFunction->getName() << "\n";
 	            }
 	          }
 	        }
 	      }
		  return false;
		}

  void getAnalysisUsage(AnalysisUsage &AU) const override {
    AU.addRequired<CallGraphWrapperPass>();
  }
};

char MPCallGraph::ID = 0;

static RegisterPass<MPCallGraph> X("mp-callgraph", "Print Call Graph Pass");

