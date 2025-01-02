%{
#define YYDEBUG 1

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string>
#include "Token.hpp"
#include "Node.hpp"
#include "parser.tab.h"

extern FILE *yyin;
extern int yylex();
extern int yyparse();
int yyerror(std::string s);

%}

%union {
    Token* token;
    Node* node;
}

%token <token> PROGRAM PROCEDURE IS T_BEGIN END IF THEN ELSE ENDIF WHILE DO ENDWHILE REPEAT UNTIL FOR ENDFOR FROM TO DOWNTO READ WRITE
%token <token> T_ASSIGN T_PLUS T_MINUS T_MUL T_DIV T_MOD T_COMMA
%token <token> NUMBER IDENTIFIER
%token <token> T_EQ T_NEQ T_GT T_LT T_GTE T_LTE
%token <token> T_SEMICOLON T_COLON T_TABLE T_LPAREN T_RPAREN T_LBRACKET T_RBRACKET

/* TODO : make sure these are correct */
%left  T_COMMA
%right T_COLON T_TABLE T_ASSIGN
%left  T_EQ T_NEQ T_GT T_LT T_GTE T_LTE
%left  T_PLUS T_MINUS
%left  T_MUL T_DIV T_MOD
%left  T_LPAREN T_LBRACKET

%type <node> program_all procedures main commands command proc_head proc_call declarations args_decl args expression condition value identifier

%start program_all

%%

program_all:
    procedures main {
        $$ = new Node(NodeType::PROGRAM_ALL);
        $$->addChild($1);  // Add procedures node
        $$->addChild($2);  // Add main node
        printf("Parsed program_all\n");
        $$->print();
    }
    ;

procedures:
    procedures PROCEDURE proc_head IS declarations T_BEGIN commands END {
        $$ = new Node(NodeType::PROCEDURES);
        $$->addChild($1);  // Add previous procedures
        $$->addChild($3);  // Add proc_head
        $$->addChild($5);  // Add declarations
        $$->addChild($7);  // Add commands
        printf("Parsed procedures with declarations\n");
    }
    | procedures PROCEDURE proc_head IS T_BEGIN commands END {
        $$ = new Node(NodeType::PROCEDURES);
        $$->addChild($1);  // Add previous procedures
        $$->addChild($3);  // Add proc_head
        $$->addChild($6);  // Add commands
        printf("Parsed procedures without declarations\n");
    }
    | %empty {
        $$ = new Node(NodeType::PROCEDURES);
        printf("Parsed empty procedures\n");
    }
    ;

main:
    PROGRAM IS declarations T_BEGIN commands END {
        $$ = new Node(NodeType::MAIN);
        $$->addChild($3);  // Add declarations
        $$->addChild($5);  // Add commands
        printf("Parsed main with declarations\n");
    }
    | PROGRAM IS T_BEGIN commands END {
        $$ = new Node(NodeType::MAIN);
        $$->addChild($4);  // Add commands
        printf("Parsed main without declarations\n");
    }
    ;

commands:
    commands command {
        $$ = new Node(NodeType::COMMANDS);
        $$->addChild($1);  // Add previous commands
        $$->addChild($2);  // Add the current command
        printf("Parsed commands (multiple)\n");
    }
    | command {
        $$ = new Node(NodeType::COMMANDS);
        $$->addChild($1);  // Add the single command
        printf("Parsed command (single)\n");
    }
    | %empty {
        $$ = new Node(NodeType::COMMANDS);
        printf("Parsed empty command\n");
    }
    ;

