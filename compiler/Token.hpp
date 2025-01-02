#ifndef TOKEN_HPP
#define TOKEN_HPP

#include <string>
#include <iostream>
#include <variant>

// Typy wartości tokenów
using TokenValue = std::variant<std::string, long>;

// Enum typów tokenów
enum class TokenType {
    PROGRAM, PROCEDURE, IS, T_BEGIN, END, IF, THEN, ELSE, ENDIF, WHILE, DO, ENDWHILE,
    REPEAT, UNTIL, FOR, ENDFOR, FROM, TO, DOWNTO, READ, WRITE,
    T_ASSIGN, T_PLUS, T_MINUS, T_MUL, T_DIV, T_MOD, T_COMMA,
    NUMBER, IDENTIFIER, UNKNOWN, END_OF_FILE,
    T_EQ, T_NEQ, T_GT, T_LT, T_GTE, T_LTE,
    T_SEMICOLON, T_COLON, T_TABLE, T_LPAREN, T_RPAREN, T_LBRACKET, T_RBRACKET
};


class Token {
public:
    Token(TokenType type = TokenType::UNKNOWN, const TokenValue& value = TokenValue{}, int line = 0, int column = 0)
        : type(type), value(value), line(line), column(column) {}

    TokenType getType() const { return type; }
    const TokenValue& getValue() const { return value; }
    int getLine() const { return line; }
    int getColumn() const { return column; }

    void print() const {
        std::cout << "Token(Type: " << tokenTypeToString(type)
                  << ", Value: " << valueToString()
                  << ", Line: " << line
                  << ", Column: " << column << ")\n";
    }

private:
    TokenType type;
    TokenValue value;
    int line;
    int column;

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


    std::string valueToString() const {
        if (std::holds_alternative<std::string>(value)) {
            return std::get<std::string>(value);
        } else if (std::holds_alternative<long>(value)) {
            return std::to_string(std::get<long>(value));
        }
        return "NONE";
    }
};

#endif
