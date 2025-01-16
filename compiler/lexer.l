
%option yylineno
%option noyywrap

%{
#include "Token.hpp"
#include "Node.hpp"
#include "parser.tab.h"
#include <iostream>
#include <string>
#include <cstdlib>

extern YYSTYPE yylval;
#define YYSTYPE *Token

%}

%%

^#(.|\\\n)*\n              { /* Ignore comments */; }

"PROGRAM"                  { yylval.token = new Token(TokenType::PROGRAM, "PROGRAM", yylineno, 0); return PROGRAM; }
"PROCEDURE"                { yylval.token = new Token(TokenType::PROCEDURE, "PROCEDURE", yylineno, 0); return PROCEDURE; }
"IS"                       { yylval.token = new Token(TokenType::IS, "IS", yylineno, 0); return IS; }
"BEGIN"                    { yylval.token = new Token(TokenType::T_BEGIN, "BEGIN", yylineno, 0); return T_BEGIN; }
"END"                      { yylval.token = new Token(TokenType::END, "END", yylineno, 0); return END; }
"IF"                       { yylval.token = new Token(TokenType::IF, "IF", yylineno, 0); return IF; }
"THEN"                     { yylval.token = new Token(TokenType::THEN, "THEN", yylineno, 0); return THEN; }
"ELSE"                     { yylval.token = new Token(TokenType::ELSE, "ELSE", yylineno, 0); return ELSE; }
"ENDIF"                    { yylval.token = new Token(TokenType::ENDIF, "ENDIF", yylineno, 0); return ENDIF; }
"WHILE"                    { yylval.token = new Token(TokenType::WHILE, "WHILE", yylineno, 0); return WHILE; }
"DO"                       { yylval.token = new Token(TokenType::DO, "DO", yylineno, 0); return DO; }
"ENDWHILE"                 { yylval.token = new Token(TokenType::ENDWHILE, "ENDWHILE", yylineno, 0); return ENDWHILE; }
"REPEAT"                   { yylval.token = new Token(TokenType::REPEAT, "REPEAT", yylineno, 0); return REPEAT; }
"UNTIL"                    { yylval.token = new Token(TokenType::UNTIL, "UNTIL", yylineno, 0); return UNTIL; }
"FOR"                      { yylval.token = new Token(TokenType::FOR, "FOR", yylineno, 0); return FOR; }
"ENDFOR"                   { yylval.token = new Token(TokenType::ENDFOR, "ENDFOR", yylineno, 0); return ENDFOR; }
"FROM"                     { yylval.token = new Token(TokenType::FROM, "FROM", yylineno, 0); return FROM; }
"TO"                       { yylval.token = new Token(TokenType::TO, "TO", yylineno, 0); return TO; }
"DOWNTO"                   { yylval.token = new Token(TokenType::DOWNTO, "DOWNTO", yylineno, 0); return DOWNTO; }
"READ"                     { yylval.token = new Token(TokenType::READ, "READ", yylineno, 0); return READ; }
"WRITE"                    { yylval.token = new Token(TokenType::WRITE, "WRITE", yylineno, 0); return WRITE; }

":="                       { yylval.token = new Token(TokenType::T_ASSIGN, ":=", yylineno, 0); return T_ASSIGN; }
"+"                        { yylval.token = new Token(TokenType::T_PLUS, "+", yylineno, 0); return T_PLUS; }
"-"                        { yylval.token = new Token(TokenType::T_MINUS, "-", yylineno, 0); return T_MINUS; }
"*"                        { yylval.token = new Token(TokenType::T_MUL, "*", yylineno, 0); return T_MUL; }
"/"                        { yylval.token = new Token(TokenType::T_DIV, "/", yylineno, 0); return T_DIV; }
"%"                        { yylval.token = new Token(TokenType::T_MOD, "%", yylineno, 0); return T_MOD; }
","                        { yylval.token = new Token(TokenType::T_COMMA, ",", yylineno, 0); return T_COMMA; }

"="                        { yylval.token = new Token(TokenType::T_EQ, "=", yylineno, 0); return T_EQ; }
"!="                       { yylval.token = new Token(TokenType::T_NEQ, "!=", yylineno, 0); return T_NEQ; }
">"                        { yylval.token = new Token(TokenType::T_GT, ">", yylineno, 0); return T_GT; }
"<"                        { yylval.token = new Token(TokenType::T_LT, "<", yylineno, 0); return T_LT; }
">="                       { yylval.token = new Token(TokenType::T_GTE, ">=", yylineno, 0); return T_GTE; }
"<="                       { yylval.token = new Token(TokenType::T_LTE, "<=", yylineno, 0); return T_LTE; }

";"                        { yylval.token = new Token(TokenType::T_SEMICOLON, ";", yylineno, 0); return T_SEMICOLON; }
":"                        { yylval.token = new Token(TokenType::T_COLON, ":", yylineno, 0); return T_COLON; }
"T"                        { yylval.token = new Token(TokenType::T_TABLE, "T", yylineno, 0); return T_TABLE; }
"("                        { yylval.token = new Token(TokenType::T_LPAREN, "(", yylineno, 0); return T_LPAREN; }
")"                        { yylval.token = new Token(TokenType::T_RPAREN, ")", yylineno, 0); return T_RPAREN; }
"["                        { yylval.token = new Token(TokenType::T_LBRACKET, "[", yylineno, 0); return T_LBRACKET; }
"]"                        { yylval.token = new Token(TokenType::T_RBRACKET, "]", yylineno, 0); return T_RBRACKET; }

-*[0-9]+                     { yylval.token = new Token(TokenType::NUMBER, yytext, yylineno, 0); return NUMBER; }

[_a-z]+                    { yylval.token = new Token(TokenType::IDENTIFIER, yytext, yylineno, 0); return IDENTIFIER; }

[ \t\r\n]+                 { /* Ignore whitespace */; }

.                          { std::cerr << "Unexpected character: " << yytext << " at line " << yylineno << std::endl; }


%%
