#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/PassManager.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Verifier.h"
#include "llvm/Bitcode/BitcodeReader.h"
#include "llvm/Bitcode/BitcodeWriter.h"
#include "llvm/Support/ManagedStatic.h"
#include "llvm/Support/PrettyStackTrace.h"
#include "llvm/Support/ToolOutputFile.h"
#include "llvm/Support/SystemUtils.h"
#include "llvm/Support/FileSystem.h"
#include "llvm/IRReader/IRReader.h"
#include "llvm/Support/SourceMgr.h"
#include "llvm/Support/Signals.h"
#include "llvm/Support/Path.h"

#include "Generator.h"
#include "CallGraph.h"

using namespace llvm;

cl::list<std::string> InputFilenames(
		cl::Positional, cl::OneOrMore, cl::desc("<input bitcode files>"));


GlobalContext GlobalCtx;

void run(CustomCallGraph &CGPass){
	
	//doModulePass and store all the functions and the callee 
	//functions inside the cgMap, fMap
	ModuleList &modules = GlobalCtx.Modules;
	
	errs()<< "Processing "<< modules.size() << " modules.\n";
	ModuleList::iterator i, e;
	
	for(i = modules.begin(), e = modules.end(); i!=e; i++){
		CGPass.doModulePass(i->first);
	}


	errs()<< "ModulePass Successfully Completed\n";

	errs()<< "Printing Stats after ModulePass\n";
	// questions to answer
	// 1. Number of functions in the callgraph
	errs()<< "Number of functions: " << GlobalCtx.fList.size() <<"\n";

	//^^^ let's think about some more stats and fill above
	
	CGPass.callGraphResolver("bpf_spin_lock");
	CGPass.callGraphResolver("bpf_get_stackid");
	
	
}

void printResults(){
	errs()<< "printing call graph results\n";
	for(llvm::Function *F : GlobalCtx.fList){
		errs() << "Function: "<<F->getName()<<"\n";	
		for(auto iter : GlobalCtx.ccMap[F]) {
			errs() << "\t" << iter.first->getName() <<"\n";
		}

		for(auto iter : GlobalCtx.indirCCMap[F]) {
			errs() << "\t" << iter <<"\n";
		}
	}
}


int main(int argc, char** argv) {
	
	// Print a stack trace if we signal out.
	sys::PrintStackTraceOnErrorSignal(argv[0]);
	PrettyStackTraceProgram X(argc, argv);

	cl::ParseCommandLineOptions(argc, argv, "global analysis\n");
	SMDiagnostic Err;
	

	// Loading modules
	errs() << "Total " << InputFilenames.size() << " file(s)\n";
	for (unsigned i = 0; i < InputFilenames.size(); ++i) {

		LLVMContext *LLVMCtx = new LLVMContext();
		std::unique_ptr<Module> M = parseIRFile(InputFilenames[i], Err, *LLVMCtx);

		if (M == NULL) {
			errs() << argv[0] << ": error loading file '"
				<< InputFilenames[i] << "'\n";
			continue;
		}

		Module *Module = M.release();
		StringRef MName = StringRef(strdup(InputFilenames[i].data()));
		GlobalCtx.Modules.push_back(std::make_pair(Module, MName));
		GlobalCtx.ModuleMaps[Module] = InputFilenames[i];
	}

	CustomCallGraph CGPass(&GlobalCtx);

	run(CGPass);

//	printResults();

	return 0;
}
