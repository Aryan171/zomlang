#include <iostream>
#include <zomlang/Lexer.hpp>

int main() {
    std::string_view sourceCode = "def foo(x) x + 1\n1.01.5";
    zomlang::Lexer lexer(sourceCode);

    for (int token = lexer.getNextToken(); token != zomlang::tok_eof; token = lexer.getNextToken()) {
        switch (token) {
            case zomlang::tok_def:
                std::cout << "Token: `def`\n";
                break;
            case zomlang::tok_extern:
                std::cout << "Token: `extern`\n";
                break;
            case zomlang::tok_identifier:
                std::cout << "Token: identifier `" << lexer.getIdentifierStr() << "`\n";
                break;
            case zomlang::tok_number:
                std::cout << "Token: number `" << lexer.getNumVal() << "`\n";
                break;
            default:
                std::cout << "Unknown token: `" << (char)token << "`\n";
                break;
        }
    }
}