command:
    identifier T_ASSIGN expression T_SEMICOLON {
        $$ = new Node(NodeType::COMMAND);
        $$->addChild($1);  // Add IDENTIFIER token
        $$->addChild($3);  // Add the expression
        printf("Parsed assignment command\n");
    }
    | IF condition THEN commands ELSE commands ENDIF {
        $$ = new Node(NodeType::COMMAND);
        $$->addChild($2);  // Add condition
        $$->addChild($4);  // Add then commands
        $$->addChild($6);  // Add else commands
        printf("Parsed IF-ELSE command\n");
    }
    | IF condition THEN commands ENDIF {
        $$ = new Node(NodeType::COMMAND);
        $$->addChild($2);  // Add condition
        $$->addChild($4);  // Add commands
        printf("Parsed IF command\n");
    }
    | WHILE condition DO commands ENDWHILE {
        $$ = new Node(NodeType::COMMAND);
        $$->addChild($2);  // Add condition
        $$->addChild($4);  // Add commands
        printf("Parsed WHILE command\n");
    }
    | REPEAT commands UNTIL condition T_SEMICOLON {
        $$ = new Node(NodeType::COMMAND);
        $$->addChild($2);  // Add commands
        $$->addChild($4);  // Add condition
        printf("Parsed REPEAT command\n");
    }
    | FOR IDENTIFIER FROM value TO value DO commands ENDFOR {
        $$ = new Node(NodeType::COMMAND);
        $$->addTokenChild($2);  // Add IDENTIFIER token
        $$->addChild($4);  // Add the first value
        $$->addChild($6);  // Add the second value
        $$->addChild($8);  // Add commands
        printf("Parsed FOR command (TO)\n");
    }
    | FOR IDENTIFIER FROM value DOWNTO value DO commands ENDFOR {
        $$ = new Node(NodeType::COMMAND);
        $$->addTokenChild($2);  // Add IDENTIFIER token
        $$->addChild($4);  // Add the first value
        $$->addChild($6);  // Add the second value
        $$->addChild($8);  // Add commands
        printf("Parsed FOR command (DOWNTO)\n");
    }
    | proc_call T_SEMICOLON {
        $$ = new Node(NodeType::COMMAND);
        $$->addChild($1);  // Add procedure call
        printf("Parsed procedure call command\n");
    }
    | READ identifier T_SEMICOLON {
        $$ = new Node(NodeType::COMMAND);
        $$->addChild($2);  // Add IDENTIFIER token
        printf("Parsed READ command\n");
    }
    | WRITE value T_SEMICOLON {
        $$ = new Node(NodeType::COMMAND);
        $$->addChild($2);  // Add value
        printf("Parsed WRITE command\n");
    }
    ;

proc_head:
    IDENTIFIER T_LPAREN args_decl T_RPAREN {
        $$ = new Node(NodeType::PROC_HEAD);
        $$->addTokenChild($1);  // Add IDENTIFIER token
        $$->addChild($3);  // Add arguments declaration
        printf("Parsed procedure head\n");
    }
    ;

proc_call:
    IDENTIFIER T_LPAREN args T_RPAREN {
        $$ = new Node(NodeType::PROC_CALL);
        $$->addTokenChild($1);  // Add IDENTIFIER token
        $$->addChild($3);  // Add arguments
        printf("Parsed procedure call\n");
    }
    ;

declarations:
    declarations T_COMMA IDENTIFIER {
        $$ = new Node(NodeType::DECLARATIONS);
        $$->addChild($1);  // Add previous declarations
        $$->addTokenChild($3);  // Add IDENTIFIER token
        printf("Parsed declarations (multiple)\n");
    }
    | declarations T_COMMA IDENTIFIER T_LBRACKET NUMBER T_COLON NUMBER T_RBRACKET {
        $$ = new Node(NodeType::DECLARATIONS);
        $$->addChild($1);  // Add previous declarations
        $$->addTokenChild($3);  // Add IDENTIFIER token
        $$->addTokenChild($5);  // Add lower bound of array
        $$->addTokenChild($7);  // Add upper bound of array
        printf("Parsed declarations with array\n");
    }
    | IDENTIFIER {
        $$ = new Node(NodeType::DECLARATIONS);
        $$->addTokenChild($1);  // Add IDENTIFIER token
        printf("Parsed single declaration\n");
    }
    | IDENTIFIER T_LBRACKET NUMBER T_COLON NUMBER T_RBRACKET {
        $$ = new Node(NodeType::DECLARATIONS);
        $$->addTokenChild($1);  // Add IDENTIFIER token
        $$->addTokenChild($3);  // Add lower bound
        $$->addTokenChild($5);  // Add upper bound
        printf("Parsed single array declaration\n");
    }
    ;

args_decl:
    args_decl T_COMMA IDENTIFIER {
        $$ = new Node(NodeType::ARGS_DECL);
        $$->addChild($1);  // Add previous argument declaration
        $$->addTokenChild($3);  // Add IDENTIFIER token
        printf("Parsed arguments declaration (multiple)\n");
    }
    | args_decl T_COMMA T_TABLE IDENTIFIER {
        $$ = new Node(NodeType::ARGS_DECL);
        $$->addChild($1);  // Add previous argument declaration
        $$->addTokenChild($3);  // Add TABLE IDENTIFIER token
        printf("Parsed arguments declaration with table\n");
    }
    | IDENTIFIER {
        $$ = new Node(NodeType::ARGS_DECL);
        $$->addTokenChild($1);  // Add IDENTIFIER token
        printf("Parsed single argument declaration\n");
    }
    | T_TABLE IDENTIFIER {
        $$ = new Node(NodeType::ARGS_DECL);
        $$->addTokenChild($2);  // Add TABLE IDENTIFIER token
        printf("Parsed single table argument declaration\n");
    }
    ;

