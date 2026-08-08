#include "zomlang/CompilerState.hpp"
    namespace zomlang {
    CompilerState::CompilerState() {
        InitializeModuleAndManagers();
    }

    void CompilerState::InitializeModuleAndManagers() {
        TheContext = std::make_unique<llvm::LLVMContext>();
        TheModule = std::make_unique<llvm::Module>("zomlang", *TheContext);
        Builder = std::make_unique<llvm::IRBuilder<>>(*TheContext);
    }
}