#include "llvm/IR/Function.h"
#include "llvm/Pass.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/CFG.h" 
#include "llvm/IR/Instructions.h" 

using namespace llvm;

namespace {
    class CallGraph : public FunctionPass {
		public:
		    static char ID;
    	    CallGraph() : FunctionPass(ID) {}

    	    virtual bool runOnFunction(Function &F) {
    	        errs() << "Function: " << F.getName() << "\n";
    	        for (auto &B : F) {
				   B.printAsOperand(errs(), false);
				   errs() << "\n";
    	           // errs() << "BasicBlock: " << B.getName() << ", Predecessors: ";
    	           // for (auto *Pred : predecessors(&B)) {
    	           //     errs() << Pred->getName() << " ";
    	           // }
    	           // errs() << ", Successors: ";
    	           // for (auto *Succ : successors(&B)) {
    	           //     errs() << Succ->getName() << " ";
    	           // }
    	           // errs() << "\n";
    	        }
    	        return false;
			}
		private:
	};


    char CallGraph::ID = 0;
    static RegisterPass<CallGraph> X("cg", "Control Flow Graph Pass", false, false);
}

