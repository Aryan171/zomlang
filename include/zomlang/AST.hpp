#pragma once

#include <string>
#include <memory>
#include <vector>
#include "llvm/IR/Value.h"
#include "llvm/IR/Constants.h"

namespace zomlang {
    class CompilerState;

    // ExprAST - Base class for all expression nodes.
    class ExprAST {
        public:
        virtual ~ExprAST() = default;
        virtual llvm::Value *codegen(CompilerState &cs) = 0;
    };
    
    // NumberExprAST - Expression class for numeric literals like "1.0".
    class NumberExprAST : public ExprAST {
        double val;

        public:
        NumberExprAST(double val) : val(val) {}
        llvm::Value *codegen(CompilerState &cs) override;
    };

    // VariableExprAST - Expression class for referencing a variable, like "a".
    class VariableExprAST : public ExprAST {
        std::string name;

        public:
        VariableExprAST(const std::string &name) : name(name) {}
        llvm::Value *codegen(CompilerState &cs) override;
    };

    // BinaryExprAST - Expression class for a binary operator.
    class BinaryExprAST : public ExprAST {
        char op;
        std::unique_ptr<ExprAST> LHS, RHS;

        public:
        BinaryExprAST(char op, std::unique_ptr<ExprAST> LHS,
                        std::unique_ptr<ExprAST> RHS)
            : op(op), LHS(std::move(LHS)), RHS(std::move(RHS)) {}
        llvm::Value *codegen(CompilerState &cs) override;
    };

    // CallExprAST - Expression class for function calls.
    class CallExprAST : public ExprAST {
        std::string callee;
        std::vector<std::unique_ptr<ExprAST>> args;

        public:
        CallExprAST(const std::string &callee,
            std::vector<std::unique_ptr<ExprAST>> args)
            : callee(callee), args(std::move(args)) {}
        llvm::Value *codegen(CompilerState &cs) override;
    };

    // PrototypeAST - This class represents the "prototype" for a function,
    // which captures its name, and its argument names (thus implicitly the number
    // of arguments the function takes).
    class PrototypeAST {
        std::string name;
        std::vector<std::string> args;

        public:
        PrototypeAST(const std::string &name, std::vector<std::string> args)
            : name(name), args(std::move(args)) {}

        const std::string &getName() const { return name; }
        llvm::Function *codegen(CompilerState &cs);
    };

    /// FunctionAST - This class represents a function definition itself.
    class FunctionAST {
        std::unique_ptr<PrototypeAST> Proto;
        std::unique_ptr<ExprAST> Body;

        public:
        FunctionAST(std::unique_ptr<PrototypeAST> Proto,
                    std::unique_ptr<ExprAST> Body)
            : Proto(std::move(Proto)), Body(std::move(Body)) {}
        llvm::Function *codegen(CompilerState &cs);
    };

    
    std::unique_ptr<ExprAST> LogError(const char *Str);
    
    std::unique_ptr<PrototypeAST> LogErrorP(const char *Str);

    llvm::Value *LogErrorV(const char *Str);
}