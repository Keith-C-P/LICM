#include "llvm/IR/Function.h"
#include "llvm/IR/PassManager.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/CFG.h"

#include <cstddef>
#include <vector>
#include <set>

// using namespace llvm;

namespace {

  struct MyDominator : llvm::PassInfoMixin<MyDominator> {
    llvm::PreservedAnalyses run(
        llvm::Function &F,
        llvm::FunctionAnalysisManager &
        ) {

      auto ordering = get_dfs_ordering(F);
      
      int i = 0;
      for (auto BB : ordering){
        llvm::errs() << i << ": " << BB->getName() << ", ";
        i++;
      }

      return llvm::PreservedAnalyses::all();
    }

    std::vector<const llvm::BasicBlock*> get_dfs_ordering(const llvm::Function &F) {
      std::size_t no_of_BB = F.size();
      std::set<const llvm::BasicBlock*> visited;
      std::vector<const llvm::BasicBlock*> ordering;  
      ordering.reserve(no_of_BB);

      dfsRec(visited, F.getEntryBlock(), ordering);

      return (ordering);
    }

    void dfsRec(std::set<const llvm::BasicBlock*> &visited, const llvm::BasicBlock &BB, std::vector<const llvm::BasicBlock*> &ordering) {
      llvm::errs() << "Visiting: " << BB.getName() << "\n";
      
      visited.insert(&BB);

      ordering.push_back(&BB);

      // Recursively visit all adjacent vertices
      // that are not visited yet
      for (const llvm::BasicBlock *Succ: llvm::successors(&BB))
        if (visited.find(Succ) == visited.end())
          dfsRec(visited, *Succ, ordering);
    }
  };

} // namespace

extern "C" LLVM_ATTRIBUTE_WEAK
llvm::PassPluginLibraryInfo llvmGetPassPluginInfo() {
  return {
    LLVM_PLUGIN_API_VERSION,
    "HelloPass",
    LLVM_VERSION_STRING,
    [](llvm::PassBuilder &PB) {
      PB.registerPipelineParsingCallback(
          [](llvm::StringRef Name,
            llvm::FunctionPassManager &FPM,
            llvm::ArrayRef<llvm::PassBuilder::PipelineElement>) {

          if (Name == "Dom") {
          FPM.addPass(MyDominator());
          return true;
          }

          return false;
          }
          );
    }
  };
}
