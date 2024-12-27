%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void yyerror(const char *s);
int yylex();
%}

%union {
    int ival;
    char *sval;
}

%token <ival> NUM
%token <sval> PIDENTIFIER
%token PROGRAM IS BEGIN END IF THEN ELSE ENDIF FOR TO DOWNTO DO ENDFOR REPEAT UNTIL PROCEDURE READ WRITE HALT
%token EQ NEQ GT LT GEQ LEQ COLON SEMICOLON COMMA LBRACKET RBRACKET LPAREN RPAREN PLUS MINUS TIMES DIV MOD FROM

%%

program:
    PROGRAM IS declarations BEGIN commands END
    {
        printf("Program parsed successfully!\n");
    };

declarations:
    declarations ',' PIDENTIFIER
    | declarations ',' PIDENTIFIER LBRACKET NUM COLON NUM RBRACKET
    | PIDENTIFIER
    | PIDENTIFIER LBRACKET NUM COLON NUM RBRACKET;

commands:
    commands command
    | command;

command:
    PIDENTIFIER COLON EQ expression SEMICOLON
    | IF condition THEN commands ENDIF
    | IF condition THEN commands ELSE commands ENDIF
    | FOR PIDENTIFIER FROM NUM TO NUM DO commands ENDFOR
    | FOR PIDENTIFIER FROM NUM DOWNTO NUM DO commands ENDFOR
    | REPEAT commands UNTIL condition SEMICOLON
    | READ PIDENTIFIER SEMICOLON
    | WRITE expression SEMICOLON;

expression:
    NUM
    | PIDENTIFIER
    | expression PLUS expression
    | expression MINUS expression
    | expression TIMES expression
    | expression DIV expression
    | expression MOD expression;

condition:
    expression EQ expression
    | expression NEQ expression
    | expression GT expression
    | expression LT expression
    | expression GEQ expression
    | expression LEQ expression;

%%

void yyerror(const char *s) {
    fprintf(stderr, "Error: %s\n", s);
}

int main(int argc, char **argv) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <input file>\n", argv[0]);
        return 1;
    }
    FILE *input = fopen(argv[1], "r");
    if (!input) {
        perror("Error opening input file");
        return 1;
    }
    yyin = input;
    yyparse();
    fclose(input);
    return 0;
}