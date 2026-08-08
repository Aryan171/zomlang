#pragma once

#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Value.h"
#include <map>
#include <memory>

namespace zomlang {

    class CompilerState {
    private:
        std::unique_ptr<llvm::LLVMContext> TheContext;
        std::unique_ptr<llvm::Module> TheModule;
        std::unique_ptr<llvm::IRBuilder<>> Builder;
        std::map<std::string, llvm::Value *> NamedValues;

    public:
        CompilerState();
        
        void InitializeModuleAndManagers();
        
        llvm::LLVMContext& getContext() { return *TheContext; }
        llvm::IRBuilder<>& getBuilder() { return *Builder; }
        llvm::Module& getModule() { return *TheModule; }
        std::map<std::string, llvm::Value *>& getNamedValues() { return NamedValues; }
    };

}