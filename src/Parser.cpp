#include "zomlang/Parser.hpp"

namespace zomlang {
    Parser::Parser(Lexer &lexer) : lexer(lexer) {}

    void Parser::parse() {
        while (true) {
            fprintf(stderr, "ready> ");
            switch (lexer.curTok) {
                case tok_eof:
                return;

                case ';': // ignore top-level semicolons.
                lexer.getNextToken();
                break;

                case tok_def:
                parseDefinition();
                break;

                case tok_extern:
                parseExtern();
                break;


                default:
                parseTopLevelExpr();
                break;
            }
        }
    }

    int Parser::getTokPrecedence() {
        if (!isascii(lexer.curTok))
            return -1;
            
        int tokPrec = BinopPrecedence[lexer.curTok];
        if (tokPrec <= 0) return -1;
            return tokPrec;
    }

    std::unique_ptr<ExprAST> Parser::parseNumberExpr() {
        auto result = std::make_unique<NumberExprAST>(lexer.getNumVal());
        lexer.getNextToken();
        return std::move(result);
    }

    std::unique_ptr<ExprAST> Parser::parseIdentifierExpr() {
        std::string idName = lexer.getIdentifierStr();
        lexer.getNextToken();

        // simple varialble reference
        if (lexer.curTok != '(') {
            return std::make_unique<VariableExprAST>(idName);
        }

        // function call
        lexer.getNextToken(); // eat '('

        std::vector<std::unique_ptr<ExprAST>> args;

        if (lexer.curTok != ')') {
            while (true) {
                if (auto arg = parseExpression()) {
                    args.push_back(std::move(arg));
                } else {
                    return nullptr;
                }

                if (lexer.curTok == ')') {
                    break;
                }

                if (lexer.curTok != ',') {
                    return LogError("Expected ')' or ',' in argument list");
                }
                lexer.getNextToken();
            }
        }

        lexer.getNextToken(); // eat ')'

        return std::make_unique<CallExprAST>(idName, std::move(args));
    }

    std::unique_ptr<ExprAST> Parser::parseParenExpr() {
        lexer.getNextToken(); // eat (
        auto V = parseExpression();
        if (!V)
            return nullptr;

        if (lexer.curTok != ')')
            return LogError("expected ')'");
        lexer.getNextToken(); // eat ).
        return V;
    }

    std::unique_ptr<ExprAST> Parser::parsePrimary() {
          switch (lexer.curTok) {
            default:
                return LogError("unknown token when expecting an expression");
            case tok_identifier:
                return parseIdentifierExpr();
            case tok_number:
                return parseNumberExpr();
            case '(':
                return parseParenExpr();
        }
    }

    std::unique_ptr<ExprAST> Parser::parseBinOpRHS(int exprPrec, std::unique_ptr<ExprAST> LHS) {
        while (true) {
            int tokPrec = getTokPrecedence();
            
            if (tokPrec < exprPrec)
                return LHS;
                
            int binOp = lexer.curTok;
            lexer.getNextToken();
            
            auto RHS = parsePrimary();
            if (!RHS)
                return nullptr;
                
            int NextPrec = getTokPrecedence();
            if (tokPrec < NextPrec) {
                RHS = parseBinOpRHS(tokPrec + 1, std::move(RHS));
                if (!RHS)
                return nullptr;
            }
            
            LHS = std::make_unique<BinaryExprAST>(binOp, std::move(LHS), std::move(RHS));
        }
    }

    std::unique_ptr<ExprAST> Parser::parseExpression() {
        auto LHS = parsePrimary();

        if (!LHS)
            return nullptr;

        return parseBinOpRHS(0, std::move(LHS));
    }

    std::unique_ptr<PrototypeAST> Parser::parsePrototype() {
        if (lexer.curTok != tok_identifier) {
            return LogErrorP("Expected function name in prototype");
        }

        std::string fnName = lexer.getIdentifierStr();
        lexer.getNextToken();

        if (lexer.curTok != '(') {
            return LogErrorP("Expected '(' in prototype");
        }

        std::vector<std::string> argNames;
        while (lexer.getNextToken() == tok_identifier) {
            argNames.push_back(lexer.getIdentifierStr());
        }

        if (lexer.curTok != ')') {
            return LogErrorP("Expected ')' in prototype");
        }

        lexer.getNextToken(); // eat ')'

        return std::make_unique<PrototypeAST>(fnName, std::move(argNames));
    }

    std::unique_ptr<FunctionAST> Parser::parseDefinition() {
        lexer.getNextToken();
        auto proto = parsePrototype();

        if (!proto) {
            return nullptr;
        }

        if (auto e = parseExpression()) {
            return std::make_unique<FunctionAST>(std::move(proto), std::move(e));
        }

        return nullptr;
    }

    std::unique_ptr<PrototypeAST> Parser::parseExtern() {
        lexer.getNextToken();
        return parsePrototype();
    }

    std::unique_ptr<FunctionAST> Parser::parseTopLevelExpr() {
        if (auto e = parseExpression()) {
            auto proto = std::make_unique<PrototypeAST>("__anon_expr", std::vector<std::string>());
            return std::make_unique<FunctionAST>(std::move(proto), std::move(e));
        }
        return nullptr;
    }
    
    std::unique_ptr<ExprAST> Parser::LogError(const char *Str) {
        fprintf(stderr, "Error: %s\n", Str);
        return nullptr;
    }
    
    std::unique_ptr<PrototypeAST> Parser::LogErrorP(const char *Str) {
        LogError(Str);
        return nullptr;
    }

}