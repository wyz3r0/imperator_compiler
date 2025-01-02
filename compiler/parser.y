%{
#define YYDEBUG 1

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string>
#include "Token.hpp"
#include "parser.tab.h"

extern FILE *yyin;
extern int yylex();
extern int yyparse();
int yyerror(std::string s);

%}


%union {
    Token* token;
}

%token <token> PROGRAM PROCEDURE IS T_BEGIN END IF THEN ELSE ENDIF WHILE DO ENDWHILE REPEAT UNTIL FOR ENDFOR FROM TO DOWNTO READ WRITE
%token <token> T_ASSIGN T_PLUS T_MINUS T_MUL T_DIV T_MOD T_COMMA
%token <token> NUMBER IDENTIFIER
%token <token> T_EQ T_NEQ T_GT T_LT T_GTE T_LTE
%token <token> T_SEMICOLON T_COLON T_TABLE T_LPAREN T_RPAREN T_LBRACKET T_RBRACKET

%start program_all

%%

program_all:
    procedures main { printf("Parsed program_all\n"); }
    ;

procedures:
    procedures PROCEDURE proc_head IS declarations T_BEGIN commands END { printf("Parsed procedures with declarations\n"); }
    | procedures PROCEDURE proc_head IS T_BEGIN commands END { printf("Parsed procedures without declarations\n"); }
    | %empty { printf("Parsed empty procedures\n"); }
    ;

main:
    PROGRAM IS declarations T_BEGIN commands END { printf("Parsed main with declarations\n"); }
    | PROGRAM IS T_BEGIN commands END { printf("Parsed main without declarations\n"); }
    ;

commands:
    commands command { printf("Parsed commands (multiple)\n"); }
    | command { printf("Parsed command (single)\n"); }
    | %empty { printf("Parsed empty command\n"); }
    ;

command:
    IDENTIFIER T_ASSIGN expression T_SEMICOLON { printf("Parsed assignment command\n"); }
    | IF condition THEN commands ELSE commands ENDIF { printf("Parsed IF-ELSE command\n"); }
    | IF condition THEN commands ENDIF { printf("Parsed IF command\n"); }
    | WHILE condition DO commands ENDWHILE { printf("Parsed WHILE command\n"); }
    | REPEAT commands UNTIL condition T_SEMICOLON { printf("Parsed REPEAT command\n"); }  // Adjust UNTIL rule
    | FOR IDENTIFIER FROM value TO value DO commands ENDFOR { printf("Parsed FOR command (TO)\n"); }
    | FOR IDENTIFIER FROM value DOWNTO value DO commands ENDFOR { printf("Parsed FOR command (DOWNTO)\n"); }
    | proc_call T_SEMICOLON { printf("Parsed procedure call command\n"); }
    | READ IDENTIFIER T_SEMICOLON { printf("Parsed READ command\n"); }
    | WRITE value T_SEMICOLON { printf("Parsed WRITE command\n"); }
    ;

proc_head:
    IDENTIFIER T_LPAREN args_decl T_RPAREN { printf("Parsed procedure head\n"); }
    ;

proc_call:
    IDENTIFIER T_LPAREN args T_RPAREN { printf("Parsed procedure call\n"); }
    ;

declarations:
    declarations T_COMMA IDENTIFIER { printf("Parsed declarations (multiple)\n"); }
    | declarations T_COMMA IDENTIFIER T_LBRACKET NUMBER T_COLON NUMBER T_RBRACKET { printf("Parsed declarations with array\n"); }
    | IDENTIFIER { printf("Parsed single declaration\n"); }
    | IDENTIFIER T_LBRACKET NUMBER T_COLON NUMBER T_RBRACKET { printf("Parsed single array declaration\n"); }
    ;

args_decl:
    args_decl T_COMMA IDENTIFIER { printf("Parsed arguments declaration (multiple)\n"); }
    | args_decl T_COMMA T_TABLE IDENTIFIER { printf("Parsed arguments declaration with table\n"); }
    | IDENTIFIER { printf("Parsed single argument declaration\n"); }
    | T_TABLE IDENTIFIER { printf("Parsed single table argument declaration\n"); }
    ;

args:
    args T_COMMA IDENTIFIER { printf("Parsed arguments (multiple)\n"); }
    | IDENTIFIER { printf("Parsed single argument\n"); }
    ;

expression:
    value { printf("Parsed expression (single value)\n"); }
    | value T_PLUS value { printf("Parsed expression (addition)\n"); }
    | value T_MINUS value { printf("Parsed expression (subtraction)\n"); }
    | value T_MUL value { printf("Parsed expression (multiplication)\n"); }
    | value T_DIV value { printf("Parsed expression (division)\n"); }
    | value T_MOD value { printf("Parsed expression (modulus)\n"); }
    ;

condition:
    value T_EQ value { printf("Parsed condition (equal)\n"); }
    | value T_NEQ value { printf("Parsed condition (not equal)\n"); }
    | value T_GT value { printf("Parsed condition (greater than)\n"); }
    | value T_LT value { printf("Parsed condition (less than)\n"); }
    | value T_GTE value { printf("Parsed condition (greater than or equal)\n"); }
    | value T_LTE value { printf("Parsed condition (less than or equal)\n"); }
    ;

value:
    NUMBER { printf("Parsed value (number)\n"); }
    | identifier { printf("Parsed value (identifier)\n"); }
    ;

identifier:
    IDENTIFIER { printf("Parsed identifier\n"); }
    | IDENTIFIER T_LBRACKET IDENTIFIER T_RBRACKET { printf("Parsed array identifier (variable index)\n"); }
    | IDENTIFIER T_LBRACKET NUMBER T_RBRACKET { printf("Parsed array identifier (number index)\n"); }
    ;

%%

int yyerror(std::string s) {
    printf("Syntax error: ");
    if (yylval.token) {
        yylval.token->print();
    } else {
        printf("unexpected token\n");
    }
    return 1;
}
