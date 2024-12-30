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

%code requires {



}

%union {
    Token* token;
}

%token <token> IDENTIFIER
%token <token> NUMBER
%token <token> PROGRAM PROCEDURE IS T_BEGIN END IF THEN ELSE ENDIF WHILE DO ENDWHILE REPEAT UNTIL FOR FROM TO DOWNTO READ WRITE ENDFOR
%token <token> T_ASSIGN T_PLUS T_MINUS T_MUL T_DIV T_MOD T_EQ T_NEQ T_GT T_LT T_GTE T_LTE
%token <token> UNKNOWN

%%

program_all:
    procedures main                         { ; }
    | main                                  { ; }
    ;

procedures:
    procedures PROCEDURE proc_head IS declarations T_BEGIN commands END   { ; }
    | procedures PROCEDURE proc_head IS T_BEGIN commands END              { ; }
    | /* no procedures */                                                       { ; }
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
    IDENTIFIER T_ASSIGN expression ';'                            { ; }
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
    declarations ',' IDENTIFIER                             { ; }
    | declarations ',' IDENTIFIER '[' NUMBER ':' NUMBER ']' { ; }
    | IDENTIFIER                                            { ; }
    | IDENTIFIER '[' NUMBER ':' NUMBER ']'                  { ; }
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
    | value T_PLUS value                             { ; }
    | value T_MINUS value                            { ; }
    | value T_MUL value                              { ; }
    | value T_DIV value                              { ; }
    | value T_MOD value                              { ; }
    ;

condition:
    value T_EQ value                                 { ; }
    | value T_NEQ value                              { ; }
    | value T_GT value                               { ; }
    | value T_LT value                               { ; }
    | value T_GTE value                              { ; }
    | value T_LTE value                              { ; }
    ;

value:
    NUMBER                                           { ; }
    | IDENTIFIER                                     { ; }
    | IDENTIFIER '[' IDENTIFIER ']'                  { ; }
    | IDENTIFIER '[' NUMBER ']'                      { ; }
    ;

%%

int yyerror(std::string s) {
    std::cout << s << std::endl;
    return 1;
}
