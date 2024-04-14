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
#include "CallGraph.h"
#include "FunctionStackUsage.h"
#include <map> 
#include <set>
#include <vector> 
#include <cstring>
#include <fstream>
#include <cstring>

std::string FilePath 
			= "/home/sidchintamaneni/helpers_cfg/callgraph_generator/indirectcall-analysis/target_bpf_indir.log";

std::string FilePathCallGraphs
			= "/home/sidchintamaneni/helpers_cfg/callgraph_generator/generated_callgraphs/helper_callgraph_without_indirectcalls/";
using namespace llvm;

std::set<std:: string>BlackListedFunctionStr = {};
  //{"refcount_warn_saturate", "migrate_enable", "migrate_disable", "_printk", "dump_stack","__alloc_pages","___slab_alloc", "__warn_printk"};

std::unordered_map<Function *, int> CacheStackDepth;
std::ofstream helper_stream;

static std::unordered_map<Function *, bool> isVisited;
int CustomCallGraph::recursiveResolver(Function *F, int depth
									,StringRef funcPath, int stackDepth){

		std::string sdepth = std::string(depth, '\t');
		helper_stream << sdepth << std::string(F->getName())<<" (depth:"<< depth <<")"
			<<" File Path: "<<std::string(funcPath) << " Stack Depth: "<< stackDepth<<"\n";

    int maxStackDepth = std::max(stackDepth, 0);
		if(find(Ctx->fList.begin(), Ctx->fList.end(),
					F) == Ctx->fList.end()){
			helper_stream << " Function Not Found \n";
		}
		else {
			helper_stream<<"\n";
		}
    int funcStackDepth = 0;
		for(auto &iter : Ctx->ccMap[F]){
			if(iter.callType == INDIRECT_CALL){
			 	helper_stream<< sdepth<<"*(indirect-calls): "<<
			 		std::string(iter.funcPath)<<
			 		" (depth:"<< depth <<")" <<" Targets for indirect calls are not found in TyPM Analysis"<<"\n";
			 	continue;
			}
      if(BlackListedFunctionStr.find(std::string(iter.F->getName())) != BlackListedFunctionStr.end()){
        // we are inside a dark alley
        sdepth = std::string(depth+1, '\t');
				helper_stream << sdepth<< std::string(iter.F->getName()) <<" "<< "Found a BlackListed Function\n";
        continue;
      }
      if(CacheStackDepth.find(iter.F) != CacheStackDepth.end()){
        sdepth = std::string(depth+1, '\t');
        helper_stream << sdepth << std::string(iter.F->getName()) << " " << 
        "Stack Depth of this function is already explored: "<< CacheStackDepth[iter.F]<< "\n";
        funcStackDepth = std::max(CacheStackDepth[iter.F], funcStackDepth);
        continue;
      }
			if(isVisited[iter.F] == false){
				isVisited[iter.F] = true;
				int localStackDepth = CustomCallGraph::
					recursiveResolver(iter.F, depth+1, iter.funcPath, iter.stackDepth);
        funcStackDepth = std::max(localStackDepth, funcStackDepth);
        CacheStackDepth[iter.F] = localStackDepth;
				isVisited[iter.F] = false;
			} else {
        sdepth = std::string(depth+1, '\t');
				helper_stream << sdepth<< std::string(iter.F->getName()) <<" "<< "WARNING: Found a loop\n";
			}
		}
  maxStackDepth += funcStackDepth;
  return maxStackDepth;
}

void CustomCallGraph::callGraphResolver(std::string helper_function){
	// what should be the algorithm now?
  std::string helper_cg_filepath = FilePathCallGraphs + helper_function;
  helper_stream.open(helper_cg_filepath);
	helper_stream << "-----------------------------------------\n";	
	helper_stream<< "Generating Call Graph for Helper Function: "
		<< helper_function << "\n";
	Function *tmp = NULL;
	for(Function *F: Ctx->fList){
		if(F->getName() == helper_function) {
			tmp = F;
		}
	}
  int stackDepth = -1;
  if(stack_usage_data.find(helper_function) != stack_usage_data.end()){
    stackDepth = stack_usage_data[helper_function];
  }
  int maxStackDepth = 0;
	if(tmp != NULL){
		maxStackDepth = recursiveResolver(tmp, 0, Ctx->ffMap[tmp], stackDepth);
	}

  helper_stream<< "Maximum Stack Depth Used by the helper "<< helper_function <<" is: " <<maxStackDepth<<"\n";
	helper_stream<< "-----------------------------------------\n";	
  helper_stream.close();

}

