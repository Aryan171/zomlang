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
#include "llvm/Support/Error.h"
#include "llvm/ExecutionEngine/Orc/ThreadSafeModule.h"
#include <utility>
#include <cstdio>

namespace zomlang {
    std::unique_ptr<FunctionAST> handleDefinition(Lexer& lexer, Parser& parser, CompilerState& cs) {
        if (auto FnAST = parser.parseDefinition()) {
            if (auto *FnIR = FnAST->codegen(cs)) {
                fprintf(stderr, "Read function definition:");
                FnIR->print(llvm::errs());
                fprintf(stderr, "\n");
                llvm::cantFail(cs.getJIT().addModule(
                    llvm::orc::ThreadSafeModule(std::move(cs.takeModule()), std::move(cs.takeContext()))));
                cs.InitializeModuleAndManagers();
            }
        } 
        else {
            lexer.getNextToken();
        }
        
        return nullptr;
    }

    std::unique_ptr<PrototypeAST> handleExtern(Lexer& lexer, Parser& parser, CompilerState& cs) {
        if (auto ProtoAST = parser.parseExtern()) {
            if (auto *FnIR = ProtoAST->codegen(cs)) {
                fprintf(stderr, "Read extern: ");
                FnIR->print(llvm::errs());
                fprintf(stderr, "\n");
                cs.getFunctionProtos()[ProtoAST->getName()] = std::move(ProtoAST);
            }
        }
        else {
            lexer.getNextToken();
        }

        return nullptr;
    }

    std::unique_ptr<FunctionAST> handleTopLevelExpr(Lexer& lexer, Parser& parser, CompilerState& cs) {
        if (auto FnAST = parser.parseTopLevelExpr()) {
            if (FnAST->codegen(cs)) {
                auto RT = cs.getJIT().getMainJITDylib().createResourceTracker();
                auto TSM = llvm::orc::ThreadSafeModule(std::move(cs.takeModule()), std::move(cs.takeContext()));
                
                llvm::cantFail(cs.getJIT().addModule(std::move(TSM), RT));
                cs.InitializeModuleAndManagers();

                auto ExprSymbol = llvm::cantFail(cs.getJIT().lookup("__anon_expr"));

                double (*FP)() = ExprSymbol.toPtr<double(*)()>();
                fprintf(stderr, "Evaluated to %f\n", FP());

                llvm::cantFail(RT->remove());
            }
        }
        return nullptr;
    }

    void RunREPL(Lexer& lexer, Parser& parser, CompilerState& cs) {
        while (true) {
            fprintf(stderr, "ready> ");
            switch (lexer.curTok) {
                case tok_eof:
                    return;

                case ';': // ignore top-level semicolons.
                    lexer.getNextToken();
                    break;

                case tok_def: {
                    if (auto FnAST = handleDefinition(lexer, parser, cs)) {
                        // Pass the CompilerState to codegen
                        if (auto *IRFunction = FnAST->codegen(cs)) {
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
                    if (auto ProtoAST = handleExtern(lexer, parser, cs)) {
                        if (auto *IRFunction = ProtoAST->codegen(cs)) {
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
                    if (auto FnAST = handleTopLevelExpr(lexer, parser, cs)) {
                        if (auto *IRFunction = FnAST->codegen(cs)) {
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
    zomlang::CompilerState cs;

    zomlang::RunREPL(lexer, parser, cs);
}