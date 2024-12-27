%{
#include "parser.tab.h"
%}

DIGIT      [0-9]
LETTER     [_a-z]
ID         {LETTER}({LETTER}|{DIGIT})*
NUM        {DIGIT}+

%%

"PROGRAM"            { return PROGRAM_TOKEN; }
"IS"                 { return IS_TOKEN; }
"BEGIN"              { return BEGIN_TOKEN; }
"END"                { return END_TOKEN; }
"IF"                 { return IF_TOKEN; }
"THEN"               { return THEN_TOKEN; }
"ELSE"               { return ELSE_TOKEN; }
"ENDIF"              { return ENDIF_TOKEN; }
"FOR"                { return FOR_TOKEN; }
"TO"                 { return TO_TOKEN; }
"DOWNTO"             { return DOWNTO_TOKEN; }
"DO"                 { return DO_TOKEN; }
"ENDFOR"             { return ENDFOR_TOKEN; }
"REPEAT"             { return REPEAT_TOKEN; }
"UNTIL"              { return UNTIL_TOKEN; }
"PROCEDURE"          { return PROCEDURE_TOKEN; }
"READ"               { return READ_TOKEN; }
"WRITE"              { return WRITE_TOKEN; }
"HALT"               { return HALT_TOKEN; }
"="                  { return EQ_TOKEN; }
"!="                 { return NEQ_TOKEN; }
">"                  { return GT_TOKEN; }
"<"                  { return LT_TOKEN; }
">="                 { return GEQ_TOKEN; }
"<="                 { return LEQ_TOKEN; }
":"                  { return COLON_TOKEN; }
";"                  { return SEMICOLON_TOKEN; }
","                  { return COMMA_TOKEN; }
"["                  { return LBRACKET_TOKEN; }
"]"                  { return RBRACKET_TOKEN; }
"("                  { return LPAREN_TOKEN; }
")"                  { return RPAREN_TOKEN; }
"+"                  { return PLUS_TOKEN; }
"-"                  { return MINUS_TOKEN; }
"*"                  { return TIMES_TOKEN; }
"/"                  { return DIV_TOKEN; }
"%"                  { return MOD_TOKEN; }
{ID}                 { yylval.sval = strdup(yytext); return PIDENTIFIER_TOKEN; }
{NUM}                { yylval.ival = atoi(yytext); return NUM_TOKEN; }
[ \t\n]+             ; /* Ignore whitespace */
"#".*                ; /* Ignore comments */
.                    { return yytext[0]; }

%%

int yywrap() {
    return 1;
}
