#include "zomlang/Lexer.hpp"
#include <stdexcept>

namespace zomlang {
    Lexer::Lexer(std::string_view sourceCode) : buffer(sourceCode), cursor(0), numVal(0.0), lineNo(1), colNo(1) {}

    char Lexer::getNextChar() {
        if (cursor >= buffer.size()) {
            return EOF;
        }
        
        char c = buffer[cursor++];
        if (c == '\n') {
            lineNo++;
            colNo = 1;
        } else {
            colNo++;
        }
        return c;
    }

    int Lexer::getNextToken() {
        int lastChar = ' ';
        while (isspace(lastChar)) {
            lastChar = getNextChar();
        }

        // def, extern keyword
        if (isalpha(lastChar)) {
            identifierStr = lastChar;
            while (isalnum((lastChar = getNextChar()))) {
                identifierStr += lastChar;
            }
            
            if (identifierStr == "def") {
                return tok_def;
            }

            if (identifierStr == "extern") {
                return tok_extern;
            }

            return tok_identifier;
        }
        
        // decimal number
        if (isdigit(lastChar) || lastChar == '.') {
            std::string numStr;
            bool decimalPointSeen = false;

            if (lastChar == '.') {
                decimalPointSeen = true;
            }

            do {
                numStr += lastChar;
                lastChar = getNextChar();

                if (lastChar == '.') {
                    if (decimalPointSeen) {
                        throw std::runtime_error("Error: Malformed number literal with multiple decimal points ('"
                             + numStr + ".') at line " + std::to_string(lineNo) + ", column " + std::to_string(colNo));
                    }
                    decimalPointSeen = true;
                }
            } while (isdigit(lastChar) || lastChar == '.');

            numVal = strtod(numStr.c_str(), nullptr);
            return tok_number;
        }

        // comment
        if (lastChar == '#') {
            do {
                lastChar = getNextChar();
            } while (lastChar != EOF && lastChar != '\n' && lastChar != '\r');

            if (lastChar != EOF) {
                return getNextToken();
            }
        }

        // Check for end of file.  Don't eat the EOF.
        if (lastChar == EOF) {
            return tok_eof;
        }
        
        int thisChar = lastChar;
        lastChar = getNextChar();
        return thisChar;
    }

    const std::string& Lexer::getIdentifierStr() const {
        return identifierStr;
    }

    double Lexer::getNumVal() const {
        return numVal;
    }
}