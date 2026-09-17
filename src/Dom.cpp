#include "llvm/IR/Function.h"
#include "llvm/IR/PassManager.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/CFG.h"

#include <cstddef>
#include <llvm/ADT/DenseMap.h>
#include <map>
#include <vector>
#include <set>
#include <algorithm>

namespace {

  struct MyDominator : llvm::PassInfoMixin<MyDominator> {
    /* 
     * Big Thanks to Tanuj Khattar and his article Dominator Tree Of A Directed Graph
     * tanujkhattar.wordpress.com/2016/01/11/dominator-tree-of-a-directed-graph/
     */
    llvm::PreservedAnalyses run(
        llvm::Function &F,
        llvm::FunctionAnalysisManager &
        ) {

        int timer = 0; 
        llvm::DenseMap<const llvm::BasicBlock*, std::size_t> dfsNum;
        std::vector<const llvm::BasicBlock*> dfsOrder; 
        std::vector<std::size_t> label; 
        std::vector<std::size_t> sdom; 
        std::vector<std::size_t> disjoint_set_union; 
        std::vector<std::size_t> dfs_parent;
        std::vector<std::size_t> reverse_graph;
        std::vector<std::size_t> dom;
        std::vector<std::size_t> par;
        std::vector<std::vector<int>> tree;
        std::vector<std::vector<int>> bucket;

        // VI g[N],tree[N],rg[N],bucket[N];
        // int sdom[N],par[N],dom[N],dsu[N],label[N];
        // int arr[N],rev[N],T;

        for(int i = n; i >= 1; i--)
        {
          const llvm::BasicBlock *BB = dfsOrder[i];

          for(const llvm::BasicBlock* Pred: llvm::predecessors(BB))
            sdom[i] = std::min(sdom[i], sdom[Find(dfsNum, dfsOrder, disjoint_set_union, label, sdom, Pred, 0)]);

          if(i > 1)
            bucket[sdom[i]].push_back(dfsNum[BB]);

          for(auto bucket_BB: bucket[i])
          {
            auto v = Find(dfsNum, dfsOrder, disjoint_set_union, label, sdom, dfsOrder[bucket_BB], 0);
            if(sdom[dfsNum[v]] == sdom[bucket_BB]) 
              dom[bucket_BB]=sdom[bucket_BB];
            else 
              dom[bucket_BB] = dfsNum[v];
          }
          if(i > 1)
            Union(disjoint_set_union, par[i] ,i);
        }

        for(int i = 2; i <= n; i++)
        {
          if(dom[i] != sdom[i])dom[i] = dom[dom[i]];
          tree[dfsNum[dfsOrder[i]]].push_back(dfsNum[dfsOrder[dom[i]]]);
          tree[dfsNum[dfsOrder[dom[i]]]].push_back(dfsNum[dfsOrder[i]]);
        }

        return llvm::PreservedAnalyses::all();
    }

    void dfs0(
        const llvm::BasicBlock* u, 
        int timer, 
        llvm::DenseMap<const llvm::BasicBlock*, std::size_t> dfsNum, 
        std::vector<const llvm::BasicBlock*> dfsOrder, 
        std::vector<std::size_t> label, 
        std::vector<std::size_t> sdom, 
        std::vector<std::size_t> disjoint_set_union, 
        std::vector<std::size_t> dfs_parent
        )
    {
      timer++;
      dfsNum[u]=timer;
      dfsOrder[timer]=u;

      label[timer]=timer;
      sdom[timer]=timer;
      disjoint_set_union[timer]=timer;

      for(const llvm::BasicBlock* Succ: llvm::successors(u))
      {
        if(!dfsNum[Succ])
        {
          dfs0(Succ, timer, dfsNum, dfsOrder, label, sdom, disjoint_set_union, dfs_parent);
          dfs_parent[dfsNum[Succ]]=dfsNum[u];
        }
      }
    }

    const llvm::BasicBlock* Find(
        llvm::DenseMap<const llvm::BasicBlock*, std::size_t> dfsNum,
        std::vector<const llvm::BasicBlock*> dfsOrder, 
        std::vector<size_t> dsu,
        std::vector<size_t> label,
        std::vector<size_t> sdom,
        const llvm::BasicBlock* u, int x=0
        )
    {
      auto dfsNum_u = dfsNum[u];
      if(dfsNum_u==dsu[dfsNum_u]) return (x)? NULL : u;
      const llvm::BasicBlock* v = Find(dfsNum, dfsOrder, dsu, label, sdom, dfsOrder[dfsNum_u], x+1);
      if(v==NULL) return u;

      if(sdom[label[dsu[dfsNum_u]]] < sdom[label[dfsNum_u]])
        label[dfsNum_u] = label[dsu[dfsNum_u]];
      dsu[dfsNum_u] = dfsNum[v];
      return (x ? v: dfsOrder[label[dfsNum_u]]);
    }

    void Union(std::vector<size_t> dsu, int u, int v){ //Add an edge u-->v
      dsu[v]=u;   
    }
}; // namespace

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
