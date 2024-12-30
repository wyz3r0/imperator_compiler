#ifndef TOKEN_HPP
#define TOKEN_HPP

#include <string>
#include <iostream>
#include <variant>

// Typy wartości tokenów
using TokenValue = std::variant<std::string, int>;

// Enum typów tokenów
enum class TokenType {
    PROGRAM, PROCEDURE, IS, T_BEGIN, END, IF, THEN, ELSE, ENDIF, WHILE, DO, ENDWHILE,
    REPEAT, UNTIL, FOR, ENDFOR, FROM, TO, DOWNTO, READ, WRITE,
    T_ASSIGN, T_PLUS, T_MINUS, T_MUL, T_DIV, T_MOD,
    NUMBER, IDENTIFIER, COMMENT, UNKNOWN, END_OF_FILE
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
            case TokenType::T_ASSIGN: return ":=";
            case TokenType::T_PLUS: return "+";
            case TokenType::T_MINUS: return "-";
            case TokenType::T_MUL: return "*";
            case TokenType::T_DIV: return "/";
            case TokenType::T_MOD: return "%";
            case TokenType::NUMBER: return "NUMBER";
            case TokenType::IDENTIFIER: return "IDENTIFIER";
            case TokenType::COMMENT: return "COMMENT";
            case TokenType::UNKNOWN: return "UNKNOWN";
            case TokenType::END_OF_FILE: return "END_OF_FILE";
        }
        return "UNKNOWN";
    }

    std::string valueToString() const {
        if (std::holds_alternative<std::string>(value)) {
            return std::get<std::string>(value);
        } else if (std::holds_alternative<int>(value)) {
            return std::to_string(std::get<int>(value));
        }
        return "NONE";
    }
};

#endif
