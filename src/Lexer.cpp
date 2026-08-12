#include "zomlang/Lexer.hpp"
#include <stdexcept>

namespace zomlang {
    Lexer::Lexer(std::string_view sourceCode) : 
    buffer(sourceCode), cursor(0), numVal(0.0), lineNo(1), colNo(1), lastChar(' ') {
        this->getNextToken(); // Initialize the first token
    }

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

    int Lexer::getTok() {
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

            if (identifierStr == "if") {
                return tok_if;
            }

            if (identifierStr == "else") {
                return tok_else;
            }

            if (identifierStr == "then") {
                return tok_then;
            }            

            return tok_identifier;
        }
        
        // decimal number
        if (isdigit(lastChar) || lastChar == '.') {
            std::string numStr;
            bool decimalPointSeen = false;

            do {
                if (lastChar == '.') {
                    if (decimalPointSeen) {
                        throw std::runtime_error("Error: Malformed number literal with multiple decimal points ('"
                            + numStr + "...') at line " + std::to_string(lineNo) + ", column " + std::to_string(colNo));
                    }
                    decimalPointSeen = true;
                }

                numStr += lastChar;
                lastChar = getNextChar();
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
                return getTok();
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

    int Lexer::getNextToken() {
        return curTok = getTok();
    }

    int Lexer::getColNo() const {
        return colNo;
    }

    int Lexer::getLineNo() const {
        return lineNo;
    }
}