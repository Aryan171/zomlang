#include "zomlang/AST.hpp"
#include "llvm/IR/Verifier.h"
#include "zomlang/Parser.hpp"
#include "zomlang/CompilerState.hpp"

namespace zomlang {
    llvm::Value *NumberExprAST::codegen(CompilerState& cs) {
        return llvm::ConstantFP::get(cs.getContext(), llvm::APFloat(val));
    }

    llvm::Value *VariableExprAST::codegen(CompilerState& cs) {
        llvm::Value *v = cs.getNamedValues()[name];
        if (!v) {
            LogErrorV("Unknown variable name");
        }
        return v;
    }

    llvm::Value *BinaryExprAST::codegen(CompilerState& cs) {
        llvm::Value *l = LHS->codegen(cs), *r = RHS->codegen(cs);

        if (!l || !r) {
            return nullptr;
        }

        switch (op) {
            case '+':
                return cs.getBuilder().CreateFAdd(l, r, "addtmp");
            case '-':
                return cs.getBuilder().CreateFSub(l, r, "subtmp");
            case '*':
                return cs.getBuilder().CreateFMul(l, r, "multmp");
            case '<':
                l = cs.getBuilder().CreateFCmpULT(l, r, "cmptmp");
                // Convert bool 0/1 to double 0.0 or 1.0
                return cs.getBuilder().CreateUIToFP(l, llvm::Type::getDoubleTy(cs.getContext()), "booltmp");
            default:
                return LogErrorV("invalid binary operator");
        }
    }

    llvm::Value *CallExprAST::codegen(CompilerState& cs) {
         // Look up the name in the global module table.
        llvm::Function *CalleeF = cs.getModule().getFunction(callee);
        if (!CalleeF)
            return LogErrorV("Unknown function referenced");

        // If argument mismatch error.
        if (CalleeF->arg_size() != args.size())
            return LogErrorV("Incorrect # arguments passed");

        std::vector<llvm::Value *> ArgsV;
        for (unsigned i = 0, e = args.size(); i != e; ++i) {
            ArgsV.push_back(args[i]->codegen(cs));
            if (!ArgsV.back())
                return nullptr;
        }

        return cs.getBuilder().CreateCall(CalleeF, ArgsV, "calltmp");
    }

    llvm::Function *PrototypeAST::codegen(CompilerState& cs) {
        // Make the function type:  double(double,double) etc.
        std::vector<llvm::Type *> Doubles(args.size(), llvm::Type::getDoubleTy(cs.getContext()));
        llvm::FunctionType *FT =
            llvm::FunctionType::get(llvm::Type::getDoubleTy(cs.getContext()), Doubles, false);

        llvm::Function *F =
            llvm::Function::Create(FT, llvm::Function::ExternalLinkage, name, &cs.getModule());

        // Set names for all arguments.
        unsigned Idx = 0;
        for (auto &arg : F->args())
            arg.setName(args[Idx++]);

        return F;
    }

    llvm::Function *FunctionAST::codegen(CompilerState& cs) {
        llvm::Function *TheFunction = cs.getModule().getFunction(Proto->getName());

        if (!TheFunction) {
            TheFunction = Proto->codegen(cs);
        }

        if (!TheFunction) {
            return nullptr;
        }

        if (!TheFunction->empty()) {
            return (llvm::Function *)LogErrorV("Function cannot be redefined.");
        }

        llvm::BasicBlock *BB = llvm::BasicBlock::Create(cs.getContext(), "entry", TheFunction);
        cs.getBuilder().SetInsertPoint(BB);

        cs.getNamedValues().clear();
        for (auto &Arg : TheFunction->args())
            cs.getNamedValues()[std::string(Arg.getName())] = &Arg;

        if (llvm::Value *RetVal = Body->codegen(cs)) {
            // Finish off the function.
            cs.getBuilder().CreateRet(RetVal);

            // Validate the generated code, checking for consistency.
            llvm::verifyFunction(*TheFunction);

            return TheFunction;
        }

        // Error reading body, remove function.
        TheFunction->eraseFromParent();
        return nullptr;
    }
    
    std::unique_ptr<ExprAST> LogError(const char *Str) {
        fprintf(stderr, "Error: %s\n", Str);
        return nullptr;
    }
    
    std::unique_ptr<PrototypeAST> LogErrorP(const char *Str) {
        LogError(Str);
        return nullptr;
    }

    llvm::Value *LogErrorV(const char *Str) {
        LogError(Str);
        return nullptr;
    }
}