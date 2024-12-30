#include "Token.hpp"  // Include your Token class definition
#include <iostream>
#include <cstdio>
#include "parser.tab.h"

// Declare the symbols defined in lex.yy.c
extern FILE* yyin;           // Input file for the lexer
extern FILE* yyout;          // Output file for the lexer (if used)
extern int yylineno;         // Line number for the lexer
extern char* yytext;         // Current token text for the lexer
extern int yyleng;           // Length of the current token
extern int yylex();          // Function to run the lexer
extern YYSTYPE yylval;

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <source-file>" << std::endl;
        return 1;
    }

    // Open the source file
    FILE* file = fopen(argv[1], "r");
    if (!file) {
        std::cerr << "Error opening file: " << argv[1] << std::endl;
        return 1;
    }

    // Set the input for the lexer to the file
    yyin = file;

    // Tokenize the file input
    while (yylex()) {
        yylval.token->print();
    }

    // Close the file
    fclose(file);

    return 0;
}
