#include "llvm/IR/Function.h"
#include "llvm/IR/PassManager.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Support/raw_ostream.h"

// using namespace llvm;

namespace {

  struct MyDominator : llvm::PassInfoMixin<MyDominator> {
    llvm::PreservedAnalyses run(
        llvm::Function &F,
        llvm::FunctionAnalysisManager &
        ) {

      llvm::errs() << "Function: "
        << F.getName() << "\n";

      for (llvm::BasicBlock &BB : F) {
        // Iterate over each Instruction in the BasicBlock
        BB.print(llvm::errs());
        for (llvm::Instruction &I : BB) {
          // Print the instruction as a line of IR
          I.print(llvm::errs());
          llvm::errs() << "\n";
        }
      }

      return llvm::PreservedAnalyses::all();
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
