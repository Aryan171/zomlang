#pragma once

#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Value.h"
#include "llvm/IR/PassManager.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/StandardInstrumentations.h"
#include "zomlang/ZomlangJIT.hpp"
#include <map>
#include <memory>

namespace zomlang {

    class CompilerState {
    private:
        std::unique_ptr<llvm::LLVMContext> TheContext;
        std::unique_ptr<llvm::Module> TheModule;
        std::unique_ptr<llvm::IRBuilder<>> Builder;
        std::map<std::string, llvm::Value *> NamedValues;
        std::unique_ptr<ZomlangJIT> TheJIT;
        
        std::unique_ptr<llvm::FunctionPassManager> TheFPM;
        std::unique_ptr<llvm::LoopAnalysisManager> TheLAM;
        std::unique_ptr<llvm::FunctionAnalysisManager> TheFAM;
        std::unique_ptr<llvm::CGSCCAnalysisManager> TheCGAM;
        std::unique_ptr<llvm::ModuleAnalysisManager> TheMAM;
        std::unique_ptr<llvm::PassInstrumentationCallbacks> ThePIC;
        std::unique_ptr<llvm::StandardInstrumentations> TheSI;

    public:
        CompilerState();

        void InitializeJIT();
        
        void InitializeModuleAndManagers();
        
        llvm::LLVMContext& getContext() { return *TheContext; }
        llvm::IRBuilder<>& getBuilder() { return *Builder; }
        llvm::Module& getModule() { return *TheModule; }
        std::map<std::string, llvm::Value *>& getNamedValues() { return NamedValues; }
        ZomlangJIT& getJIT() { return *TheJIT; }
    };

}