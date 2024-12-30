%{
#define YYDEBUG 1

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string>
#include "parser.tab.h"

extern FILE *yyin;
extern int yylex();
extern int yyparse();
int yyerror(std::string s);

%}

%union {
    int number;      // For numeric literals
    char *string;    // For identifiers
}

%token <string> IDENTIFIER
%token <number> NUMBER
%token PROGRAM PROCEDURE IS T_BEGIN END IF THEN ELSE ENDIF WHILE DO ENDWHILE REPEAT UNTIL FOR FROM TO DOWNTO READ WRITE ENDFOR
%token T_ASSIGN T_PLUS T_MINUS T_MUL T_DIV T_MOD T_EQ T_NEQ T_GT T_LT T_GTE T_LTE

%type <number> value

%%

program_all:
    procedures main                         { ; }
    | main                                  { ; }
    ;

procedures:
    procedures PROCEDURE proc_head IS declarations T_BEGIN commands END   { ; }
    | procedures PROCEDURE proc_head IS T_BEGIN commands END              { ; }
    | /* epsilon */                                                       { ; }
    ;

main:
    PROGRAM IS declarations T_BEGIN commands END    { ; }
    | PROGRAM IS T_BEGIN commands END               { ; }
    ;

commands:
    commands command                                { ; }
    | command                                       { ; }
    ;

command:
    IDENTIFIER T_ASSIGN expression ';'                            { std::cout << $1 << ":="; }
    | IF condition THEN commands ELSE commands ENDIF              { ; }
    | IF condition THEN commands ENDIF                            { ; }
    | WHILE condition DO commands ENDWHILE                        { ; }
    | REPEAT commands UNTIL condition ';'                         { ; }
    | FOR IDENTIFIER FROM value TO value DO commands ENDFOR       { ; }
    | FOR IDENTIFIER FROM value DOWNTO value DO commands ENDFOR   { ; }
    | proc_call ';'                                               { ; }
    | READ IDENTIFIER ';'                                         { ; }
    | WRITE value ';'                                             { ; }
    ;

proc_head:
    IDENTIFIER '(' args_decl ')'    { ; }
    ;

proc_call:
    IDENTIFIER '(' args ')'         { ; }
    ;

declarations:
    declarations ',' IDENTIFIER                             { std::cout << ", " << $3 << std::endl; }
    | declarations ',' IDENTIFIER '[' NUMBER ':' NUMBER ']' { std::cout << ", " << $3 << std::endl; }
    | IDENTIFIER                                            { std::cout << $1; }
    | IDENTIFIER '[' NUMBER ':' NUMBER ']'                  { std::cout << $1; }
    ;

args_decl:
    args_decl ',' IDENTIFIER                         { ; }
    | args_decl ',' 'T' IDENTIFIER                   { ; }
    | IDENTIFIER                                     { ; }
    | 'T' IDENTIFIER                                 { ; }
    ;

args:
    args ',' IDENTIFIER                              { ; }
    | IDENTIFIER                                     { ; }
    ;

expression:
    value                                            { ; }
    | value T_PLUS value                             { std::cout << $1 << " + " << $3; }
    | value T_MINUS value                            { std::cout << $1 << " - " << $3; }
    | value T_MUL value                              { std::cout << $1 << " * " << $3; }
    | value T_DIV value                              { std::cout << $1 << " / " << $3; }
    | value T_MOD value                              { std::cout << $1 << " % " << $3; }
    ;

condition:
    value T_EQ value                                 { std::cout << "="; }
    | value T_NEQ value                              { std::cout << "!="; }
    | value T_GT value                               { std::cout << ">"; }
    | value T_LT value                               { std::cout << "<"; }
    | value T_GTE value                              { std::cout << ">="; }
    | value T_LTE value                              { std::cout << "<="; }
    ;

value:
    NUMBER                                           { $$ = $1; }
    | IDENTIFIER                                     { std::cout << $1 << std::endl; }
    | IDENTIFIER '[' IDENTIFIER ']'                  { std::cout << $1 << "[" << $3 << "]" << std::endl; }
    | IDENTIFIER '[' NUMBER ']'                      { std::cout << $1 << "[" << $3 << "]" << std::endl; }
    ;

%%

int yyerror(std::string s) {
    std::cout << s << std::endl;
    return 1;
}
