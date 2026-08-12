#pragma once

#include <string_view>
#include <string>

namespace zomlang {
    enum Token {
        tok_eof = -1,
        tok_def = -2,
        tok_identifier = -3,
        tok_number = -4,
        tok_extern = -5,
        tok_if = -6,
        tok_then = -7,
        tok_else = -8
    };

    class Lexer {
        private:
        std::string_view buffer;
        size_t cursor;
        double numVal;
        std::string identifierStr;
        int lineNo;
        int colNo;
        char lastChar;

        char getNextChar();
        int getTok();

        public:
        int curTok;
        explicit Lexer(std::string_view sourceCode);
        int getNextToken();
        double getNumVal() const;
        const std::string& getIdentifierStr() const;
        int getLineNo() const;
        int getColNo() const;
    };
}