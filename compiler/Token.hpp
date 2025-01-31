#ifndef TOKEN_HPP
#define TOKEN_HPP

#include <string>
#include <iostream>
#include <variant>

// Enum typów tokenów
enum class TokenType {
    PROGRAM, PROCEDURE, IS, T_BEGIN, END, IF, THEN, ELSE, ENDIF, WHILE, DO, ENDWHILE,
    REPEAT, UNTIL, FOR, ENDFOR, FROM, TO, DOWNTO, READ, WRITE,
    T_ASSIGN, T_PLUS, T_MINUS, T_MUL, T_DIV, T_MOD, T_COMMA,
    NUMBER, IDENTIFIER, UNKNOWN, END_OF_FILE,
    T_EQ, T_NEQ, T_GT, T_LT, T_GTE, T_LTE,
    T_SEMICOLON, T_COLON, T_TABLE, T_LPAREN, T_RPAREN, T_LBRACKET, T_RBRACKET
};

enum class TokenFunction {
    DEFAULT,    // Out of procedure value
    ARG,        // Procedure argument
    PROC        // Return address
};

class Token {
public:
    Token(TokenType type = TokenType::UNKNOWN, std::string value = "", unsigned long long line = 0, unsigned long long column = 0, long long address = -1, bool reassignable = false, TokenFunction function = TokenFunction::DEFAULT)
        : type(type), value(value), line(line), column(column), address(address), reassignable(reassignable), function(function) {}

    TokenType getType() const { return type; }
    std::string getValue() const { return value; }
    unsigned long long getLine() const { return line; }
    unsigned long long getColumn() const { return column; }
    long long getAddress() const { return address; }
    bool getAssignibility() const { return reassignable; }
    TokenFunction getFunction() const { return function; }

    void setAddress(long long addr) { this->address = addr; }
    void setValue(std::string value) { this->value = value; }
    Token* setFunction(TokenFunction function) { this->function = function; return this; }
    Token* setAssignability(bool reass) { this->reassignable = reass; return this; }

    void print() const {
        std::cout << "Token(Type: " << tokenTypeToString(type)
                  << ", Value: " << value
                  << ", Line: " << line
                  << ", Column: " << column
                  << ", Address: " << address
                  << ", Reasignable: " << reassignable << std::boolalpha
                  << ", Function: " << tokenFunctionToString(function) << ")\n";
    }

private:
    TokenType type;
    std::string value;
    unsigned long long line;
    unsigned long long column;
    long long address;
    bool reassignable;
    TokenFunction function;

    static std::string tokenTypeToString(TokenType type) {
        switch (type) {
            case TokenType::PROGRAM: return "PROGRAM";
            case TokenType::PROCEDURE: return "PROCEDURE";
            case TokenType::IS: return "IS";
            case TokenType::T_BEGIN: return "T_BEGIN";
            case TokenType::END: return "END";
            case TokenType::IF: return "IF";
            case TokenType::THEN: return "THEN";
            case TokenType::ELSE: return "ELSE";
            case TokenType::ENDIF: return "ENDIF";
            case TokenType::WHILE: return "WHILE";
            case TokenType::DO: return "DO";
            case TokenType::ENDWHILE: return "ENDWHILE";
            case TokenType::REPEAT: return "REPEAT";
            case TokenType::UNTIL: return "UNTIL";
            case TokenType::FOR: return "FOR";
            case TokenType::ENDFOR: return "ENDFOR";
            case TokenType::FROM: return "FROM";
            case TokenType::TO: return "TO";
            case TokenType::DOWNTO: return "DOWNTO";
            case TokenType::READ: return "READ";
            case TokenType::WRITE: return "WRITE";
            case TokenType::T_ASSIGN: return "T_ASSIGN";
            case TokenType::T_PLUS: return "T_PLUS";
            case TokenType::T_MINUS: return "T_MINUS";
            case TokenType::T_MUL: return "T_MUL";
            case TokenType::T_DIV: return "T_DIV";
            case TokenType::T_MOD: return "T_MOD";
            case TokenType::T_COMMA: return "COMMA";
            case TokenType::NUMBER: return "NUMBER";
            case TokenType::IDENTIFIER: return "IDENTIFIER";
            case TokenType::UNKNOWN: return "UNKNOWN";
            case TokenType::END_OF_FILE: return "END_OF_FILE";
            case TokenType::T_EQ: return "T_EQ";
            case TokenType::T_NEQ: return "T_NEQ";
            case TokenType::T_GT: return "T_GT";
            case TokenType::T_LT: return "T_LT";
            case TokenType::T_GTE: return "T_GTE";
            case TokenType::T_LTE: return "T_LTE";
            case TokenType::T_SEMICOLON: return "SEMICOLON";
            case TokenType::T_COLON: return "COLON";
            case TokenType::T_TABLE: return "T";
            case TokenType::T_LPAREN: return "LPAREN";
            case TokenType::T_RPAREN: return "RPAREN";
            case TokenType::T_LBRACKET: return "LBRACKET";
            case TokenType::T_RBRACKET: return "RBRACKET";
            default: return "UNKNOWN";
        }
    }

        static std::string tokenFunctionToString(TokenFunction function) {
        switch (function) {
            case TokenFunction::PROC: return "PROCEDURE";
            case TokenFunction::ARG: return "ARGUMENT";
            default: return "DEFAULT";
        }
    }
};

#endif
