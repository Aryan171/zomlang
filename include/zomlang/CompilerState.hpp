#pragma once

#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Value.h"
#include "llvm/IR/PassManager.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/StandardInstrumentations.h"
#include "llvm/IR/Function.h"
#include "zomlang/ZomlangJIT.hpp"
#include "zomlang/AST.hpp"
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
        std::map<std::string, std::unique_ptr<PrototypeAST>> FunctionProtos;
        
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
        std::map<std::string, std::unique_ptr<PrototypeAST>> &getFunctionProtos() { return FunctionProtos; }
        llvm::Function *getFunction(std::string name);

        llvm::FunctionPassManager& getFPM() { return *TheFPM; }
        llvm::LoopAnalysisManager& getLAM() { return *TheLAM; }
        llvm::FunctionAnalysisManager& getFAM() { return *TheFAM; }
        llvm::CGSCCAnalysisManager& getCGAM() { return *TheCGAM; }
        llvm::ModuleAnalysisManager& getMAM() { return *TheMAM; }
        llvm::PassInstrumentationCallbacks& getPIC() { return *ThePIC; }
        llvm::StandardInstrumentations& getSI() { return *TheSI; }

        std::unique_ptr<llvm::Module> takeModule() { return std::move(TheModule); }
        std::unique_ptr<llvm::LLVMContext> takeContext() { return std::move(TheContext); }
    };
}