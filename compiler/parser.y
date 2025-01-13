%{
#define YYDEBUG 1

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include "Token.hpp"
#include "Node.hpp"
#include "parser.tab.h"

extern FILE *yyin;
extern int yylex();
extern int yyparse();
int yyerror(std::string s);

std::vector<Token*> variable_vector;

// 0 is for the ACC
// 1-4 are for registers
int var_counter = 5;

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
        $$ = new ProgramAllNode();
        $$->addChild($1);  // Add procedures node
        $$->addChild($2);  // Add main node
        printf("Parsed program_all\n");
        $$->print();
        for (auto *i: variable_vector) {
            i->print();
        }
    }
    ;

procedures:
    procedures PROCEDURE proc_head IS declarations T_BEGIN commands END {
        $$ = new ProceduresNode();
        $$->addChild($1);  // Add previous procedures
        $$->addChild($3);  // Add proc_head
        $$->addChild($5);  // Add declarations
        $$->addChild($7);  // Add commands
        printf("Parsed procedures with declarations\n");
    }
    | procedures PROCEDURE proc_head IS T_BEGIN commands END {
        $$ = new ProceduresNode();
        $$->addChild($1);  // Add previous procedures
        $$->addChild($3);  // Add proc_head
        $$->addChild($6);  // Add commands
        printf("Parsed procedures without declarations\n");
    }
    | %empty {
        $$ = new ProceduresNode();
        printf("Parsed empty procedures\n");
    }
    ;

main:
    PROGRAM IS declarations T_BEGIN commands END {
        $$ = new MainNode();
        $$->addChild($3);  // Add declarations
        $$->addChild($5);  // Add commands
        printf("Parsed main with declarations\n");
    }
    | PROGRAM IS T_BEGIN commands END {
        $$ = new MainNode();
        $$->addChild($4);  // Add commands
        printf("Parsed main without declarations\n");
    }
    ;

commands:
    commands command {
        $$ = new CommandsNode();
        $$->addChild($1);  // Add previous commands
        $$->addChild($2);  // Add the current command
        printf("Parsed commands (multiple)\n");
    }
    | command {
        $$ = new CommandsNode();
        $$->addChild($1);  // Add the single command
        printf("Parsed command (single)\n");
    }
    | %empty {
        $$ = new CommandsNode();
        printf("Parsed empty command\n");
    }
    ;

command:
    identifier T_ASSIGN expression T_SEMICOLON {
        $$ = new AssignmentCommandNode();
        $$->addChild($1);  // Add IDENTIFIER token
        $$->addChild($3);  // Add the expression
        printf("Parsed assignment command\n");
    }
    | IF condition THEN commands ELSE commands ENDIF {
        $$ = new IfElseCommandNode();
        $$->addChild($2);  // Add condition
        $$->addChild($4);  // Add then commands
        $$->addChild($6);  // Add else commands
        printf("Parsed IF-ELSE command\n");
    }
    | IF condition THEN commands ENDIF {
        $$ = new IfCommandNode();
        $$->addChild($2);  // Add condition
        $$->addChild($4);  // Add commands
        printf("Parsed IF command\n");
    }
    | WHILE condition DO commands ENDWHILE {
        $$ = new WhileCommandNode();
        $$->addChild($2);  // Add condition
        $$->addChild($4);  // Add commands
        printf("Parsed WHILE command\n");
    }
    | REPEAT commands UNTIL condition T_SEMICOLON {
        $$ = new RepeatCommandNode();
        $$->addChild($2);  // Add commands
        $$->addChild($4);  // Add condition
        printf("Parsed REPEAT command\n");
    }
    | FOR IDENTIFIER FROM value TO value DO commands ENDFOR {
        $$ = new ForCommandNode($2); // Add IDENTIFIER token
        $$->addChild($4);  // Add the first value
        $$->addChild($6);  // Add the second value
        $$->addChild($8);  // Add commands
        printf("Parsed FOR command (TO)\n");
    }
    | FOR IDENTIFIER FROM value DOWNTO value DO commands ENDFOR {
        $$ = new ForCommandNode($2); // Add IDENTIFIER token
        $$->addChild($4);  // Add the first value
        $$->addChild($6);  // Add the second value
        $$->addChild($8);  // Add commands
        printf("Parsed FOR command (DOWNTO)\n");
    }
    | proc_call T_SEMICOLON {
        $$ = new ProcCallCommandNode();
        $$->addChild($1);  // Add procedure call
        printf("Parsed procedure call command\n");
    }
    | READ identifier T_SEMICOLON {
        $$ = new ReadCommandNode();
        $$->addChild($2);  // Add IDENTIFIER token
        printf("Parsed READ command\n");
    }
    | WRITE value T_SEMICOLON {
        $$ = new WriteCommandNode();
        $$->addChild($2);  // Add value
        printf("Parsed WRITE command\n");
    }
    ;

proc_head:
    IDENTIFIER T_LPAREN args_decl T_RPAREN {
        $$ = new ProcHeadNode($1); // Add IDENTIFIER token
        $$->addChild($3);  // Add arguments declaration
        printf("Parsed procedure head\n");
    }
    ;

proc_call:
    IDENTIFIER T_LPAREN args T_RPAREN {
        $$ = new ProcHeadNode($1); // Add IDENTIFIER token
        $$->addChild($3);  // Add arguments
        printf("Parsed procedure call\n");
    }
    ;

