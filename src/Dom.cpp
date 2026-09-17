#include "llvm/IR/Function.h"
#include "llvm/IR/PassManager.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/CFG.h"

#include <cstddef>
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
        std::map<const llvm::BasicBlock*, std::size_t> dfsNum;
        std::vector<const llvm::BasicBlock*> dfsOrder; 
        std::vector<std::size_t> label; 
        std::vector<std::size_t> sdom; 
        std::vector<std::size_t> disjoint_set_union; 
        std::vector<std::size_t> dfs_parent;
        std::vector<std::size_t> reverse_graph;
        std::vector<std::vector<const llvm::BasicBlock*>> bucket;
        std::vector<std::size_t> dom;

        // VI g[N],tree[N],rg[N],bucket[N];
        // int sdom[N],par[N],dom[N],dsu[N],label[N];
        // int arr[N],rev[N],T;

        for(int i = n; i >= 1; i--)
        {
          const llvm::BasicBlock *BB = dfsOrder[i];

          for(auto Pred: llvm::predecessors(BB))
            sdom[i] = std::min(sdom[i], sdom[Find(disjoint_set_union, label, sdom, Pred, 0)]);

          if(i > 1)
            bucket[sdom[i]].push_back(BB);

          for(auto bucket_BB: bucket[i])
          {
            auto v = Find(disjoint_set_union, label, sdom, bucket_BB, 0);
            if(sdom[v]==sdom[bucket_BB]) 
              dom[bucket_BB]=sdom[bucket_BB];
            else 
              dom[bucket_BB] = v;
          }
          if(i>1)Union(par[i],i);
        }

        for(int i=2;i<=n;i++)
        {
          if(dom[i]!=sdom[i])dom[i]=dom[dom[i]];
          tree[rev[i]].PB(rev[dom[i]]);
          tree[rev[dom[i]]].PB(rev[i]);
        }

        return llvm::PreservedAnalyses::all();
    }

    void dfs0(
        const llvm::BasicBlock* u, 
        int timer, 
        std::map<const llvm::BasicBlock*, std::size_t> arrival, 
        std::vector<const llvm::BasicBlock*> reverse_arrival, 
        std::vector<std::size_t> label, 
        std::vector<std::size_t> sdom, 
        std::vector<std::size_t> disjoint_set_union, 
        std::vector<std::size_t> dfs_parent
        )
    {
      timer++;
      arrival[u]=timer;
      reverse_arrival[timer]=u;

      label[timer]=timer;
      sdom[timer]=timer;
      disjoint_set_union[timer]=timer;

      for(const llvm::BasicBlock* Succ: llvm::successors(u))
      {
        if(!arrival[Succ])
        {
          dfs0(Succ, timer, arrival, reverse_arrival, label, sdom, disjoint_set_union, dfs_parent);
          dfs_parent[arrival[Succ]]=arrival[u];
        }
      }
    }

    int Find(
        std::vector<size_t> dsu,
        std::vector<size_t> label,
        std::vector<size_t> sdom,
        const llvm::BasicBlock* u, int x=0
        )
    {
      if(u==dsu[u]) return (x)?-1 : u;
      int v = Find(dsu[u], x+1);
      if(v<0) return u;

      if(sdom[label[dsu[u]]] < sdom[label[u]])
        label[u] = label[dsu[u]];
      dsu[u] = v;
      return (x)?v:label[u];
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