void CustomCallGraph::indirectCallMapGen() {

	std::ifstream ifs;

	ifs.open(FilePath, std::ifstream::in);

	if(!ifs){
		errs() << "failed to open TyPM file: " << FilePath << "\n";
	}
	
	std::string read_line;
	
	while(ifs.good()){
    // reading a line from the target_bpf_indir file
    // supposed to be [CALLER]
		getline(ifs, read_line);
    // that was being checked here
		if(strstr(read_line.c_str(), "[CALLER]")){
			char *dup = strdup(read_line.c_str());
			char *pch_caller;
			char *pch = strtok(dup, " ");
			//splitting Caller
			while(pch !=NULL){
				pch_caller = pch;
				pch =  strtok(NULL, " ");
			}
 //     errs()<< "[CALLER] " << pch_caller << "\n";
      // Now pch_caller variable contains indirect call callsite
      // reading the count of indirectcalls targets
			getline(ifs, read_line);
			dup = strdup(read_line.c_str());
			pch = strtok(dup, ":");
			int i = 0;
			//splitting number of indirectcalls
			//seems redundent to me
			while(pch !=NULL){
				// Insert in a Map
				if(i == 1){
					i = std::stoi(pch);
					break;
				}
				pch = strtok(NULL, ":");
				i++;
			}
//      errs() << "Indirect-calls targets: " << i << "\n";
      // such a garbage way to extract the number of targets
      // for each indirect call
			// Looping through the targerts
			for(int j = 0; j < i; j++){
				getline(ifs, read_line);

				if(read_line.empty()) break;
				dup = strdup(read_line.c_str());
				pch = strtok(dup, " ");
        // expecting the below line to remove the [TARGET]
        // fromt the line
				pch = strtok(NULL, " ");
				std::string filePath;
				std::string funcName;
				int k = 0;
				while(pch !=NULL){
					//cout<<pch<<endl;
					if(k==0) filePath = pch;
					if(k==1) funcName = pch;
					pch = std::strtok(NULL, " ");
					k++;
				}
//        errs() <<"[TARGETS] " << filePath
//          <<" " << funcName <<"\n";
				// resolve name and filePath to Function *
				bool matchFound = false;
        // ccMap contains caller and calle data
				for(auto iter: Ctx->ccMap){
          // ccMap = <Function *, vector<CallInfo>>
          // asssuming that no duplicates exits with 
          // function name
					if(iter.first->getName() == funcName){
				    Ctx->indirTargets[pch_caller].push_back(iter.first);
						matchFound = true;
            break;
					} 
				}

				if(!matchFound){
					// should do a sanity check here
					// and somehow show that there is 
					// no match for this function in the
					// CallerCalleeMap
					continue;
				}
        // if match found populate the map
        // indirTargets 
			}
//      errs() <<"\n";
		}
	}

  //errs() << "REAL OUTPUT FOR DIFF CHECKER"
  //  << "\n";
  //errs() << "----------------------------\n";
  // Sanity Check to Compare and see the difference 
  // between the originial and this file
//	for(auto &iter: Ctx->indirTargets){
//		auto &pair = iter;
//		errs() << "[CALLER] " << pair.first<< "\n";
//		for(auto &iter2: pair.second){
//			errs() << "[TARGETS] " <<iter2->getName() << "\n";
//		}
//	}

	ifs.close();
}

