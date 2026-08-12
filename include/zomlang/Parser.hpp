#pragma once

#include "Lexer.hpp"
#include "AST.hpp"
#include <memory>
#include <map>

namespace zomlang {
    class Parser {
        private:
        Lexer &lexer;

        public:
        explicit Parser(Lexer &lexer);
        
        std::unique_ptr<ExprAST> parseExpression();
        std::unique_ptr<ExprAST> parseNumberExpr();
        std::unique_ptr<ExprAST> parseIdentifierExpr();
        std::unique_ptr<ExprAST> parseParenExpr();
        std::unique_ptr<ExprAST> parsePrimary();
        std::unique_ptr<ExprAST> parseBinOpRHS(int exprPrec, std::unique_ptr<ExprAST> LHS);
        std::unique_ptr<ExprAST> parseIfExpr();
        std::unique_ptr<PrototypeAST> parsePrototype();
        std::unique_ptr<FunctionAST> parseTopLevelExpr();
        std::unique_ptr<PrototypeAST> parseExtern();
        std::unique_ptr<FunctionAST> parseDefinition();

        std::string_view sourceCode;

        std::map<char, int> BinopPrecedence = {
            {'<', 10},
            {'+', 20},
            {'-', 20},
            {'*', 40}
        };
            
        int getTokPrecedence();
    };
}