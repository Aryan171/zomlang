#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <zomlang/Parser.hpp>
#include <zomlang/Lexer.hpp>
#include "zomlang/Parser.hpp"
#include "zomlang/CompilerState.hpp"
#include "llvm/Support/TargetSelect.h"
#include "llvm/Support/raw_ostream.h"
#include <cstdio>

namespace zomlang {
    void RunREPL(CompilerState& state, Parser& parser, Lexer& lexer) {
        while (true) {
            fprintf(stderr, "ready> ");
            switch (lexer.curTok) {
                case tok_eof:
                    return;

                case ';': // ignore top-level semicolons.
                    lexer.getNextToken();
                    break;

                case tok_def: {
                    if (auto FnAST = parser.handleDefinition()) {
                        // Pass the CompilerState to codegen
                        if (auto *IRFunction = FnAST->codegen(state)) {
                            fprintf(stderr, "Parsed a function definition:\n");
                            IRFunction->print(llvm::errs());
                            fprintf(stderr, "\n");
                        }
                    } else {
                        lexer.getNextToken(); // Skip token on error recovery
                    }
                    break;
                }

                case tok_extern: {
                    if (auto ProtoAST = parser.handleExtern()) {
                        if (auto *IRFunction = ProtoAST->codegen(state)) {
                            fprintf(stderr, "Parsed an extern:\n");
                            IRFunction->print(llvm::errs());
                            fprintf(stderr, "\n");
                        }
                    } else {
                        lexer.getNextToken();
                    }
                    break;
                }

                default: {
                    if (auto FnAST = parser.handleTopLevelExpr()) {
                        if (auto *IRFunction = FnAST->codegen(state)) {
                            fprintf(stderr, "Parsed a top-level expression:\n");
                            IRFunction->print(llvm::errs());
                            fprintf(stderr, "\n");
                        }
                    } else {
                        lexer.getNextToken();
                    }
                    break;
                }
            }
        }
    }
}

int main() {
    std::ifstream file("../../examples/fibonacci.zom");
    if (!file) {
        std::cerr << "Failed to open examples/fibonacci.zom\n";
        return 1;
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string source = buffer.str();

    std::string_view sourceCode(source);
    zomlang::Lexer lexer(sourceCode);
    zomlang::Parser parser(lexer);
    zomlang::CompilerState state;

    zomlang::RunREPL(state,parser, lexer);
}