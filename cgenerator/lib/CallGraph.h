#ifndef CALLGRAPH_H

#define CALLGRAPH_H

#include "Generator.h"

class CustomCallGraph :
	public CallGraphResolver {
	
	private:
		int MIdx;

	public:
		CustomCallGraph(GlobalContext *Ctx) 
			: CallGraphResolver(Ctx) {
				MIdx = 0;
			}
		
		void doModulePass(llvm::Module *);
		void callGraphResolver(std::string);
		int recursiveResolver(llvm::Function *, int, llvm::StringRef, int);
		void indirectCallMapGen();
		void indirectCallResolver();
};

#endif // CALLGRAPH_H
