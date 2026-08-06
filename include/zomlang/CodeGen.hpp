#pragma once

#include <iostream>
#include <map>
#include <memory>
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Value.h"

namespace zomlang {
    // TheContext is an opaque object that owns a lot of core LLVM data structures, 
    // such as the type and constant value tables. We don’t need to understand it in 
    // detail, we just need a single instance to pass into APIs that require it.
    extern std::unique_ptr<llvm::LLVMContext> TheContext;
    // The Builder object is a helper object that makes it easy to generate LLVM 
    // instructions. Instances of the IRBuilder class template keep track of the 
    // current place to insert instructions and has methods to create new instructions.
    extern std::unique_ptr<llvm::IRBuilder<>> Builder;
    // TheModule is an LLVM construct that contains functions and global variables. 
    // In many ways, it is the top-level structure that the LLVM IR uses to contain code. 
    // It will own the memory for all of the IR that we generate, which is why the 
    // codegen() method returns a raw Value*, rather than a unique_ptr<Value>.
    extern std::unique_ptr<llvm::Module> TheModule;
    // The NamedValues map keeps track of which values are defined in the current
    // scope and what their LLVM representation is. (In other words, it is a symbol
    // table for the code). In this form of zomlang, the only things that can
    // be referenced are function parameters. As such, function parameters will be
    // in this map when generating code for their function body.
    extern std::map<std::string, llvm::Value *> NamedValues;
}