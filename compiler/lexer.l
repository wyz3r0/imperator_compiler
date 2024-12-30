%{
#include "parser.tab.h"
#include <stdlib.h>
#include <string.h>
%}

%%

^#(.|\\\n)*\n  { ; } // Ignore comments

"PROGRAM"      { return PROGRAM; }
"PROCEDURE"    { return PROCEDURE; }
"IS"           { return IS; }
"BEGIN"        { return T_BEGIN; }
"END"          { return END; }
"IF"           { return IF; }
"THEN"         { return THEN; }
"ELSE"         { return ELSE; }
"ENDIF"        { return ENDIF; }
"WHILE"        { return WHILE; }
"DO"           { return DO; }
"ENDWHILE"     { return ENDWHILE; }
"REPEAT"       { return REPEAT; }
"UNTIL"        { return UNTIL; }
"FOR"          { return FOR; }
"ENDFOR"       { return ENDFOR; }
"FROM"         { return FROM; }
"TO"           { return TO; }
"DOWNTO"       { return DOWNTO; }
"READ"         { return READ; }
"WRITE"        { return WRITE; }
":="           { return T_ASSIGN; }
"\+"           { return T_PLUS; }
"-"            { return T_MINUS; }
"\*"           { return T_MUL; }
"/"            { return T_DIV; }
"%"            { return T_MOD; }
"="            { return T_EQ; }
"!="           { return T_NEQ; }
">"            { return T_GT; }
"<"            { return T_LT; }
">="           { return T_GTE; }
"<="           { return T_LTE; }

[0-9]+          { yylval.number = atoi(yytext); return NUMBER; }
[_a-z]+         { yylval.string = strdup(yytext); return IDENTIFIER; }
[ \t\n]+        ; // Ignore whitespace
.               { return yytext[0]; } // Return single characters

%%

int yywrap() {
    return 1;
}
