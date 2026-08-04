#pragma once

#include <string>
#include <memory>
#include <vector>

namespace zomlang {
    // ExprAST - Base class for all expression nodes.
    class ExprAST {
        public:
        virtual ~ExprAST() = default;
    };
    
    // NumberExprAST - Expression class for numeric literals like "1.0".
    class NumberExprAST : public ExprAST {
        double val;

        public:
        NumberExprAST(double val) : val(val) {}
    };

    // VariableExprAST - Expression class for referencing a variable, like "a".
    class VariableExprAST : public ExprAST {
        std::string name;

        public:
        VariableExprAST(const std::string &name) : name(name) {}
    };

    // BinaryExprAST - Expression class for a binary operator.
    class BinaryExprAST : public ExprAST {
        char op;
        std::unique_ptr<ExprAST> LHS, RHS;

        public:
        BinaryExprAST(char op, std::unique_ptr<ExprAST> LHS,
                        std::unique_ptr<ExprAST> RHS)
            : op(op), LHS(std::move(LHS)), RHS(std::move(RHS)) {}
    };

    // CallExprAST - Expression class for function calls.
    class CallExprAST : public ExprAST {
        std::string callee;
        std::vector<std::unique_ptr<ExprAST>> args;

        public:
        CallExprAST(const std::string &callee,
            std::vector<std::unique_ptr<ExprAST>> args)
            : callee(callee), args(std::move(args)) {}
    };

    // PrototypeAST - This class represents the "prototype" for a function,
    // which captures its name, and its argument names (thus implicitly the number
    // of arguments the function takes).
    class PrototypeAST {
        std::string Name;
        std::vector<std::string> Args;

        public:
        PrototypeAST(const std::string &Name, std::vector<std::string> Args)
            : Name(Name), Args(std::move(Args)) {}

        const std::string &getName() const { return Name; }
    };

    /// FunctionAST - This class represents a function definition itself.
    class FunctionAST {
        std::unique_ptr<PrototypeAST> Proto;
        std::unique_ptr<ExprAST> Body;

        public:
        FunctionAST(std::unique_ptr<PrototypeAST> Proto,
                    std::unique_ptr<ExprAST> Body)
            : Proto(std::move(Proto)), Body(std::move(Body)) {}
    };

}