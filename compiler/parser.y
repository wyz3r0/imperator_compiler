%{
#define YYDEBUG 1

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include "Token.hpp"
#include "Node.hpp"
#include "postprocessing.hpp"
#include "parser.tab.h"
#include "ErrorHandler.hpp"

extern const std::string parsedFileName;
extern FILE *yyin;
extern int yylex();
extern int yyparse();
int yyerror(std::string s);

/*
    REGISTERS:
        R0 - ACC
        R1 - temp var 1
        R2 - temp var 2
        R3 - temp var 3
        R4 - return val
        R5 - 0 (false)
        R6 - 1 (true)
        R7 - temp var 4
        R8 - temp var 5
*/

std::vector<Token*> tokens;

long long var_counter = 9;
long long proc_counter = 0;
long long condition_counter = 0;
long long command_counter = 0;
long long expression_counter = 0;


Token* manageToken(Token* newToken) {
    if (proc_counter != -1 && newToken->getFunction() != TokenFunction::PROC && newToken->getType() == TokenType::IDENTIFIER) {
        newToken->setAssignability(true)->setValue(std::to_string(proc_counter) + "-" + newToken->getValue());
    }

    auto it = std::find_if(tokens.begin(), tokens.end(),
                           [&newToken](Token* token) { return token->getValue() == newToken->getValue(); });

    if (it != tokens.end()) {
        return *it;
    }

    newToken->setAddress(var_counter);
    tokens.push_back(newToken);
    var_counter++;

    return newToken;
}

Token* manageTabel(Token* identifier, Token* lower_bound, Token* upper_bound) {
    if (std::stoll(lower_bound->getValue()) > std::stoll(upper_bound->getValue())) {
        LOG_ERROR("Lower bound is greater than upper bound", identifier);
    }

    if (proc_counter != -1 && identifier->getFunction() != TokenFunction::PROC && identifier->getType() == TokenType::IDENTIFIER) {
        identifier->setAssignability(true)->setValue(std::to_string(proc_counter) + "-" + identifier->getValue());
    }

    identifier->setAddress(var_counter-std::stoll(lower_bound->getValue()));    // Set absolute address of 0th index
    tokens.push_back(identifier);                                               // Add identifier to the tokens withh 0th index's address

    var_counter += std::stoll(upper_bound->getValue()) - std::stoll(lower_bound->getValue()) + 1;

    return identifier;
}

bool saveToFile(const std::string& content) {
    std::string outputFileName = parsedFileName + ".mr";
    std::ofstream outFile(outputFileName);
    if (outFile.is_open()) {
        outFile << content;
        outFile.close();
        return true;
    } else {
        return false;
    }
}

void vibecheck(){
    if (!ErrorHandler::getInstance().getErrors().empty()) {
        ErrorHandler::getInstance().printErrors();
        exit(1);
    }
}

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

%type <node> procedures main commands command proc_head proc_call declarations args_decl args expression condition value identifier number

%start program_all

%%

program_all:
    {   // INIT
        // Reserve addresses 5 and 6 for bools.
        tokens.push_back(new Token(TokenType::NUMBER, "0", 0, 0, 5, false));
        tokens.push_back(new Token(TokenType::NUMBER, "1", 0, 0, 6, false));
    }
    procedures { proc_counter = -1; } main {
        vibecheck();

        Node* AST = new ProgramAllNode();
        AST->addChild($2);  // Add procedures node
        AST->addChild($4);  // Add main node
        printf("Parsed program_all\n");

        vibecheck();

        AST->print();
        for (auto token : tokens) {
            token->print();
        }

        // Build assembly.
        std::string assembly = AST->build(&tokens);
        vibecheck();
        std::cout << "First pass assembly:" << std::endl << assembly << std::endl;

        assembly = calculate_jumps(assembly);
        vibecheck();
        std::cout << "Assembly with calculated jumps:" << std::endl << assembly << std::endl;

        delete AST;

        if (!saveToFile(assembly)) {
            std::cout << "FATAL COMPILATION ERROR" << std::endl;
        }
    }
    ;

/* TODO : seg fault for over 2 procedures */
procedures:
    procedures PROCEDURE proc_head IS declarations T_BEGIN commands END {
        $$ = new ProceduresNode($2, proc_counter);
        $$->addChild($1);  // Add previous procedures
        $$->addChild($3);  // Add proc_head
        $$->addChild($7);  // Add commands
        $$->addChild($5);  // Add declarations
        proc_counter++;
        printf("Parsed procedures with declarations\n");
    }
    | procedures PROCEDURE proc_head IS T_BEGIN commands END {
        $$ = new ProceduresNode($2, proc_counter);
        $$->addChild($1);  // Add previous procedures
        $$->addChild($3);  // Add proc_head
        $$->addChild($6);  // Add commands
        proc_counter++;
        printf("Parsed procedures without declarations\n");
    }
    | %empty {
        $$ = new ProceduresNode();
        printf("Parsed empty procedures\n");
    }
    ;

proc_head:
    IDENTIFIER T_LPAREN args_decl T_RPAREN {
        $$ = new ProcHeadNode(manageToken($1->setFunction(TokenFunction::PROC))); // Add IDENTIFIER token
        $$->addChild($3);  // Add arguments declaration
        printf("Parsed procedure head\n");
    }
    ;

proc_call:
    IDENTIFIER T_LPAREN args T_RPAREN {
        $$ = new ProcCallNode(manageToken($1->setFunction(TokenFunction::PROC))); // Add IDENTIFIER token
        $$->addChild($3);  // Add arguments
        printf("Parsed procedure call\n");
    }
    ;

args_decl:
    args_decl T_COMMA IDENTIFIER {
        $$ = new ArgsDeclNode(manageToken($3->setFunction(TokenFunction::ARG)));  // Add IDENTIFIER token
        $$->addChild($1);  // Add previous argument declaration
        printf("Parsed arguments declaration (multiple)\n");
    }
    | args_decl T_COMMA T_TABLE IDENTIFIER {
        $$ = new ArgsDeclNode(manageToken($4->setFunction(TokenFunction::T_ARG)));  // Add IDENTIFIER token
        $$->addChild($1);  // Add previous argument declaration
        printf("Parsed arguments declaration with table\n");
    }
    | IDENTIFIER {
        $$ = new ArgsDeclNode(manageToken($1->setFunction(TokenFunction::ARG))); // Add IDENTIFIER token
        printf("Parsed single argument declaration\n");
    }
    | T_TABLE IDENTIFIER {
        $$ = new ArgsDeclNode(manageToken($2->setFunction(TokenFunction::T_ARG))); // Add IDENTIFIER token
        printf("Parsed single table argument declaration\n");
    }
    ;

args:
    args T_COMMA IDENTIFIER {
        $$ = new ArgsNode(manageToken($3)); // Add IDENTIFIER token
        $$->addChild($1);  // Add previous arguments
        printf("Parsed arguments (multiple)\n");
    }
    | IDENTIFIER {
        $$ = new ArgsNode(manageToken($1)); // Add IDENTIFIER token
        printf("Parsed single argument\n");
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
        // TODO: check if the identifier is reassignable
        $$ = new AssignmentCommandNode($2, command_counter++);
        $$->addChild($1);  // Add IDENTIFIER token
        $$->addChild($3);  // Add the expression
        printf("Parsed assignment command\n");
    }
    | IF condition THEN commands ELSE commands ENDIF {
        $$ = new IfElseCommandNode($1, command_counter++);
        $$->addChild($2);  // Add condition
        $$->addChild($4);  // Add then commands
        $$->addChild($6);  // Add else commands
        printf("Parsed IF-ELSE command\n");
    }
    | IF condition THEN commands ENDIF {
        $$ = new IfCommandNode($1, command_counter++);
        $$->addChild($2);  // Add condition
        $$->addChild($4);  // Add commands
        printf("Parsed IF command\n");
    }
    | WHILE condition DO commands ENDWHILE {
        $$ = new WhileCommandNode($1, command_counter++);
        $$->addChild($2);  // Add condition
        $$->addChild($4);  // Add commands
        printf("Parsed WHILE command\n");
    }
    | REPEAT commands UNTIL condition T_SEMICOLON {
        $$ = new RepeatCommandNode($1, command_counter++);
        $$->addChild($2);  // Add commands
        $$->addChild($4);  // Add condition
        printf("Parsed REPEAT command\n");
    }
    | FOR IDENTIFIER FROM value TO value DO commands ENDFOR {
        // TODO: error if identifier has the same value as initialized variable
        $$ = new ForToCommandNode(manageToken($2), command_counter++); // Add IDENTIFIER token
        $$->addChild($4);  // Add the first value
        $$->addChild($6);  // Add the second value
        $$->addChild($8);  // Add commands
        printf("Parsed FOR command (TO)\n");
    }
    | FOR IDENTIFIER FROM value DOWNTO value DO commands ENDFOR {
        // TODO: error if identifier has the same value as initialized variable
        $$ = new ForDownToCommandNode(manageToken($2), command_counter++); // Add IDENTIFIER token
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

declarations:
    declarations T_COMMA IDENTIFIER {
        $$ = new DeclarationsNode(manageToken($3->setAssignability(true)));  // Add IDENTIFIER token
        $$->addChild($1);               // Add previous declarations

        printf("Parsed declarations (multiple)\n");
    }
    | declarations T_COMMA IDENTIFIER T_LBRACKET number T_COLON number T_RBRACKET {
        Token* lower_bound = $5->token;
        Token* upper_bound = $7->token;
        $$ = new DeclarationsNode(manageTabel($3->setFunction(TokenFunction::TABLE), lower_bound, upper_bound));
        $$->addChild($1);  // Add previous declarations

        printf("Parsed declarations with array\n");
    }
    | IDENTIFIER {
        $$ = new DeclarationsNode(manageToken($1->setAssignability(true)));  // Add IDENTIFIER token

        printf("Parsed single declaration\n");
    }
    | IDENTIFIER T_LBRACKET number T_COLON number T_RBRACKET {
        Token* lower_bound = $3->token;
        Token* upper_bound = $5->token;
        $$ = new DeclarationsNode(manageTabel($1->setFunction(TokenFunction::TABLE), lower_bound, upper_bound));

        printf("Parsed single array declaration\n");
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
        $$ = new ExpressionNode($2, expression_counter++);
        $$->addChild($1);  // Add first value
        $$->addChild($3);  // Add second value
        printf("Parsed expression (division)\n");
    }
    | value T_MOD value {
        $$ = new ExpressionNode($2, expression_counter++);
        $$->addChild($1);  // Add first value
        $$->addChild($3);  // Add second value
        printf("Parsed expression (modulus)\n");
    }
    ;

condition:
    value T_EQ value {
        $$ = new ConditionNode($2, condition_counter++);
        $$->addChild($1);  // Add first value
        $$->addChild($3);  // Add second value
        printf("Parsed condition (equal)\n");
    }
    | value T_NEQ value {
        $$ = new ConditionNode($2, condition_counter++);
        $$->addChild($1);  // Add first value
        $$->addChild($3);  // Add second value
        printf("Parsed condition (not equal)\n");
    }
    | value T_GT value {
        $$ = new ConditionNode($2, condition_counter++);
        $$->addChild($1);  // Add first value
        $$->addChild($3);  // Add second value
        printf("Parsed condition (greater than)\n");
    }
    | value T_LT value {
        $$ = new ConditionNode($2, condition_counter++);
        $$->addChild($1);  // Add first value
        $$->addChild($3);  // Add second value
        printf("Parsed condition (less than)\n");
    }
    | value T_GTE value {
        $$ = new ConditionNode($2, condition_counter++);
        $$->addChild($1);  // Add first value
        $$->addChild($3);  // Add second value
        printf("Parsed condition (greater than or equal)\n");
    }
    | value T_LTE value {
        $$ = new ConditionNode($2, condition_counter++);
        $$->addChild($1);  // Add first value
        $$->addChild($3);  // Add second value
        printf("Parsed condition (less than or equal)\n");
    }
    ;

value:
    number {
        $$ = new ValueNode();   // Add NUMBER token
        $$->addChild($1);
        printf("Parsed value (number)\n");
    }
    | identifier {
        $$ = new ValueNode();
        $$->addChild($1);       // Add IDENTIFIER token
        printf("Parsed value (identifier)\n");
    }
    ;

number:
    NUMBER {
        $1->setAssignability(false);
        $$ = new NumberNode(manageToken($1)); // Add NUMBER token
        printf("Parsed number\n");
    }
    | T_MINUS NUMBER {
        Token* negative_number = new Token(TokenType::NUMBER, $1->getValue() + $2->getValue(), $1->getLine(), $2->getColumn(), -1, false);
        // Delete unnecessary tokens
        delete $1;
        $1 = nullptr;
        delete $2;
        $2 = nullptr;
        $$ = new NumberNode(manageToken(negative_number)); // Add NUMBER token
        printf("Parsed negative number\n");
    }

identifier:
    IDENTIFIER {
        $$ = new IdentifierNode(manageToken($1));
        printf("Parsed identifier\n");
    }
    | IDENTIFIER T_LBRACKET IDENTIFIER T_RBRACKET {
        Token* index0 = manageToken($1);
        $$ = new TableNode(index0);
        $$->addChild(new IdentifierNode(manageToken($3)));
        printf("Parsed array identifier (variable index)\n");
    }
    | IDENTIFIER T_LBRACKET number T_RBRACKET {
        Token* index0 = manageToken($1);
        $$ = new TableNode(index0);
        $$->addChild($3);
        printf("Parsed array identifier (number index)\n");
    }
    ;

%%

int yyerror(std::string s) {
    LOG_ERROR(s, nullptr);
    vibecheck();
    return 1;
}
