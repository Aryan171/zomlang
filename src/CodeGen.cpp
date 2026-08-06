#include "zomlang/CodeGen.hpp"
#include "zomlang/AST.hpp"
#include "llvm/IR/Verifier.h"
#include "zomlang/Parser.hpp"

namespace zomlang {
    std::unique_ptr<llvm::LLVMContext> TheContext = std::make_unique<llvm::LLVMContext>();
    std::unique_ptr<llvm::Module> TheModule = std::make_unique<llvm::Module>("zomlang JIT", *TheContext);
    std::unique_ptr<llvm::IRBuilder<>> Builder = std::make_unique<llvm::IRBuilder<>>(*TheContext);
    std::map<std::string, llvm::Value *> NamedValues = {};


    llvm::Value *NumberExprAST::codegen() {
        return llvm::ConstantFP::get(*TheContext, llvm::APFloat(val));
    }

    llvm::Value *VariableExprAST::codegen() {
        llvm::Value *v = NamedValues[name];
        if (!v) {
            LogErrorV("Unknown variable name");
        }
        return v;
    }

    llvm::Value *BinaryExprAST::codegen() {
        llvm::Value *l = LHS->codegen(), *r = RHS->codegen();

        if (!l || !r) {
            return nullptr;
        }

        switch (op) {
            case '+':
                return Builder->CreateFAdd(l, r, "addtmp");
            case '-':
                return Builder->CreateFSub(l, r, "subtmp");
            case '*':
                return Builder->CreateFMul(l, r, "multmp");
            case '<':
                l = Builder->CreateFCmpULT(l, r, "cmptmp");
                // Convert bool 0/1 to double 0.0 or 1.0
                return Builder->CreateUIToFP(l, llvm::Type::getDoubleTy(*TheContext), "booltmp");
            default:
                return LogErrorV("invalid binary operator");
        }
    }

    llvm::Value *CallExprAST::codegen() {
         // Look up the name in the global module table.
        llvm::Function *CalleeF = TheModule->getFunction(callee);
        if (!CalleeF)
            return LogErrorV("Unknown function referenced");

        // If argument mismatch error.
        if (CalleeF->arg_size() != args.size())
            return LogErrorV("Incorrect # arguments passed");

        std::vector<llvm::Value *> ArgsV;
        for (unsigned i = 0, e = args.size(); i != e; ++i) {
            ArgsV.push_back(args[i]->codegen());
            if (!ArgsV.back())
                return nullptr;
        }

        return Builder->CreateCall(CalleeF, ArgsV, "calltmp");
    }

    llvm::Function *PrototypeAST::codegen() {
        // Make the function type:  double(double,double) etc.
        std::vector<llvm::Type *> Doubles(args.size(), llvm::Type::getDoubleTy(*TheContext));
        llvm::FunctionType *FT =
            llvm::FunctionType::get(llvm::Type::getDoubleTy(*TheContext), Doubles, false);

        llvm::Function *F =
            llvm::Function::Create(FT, llvm::Function::ExternalLinkage, name, TheModule.get());

        // Set names for all arguments.
        unsigned Idx = 0;
        for (auto &arg : F->args())
            arg.setName(args[Idx++]);

        return F;
    }

    llvm::Function *FunctionAST::codegen() {
        llvm::Function *TheFunction = TheModule->getFunction(Proto->getName());

        if (!TheFunction) {
            TheFunction = Proto->codegen();
        }

        if (!TheFunction) {
            return nullptr;
        }

        if (!TheFunction->empty()) {
            return (llvm::Function *)LogErrorV("Function cannot be redefined.");
        }

        llvm::BasicBlock *BB = llvm::BasicBlock::Create(*TheContext, "entry", TheFunction);
        Builder->SetInsertPoint(BB);

        NamedValues.clear();
        for (auto &Arg : TheFunction->args())
            NamedValues[std::string(Arg.getName())] = &Arg;

        if (llvm::Value *RetVal = Body->codegen()) {
            // Finish off the function.
            Builder->CreateRet(RetVal);

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