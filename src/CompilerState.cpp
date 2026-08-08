#include "zomlang/CompilerState.hpp"
#include "zomlang/ZomlangJIT.hpp"
#include "llvm/IR/PassManager.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/StandardInstrumentations.h"
#include "llvm/Support/TargetSelect.h"

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
    }
}