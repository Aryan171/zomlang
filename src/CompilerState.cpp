#include "zomlang/CompilerState.hpp"
#include "zomlang/ZomlangJIT.hpp"
#include "llvm/IR/PassManager.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/StandardInstrumentations.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm/Transforms/InstCombine/InstCombine.h"
#include "llvm/Transforms/Scalar/Reassociate.h"
#include "llvm/Transforms/Scalar/GVN.h"
#include "llvm/Transforms/Scalar/SimplifyCFG.h"


namespace zomlang {
    CompilerState::CompilerState() {
        llvm::InitializeNativeTarget();
        llvm::InitializeNativeTargetAsmPrinter();
        llvm::InitializeNativeTargetAsmParser();
        InitializeJIT();
        InitializeModuleAndManagers();
    }

    void CompilerState::InitializeJIT() {
        auto JITOrErr = ZomlangJIT::Create();
        if (!JITOrErr) {
            llvm::errs() << "Failed to create JIT instance\n";
            return;
        }
        TheJIT = std::move(*JITOrErr);
    }

    void CompilerState::InitializeModuleAndManagers() {
        TheContext = std::make_unique<llvm::LLVMContext>();
        TheModule = std::make_unique<llvm::Module>("zomlangJIT", *TheContext);
        Builder = std::make_unique<llvm::IRBuilder<>>(*TheContext);

        TheModule->setDataLayout(TheJIT->getDataLayout());

        TheFPM = std::make_unique<llvm::FunctionPassManager>();
        TheLAM = std::make_unique<llvm::LoopAnalysisManager>();
        TheFAM = std::make_unique<llvm::FunctionAnalysisManager>();
        TheCGAM = std::make_unique<llvm::CGSCCAnalysisManager>();
        TheMAM = std::make_unique<llvm::ModuleAnalysisManager>();
        ThePIC = std::make_unique<llvm::PassInstrumentationCallbacks>();
        TheSI = std::make_unique<llvm::StandardInstrumentations>(*TheContext, true);

        TheSI->registerCallbacks(*ThePIC, TheMAM.get());
        
        // Do simple "peephole" optimizations and bit-twiddling optimizations
        TheFPM->addPass(llvm::InstCombinePass());
        // Reassociate expressions.
        TheFPM->addPass(llvm::ReassociatePass());
        // Eliminate Common SubExpressions.
        TheFPM->addPass(llvm::GVNPass());
        // Simplify the control flow graph (deleting unreachable blocks, etc).
        TheFPM->addPass(llvm::SimplifyCFGPass());

          // Register analysis passes used in these transform passes.
        llvm::PassBuilder PB;
        PB.registerModuleAnalyses(*TheMAM);
        PB.registerFunctionAnalyses(*TheFAM);
        PB.crossRegisterProxies(*TheLAM, *TheFAM, *TheCGAM, *TheMAM);
    }
}