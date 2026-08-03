#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <iostream>

int main() {
    llvm::LLVMContext context;
    llvm::Module module("zomlang_module", context);
    
    std::cout << "Success! LLVM 21 is fully linked in native WSL." << std::endl;
    module.print(llvm::outs(), nullptr);
    return 0;
}