void CustomCallGraph::indirectCallResolver() {
	// iterate over CallerCalleeMap
	for(auto &iter: Ctx->ccMap){
		//errs() << "Resolving Indirect calls for the function: "<<
		//	iter.first->getName() << "\n";
		auto &pair = iter;
		// iterate over Caller's function calls
		for(int iter2 = 0; iter2 < iter.second.size();){
			// if indirect call is found
			if (pair.second[iter2].callType == INDIRECT_CALL){
				//errs() << "Found Indirect call: "<< 
				//pair.second[iter2].funcPath << "\n";
				// check if that indirect call is found
				// in TyPM output
				if(Ctx->indirTargets.find(pair.second[iter2].funcPath.str())
						!= Ctx->indirTargets.end()){
					// if found replace indirect call from CallerCalleeMap
					// with potential targets
					//errs() << "Found targets in the TyPM output\n";
					int target_size = 
				  Ctx->indirTargets[pair.second[iter2].funcPath.str()].size();
					//errs() << "Number of targets to be inserted are: "<< target_size << "\n";
					// if there is one potential target then 
					// just replace it 
          //errs() <<"Iterating through all the values: \n";
         // for(auto &x:  Ctx->indirTargets[pair.second[iter2].funcPath.str()]){
         //   errs() << x->getName();
         //   errs() <<"\n";
         // }
         // errs() << "Replacing the value indirect-call with first target: " <<"\n";
					//pair.second[iter2] = 
					//{(Ctx->indirTargets[pair.second[iter2].funcPath.str()])[0],
					//	false, "", DIRECT_CALL
					//};
          int forIter = 0;
          for(auto &x:  Ctx->indirTargets[pair.second[iter2].funcPath.str()]){
            int stackDepth = -1;
            if(stack_usage_data.find(std::string(x->getName())) != stack_usage_data.end()){
              stackDepth = stack_usage_data[std::string(x->getName())];
            }
            if(forIter == 0){
              pair.second[forIter] = {
                x, false, "Target function for a indirect-call", DIRECT_CALL, stackDepth
              };
            } else{
              pair.second.insert(
                  pair.second.begin() + iter2 + forIter,
                {x, false, "Target funciton for indirect-call", DIRECT_CALL, stackDepth}
              );
            }
            forIter++;
          }
          if(target_size > 1){
            iter2 += target_size - 1;
          }
				} else{
					// Indirect call target not found
					// in TyPM analysis
				}
			}
			iter2++;
		}
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
		//errs() << "Function: "<< F->getName() << " " << 
		//	Ctx->ModuleMaps[M]<<" "<<
		//	F <<"\n";
		Ctx->fList.push_back(F);
    isVisited[F] = false;
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
        isVisited[CF] = false;
				if(CI->isIndirectCall()){
					std::string st;
					if (const DebugLoc &DbgLoc = i->getDebugLoc()) {
						unsigned Line = DbgLoc.getLine();
						StringRef File = DbgLoc->getFilename();
						st= File.str()+":"+
								std::to_string(Line);
					}
					if(Ctx->ccMap.find(F) != Ctx->ccMap.end()){
						Ctx->ccMap[F].push_back({CF, false,
								StringRef(strdup(st.c_str())),
								INDIRECT_CALL, 0});
					} else{
						Ctx->ccMap[F] = std::vector<CallInfo>
							 {{CF, false, StringRef(strdup(st.c_str()))
							  , INDIRECT_CALL, 0}};
					}

				}else {
					if(CF){
						std::string st;
						if (const DebugLoc &DbgLoc = i->getDebugLoc()) {
							unsigned Line = DbgLoc.getLine();
							StringRef File = DbgLoc->getFilename();
							st= File.str()+":"+
										std::to_string(Line);
						}
            int stackDepth = -1;
            if(stack_usage_data.find(std::string(CF->getName())) != stack_usage_data.end()){
              stackDepth = stack_usage_data[std::string(CF->getName())];
            }
						if(Ctx->ccMap.find(F) != Ctx->ccMap.end()){
							Ctx->ccMap[F].push_back({CF, false,
									StringRef(strdup(st.c_str())),
									DIRECT_CALL, stackDepth});
						} else{
							Ctx->ccMap[F] = std::vector<CallInfo>
								 {{CF, false, StringRef(strdup(st.c_str()))
                , DIRECT_CALL, stackDepth}};
						}
					}
				}
			}
		}
	}
}