/* TODO : Add separate declarations to the procedures */
declarations:
    declarations T_COMMA IDENTIFIER {
        $$ = new DeclarationsNode($3); // Add IDENTIFIER token
        $$->addChild($1);  // Add previous declarations

        variable_vector.push_back($3);
        var_counter++;

        printf("Parsed declarations (multiple)\n");
    }
    | declarations T_COMMA IDENTIFIER T_LBRACKET NUMBER T_COLON NUMBER T_RBRACKET {
        $$ = new DeclarationsNode();
        $$->addChild($1);  // Add previous declarations
        // $$->addTokenChild($3);  // Add IDENTIFIER token
        // $$->addTokenChild($5);  // Add lower bound of array
        // $$->addTokenChild($7);  // Add upper bound of array

        variable_vector.push_back($3);
        var_counter++;

        printf("Parsed declarations with array\n");
    }
    | IDENTIFIER {
        $$ = new DeclarationsNode($1);

        variable_vector.push_back($1);
        var_counter++;

        printf("Parsed single declaration\n");
    }
    | IDENTIFIER T_LBRACKET NUMBER T_COLON NUMBER T_RBRACKET {
        $$ = new DeclarationsNode();
        // $$->addTokenChild($1);  // Add IDENTIFIER token
        // $$->addTokenChild($3);  // Add lower bound
        // $$->addTokenChild($5);  // Add upper bound

        variable_vector.push_back($1);
        var_counter++;

        printf("Parsed single array declaration\n");
    }
    ;

args_decl:
    args_decl T_COMMA IDENTIFIER {
        $$ = new ArgsDeclNode($3);  // Add IDENTIFIER token
        $$->addChild($1);  // Add previous argument declaration
        printf("Parsed arguments declaration (multiple)\n");
    }
    | args_decl T_COMMA T_TABLE IDENTIFIER {
        $$ = new ArgsDeclNode($3);  // Add IDENTIFIER token
        $$->addChild($1);  // Add previous argument declaration
        printf("Parsed arguments declaration with table\n");
    }
    | IDENTIFIER {
        $$ = new ArgsDeclNode($1); // Add IDENTIFIER token
        printf("Parsed single argument declaration\n");
    }
    | T_TABLE IDENTIFIER {
        $$ = new ArgsDeclNode($2); // Add IDENTIFIER token
        printf("Parsed single table argument declaration\n");
    }
    ;

args:
    args T_COMMA IDENTIFIER {
        $$ = new ArgsNode($3); // Add IDENTIFIER token
        $$->addChild($1);  // Add previous arguments
        printf("Parsed arguments (multiple)\n");
    }
    | IDENTIFIER {
        $$ = new ArgsNode($1); // Add IDENTIFIER token
        printf("Parsed single argument\n");
    }
    ;

expression:
    value {
        $$ = new ExpressionNode();
        $$->addChild($1);  // Add value
        printf("Parsed expression (single value)\n");
    }
    | value T_PLUS value {
        $$ = new ExpressionNode($2);
        $$->addChild($1);  // Add first value
        $$->addChild($3);  // Add second value
        printf("Parsed expression (addition)\n");
    }
    | value T_MINUS value {
        $$ = new ExpressionNode($2);
        $$->addChild($1);  // Add first value
        $$->addChild($3);  // Add second value
        printf("Parsed expression (subtraction)\n");
    }
    | value T_MUL value {
        $$ = new ExpressionNode($2);
        $$->addChild($1);  // Add first value
        $$->addChild($3);  // Add second value
        printf("Parsed expression (multiplication)\n");
    }
    | value T_DIV value {
        $$ = new ExpressionNode($2);
        $$->addChild($1);  // Add first value
        $$->addChild($3);  // Add second value
        printf("Parsed expression (division)\n");
    }
    | value T_MOD value {
        $$ = new ExpressionNode($2);
        $$->addChild($1);  // Add first value
        $$->addChild($3);  // Add second value
        printf("Parsed expression (modulus)\n");
    }
    ;

condition:
    value T_EQ value {
        $$ = new ConditionNode($2);
        $$->addChild($1);  // Add first value
        $$->addChild($3);  // Add second value
        printf("Parsed condition (equal)\n");
    }
    | value T_NEQ value {
        $$ = new ConditionNode($2);
        $$->addChild($1);  // Add first value
        $$->addChild($3);  // Add second value
        printf("Parsed condition (not equal)\n");
    }
    | value T_GT value {
        $$ = new ConditionNode($2);
        $$->addChild($1);  // Add first value
        $$->addChild($3);  // Add second value
        printf("Parsed condition (greater than)\n");
    }
    | value T_LT value {
        $$ = new ConditionNode($2);
        $$->addChild($1);  // Add first value
        $$->addChild($3);  // Add second value
        printf("Parsed condition (less than)\n");
    }
    | value T_GTE value {
        $$ = new ConditionNode($2);
        $$->addChild($1);  // Add first value
        $$->addChild($3);  // Add second value
        printf("Parsed condition (greater than or equal)\n");
    }
    | value T_LTE value {
        $$ = new ConditionNode($2);
        $$->addChild($1);  // Add first value
        $$->addChild($3);  // Add second value
        printf("Parsed condition (less than or equal)\n");
    }
    ;

value:
    NUMBER {
        $$ = new ValueNode($1); // Add NUMBER token
        printf("Parsed value (number)\n");
    }
    | identifier {
        $$ = new ValueNode();
        $$->addChild($1);  // Add IDENTIFIER token
        printf("Parsed value (identifier)\n");
    }
    ;

identifier:
    IDENTIFIER {
        $$ = new IdentifierNode($1);
        printf("Parsed identifier\n");
    }
    | IDENTIFIER T_LBRACKET IDENTIFIER T_RBRACKET {
        $$ = new IdentifierNode();
        // $$->addTokenChild($1);
        // $$->addTokenChild($3);
        printf("Parsed array identifier (variable index)\n");
    }
    | IDENTIFIER T_LBRACKET NUMBER T_RBRACKET {
        $$ = new IdentifierNode();
        // $$->addTokenChild($1);
        // $$->addTokenChild($3);
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
