#include "llvm/IR/Function.h"
#include "llvm/Pass.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/CFG.h" 
#include "llvm/IR/Instructions.h" 
#include "llvm/IR/InstIterator.h"
#include "llvm/IR/IntrinsicInst.h"

using namespace llvm;

namespace {
    class FuncCallGraph : public FunctionPass {
		public:
		    static char ID;
    	    FuncCallGraph() : FunctionPass(ID) {}

    	    virtual bool runOnFunction(Function &F) {

    	        errs() << "Function: " << F.getName() << "\n";

				for(inst_iterator i = inst_begin(F), e = inst_end(F); i!=e; i++){
					// removing the instruction with debug statements
					if (isa<DbgInfoIntrinsic>(&*i) || isa<IntrinsicInst>(&*i)) {
						continue;
					}

					if(CallInst *CI = dyn_cast<CallInst>(&*i)){

						// getCalledOperands returns the functions
						// being called both direct and indirect
						Value *CV = CI->getCalledOperand();
						Function *CF = dyn_cast<Function>(CV);
						
						if(CI->isIndirectCall()){
							if(CF) {
								errs()<<"\t"<<*CF<<"\n";	
							}
						} else{
							// Check if the function is inline ASM
							if(CF){
								errs()<<"\t"<<CF->getName()<<"\n";
							}
						}
					}

				}

    	        return false;
			}
		private:
	};


    char FuncCallGraph::ID = 0;
    static RegisterPass<FuncCallGraph> X("cg", "Control Flow Graph Pass", false, false);
}

