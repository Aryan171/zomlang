#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <zomlang/Parser.hpp>
#include <zomlang/Lexer.hpp>

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

    parser.parse();
}