args:
    args T_COMMA IDENTIFIER {
        $$ = new Node(NodeType::ARGS);
        $$->addChild($1);  // Add previous arguments
        $$->addTokenChild($3);  // Add IDENTIFIER token
        printf("Parsed arguments (multiple)\n");
    }
    | IDENTIFIER {
        $$ = new Node(NodeType::ARGS);
        $$->addTokenChild($1);  // Add IDENTIFIER token
        printf("Parsed single argument\n");
    }
    ;

expression:
    value {
        $$ = new Node(NodeType::EXPRESSION);
        $$->addChild($1);  // Add value
        printf("Parsed expression (single value)\n");
    }
    | value T_PLUS value {
        $$ = new Node(NodeType::EXPRESSION);
        $$->addChild($1);  // Add first value
        $$->addChild($3);  // Add second value
        printf("Parsed expression (addition)\n");
    }
    | value T_MINUS value {
        $$ = new Node(NodeType::EXPRESSION);
        $$->addChild($1);  // Add first value
        $$->addChild($3);  // Add second value
        printf("Parsed expression (subtraction)\n");
    }
    | value T_MUL value {
        $$ = new Node(NodeType::EXPRESSION);
        $$->addChild($1);  // Add first value
        $$->addChild($3);  // Add second value
        printf("Parsed expression (multiplication)\n");
    }
    | value T_DIV value {
        $$ = new Node(NodeType::EXPRESSION);
        $$->addChild($1);  // Add first value
        $$->addChild($3);  // Add second value
        printf("Parsed expression (division)\n");
    }
    | value T_MOD value {
        $$ = new Node(NodeType::EXPRESSION);
        $$->addChild($1);  // Add first value
        $$->addChild($3);  // Add second value
        printf("Parsed expression (modulus)\n");
    }
    ;

condition:
    value T_EQ value {
        $$ = new Node(NodeType::CONDITION);
        $$->addChild($1);  // Add first value
        $$->addChild($3);  // Add second value
        printf("Parsed condition (equal)\n");
    }
    | value T_NEQ value {
        $$ = new Node(NodeType::CONDITION);
        $$->addChild($1);  // Add first value
        $$->addChild($3);  // Add second value
        printf("Parsed condition (not equal)\n");
    }
    | value T_GT value {
        $$ = new Node(NodeType::CONDITION);
        $$->addChild($1);  // Add first value
        $$->addChild($3);  // Add second value
        printf("Parsed condition (greater than)\n");
    }
    | value T_LT value {
        $$ = new Node(NodeType::CONDITION);
        $$->addChild($1);  // Add first value
        $$->addChild($3);  // Add second value
        printf("Parsed condition (less than)\n");
    }
    | value T_GTE value {
        $$ = new Node(NodeType::CONDITION);
        $$->addChild($1);  // Add first value
        $$->addChild($3);  // Add second value
        printf("Parsed condition (greater than or equal)\n");
    }
    | value T_LTE value {
        $$ = new Node(NodeType::CONDITION);
        $$->addChild($1);  // Add first value
        $$->addChild($3);  // Add second value
        printf("Parsed condition (less than or equal)\n");
    }
    ;

value:
    NUMBER {
        $$ = new Node(NodeType::VALUE);
        $$->addTokenChild($1);  // Add NUMBER token
        printf("Parsed value (number)\n");
    }
    | identifier {
        $$ = new Node(NodeType::VALUE);
        $$->addChild($1);  // Add IDENTIFIER token
        printf("Parsed value (identifier)\n");
    }
    ;

identifier:
    IDENTIFIER {
        $$ = new Node(NodeType::IDENTIFIER);
        $$->addTokenChild($1);  // Add IDENTIFIER token
        printf("Parsed identifier\n");
    }
    | IDENTIFIER T_LBRACKET IDENTIFIER T_RBRACKET {
        $$ = new Node(NodeType::IDENTIFIER);
        $$->addTokenChild($1);
        $$->addTokenChild($3);
        printf("Parsed array identifier (variable index)\n");
    }
    | IDENTIFIER T_LBRACKET NUMBER T_RBRACKET {
        $$ = new Node(NodeType::IDENTIFIER);
        $$->addTokenChild($1);
        $$->addTokenChild($3);
        printf("Parsed array identifier (number index)\n");
    }
    ;

%%

int yyerror(std::string s) {
    std::cout << s << " : ";
    if (yylval.token) {
        yylval.token->print();
    } else {
        printf("unexpected token\n");
    }

    return 1;
}
