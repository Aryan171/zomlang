#pragma once

#include <string_view>
#include <string>

namespace zomlang {
    enum Token {
        tok_eof = -1,
        tok_def = -2,
        tok_identifier = -3,
        tok_number = -4,
        tok_extern = -5
    };

    class Lexer {
        private:
        std::string_view buffer;
        size_t cursor;
        double numVal;
        std::string identifierStr;
        int curTok;
        int lineNo;
        int colNo;

        char getNextChar();
        int getTok();

        public:
        explicit Lexer(std::string_view sourceCode);
        int getNextToken();
        double getNumVal() const;
        const std::string& getIdentifierStr() const;
        int getLineNo() const;
        int getColNo() const;
    };
}