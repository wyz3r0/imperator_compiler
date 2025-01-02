#include "Token.hpp"  // Include your Token class definition
#include <iostream>
#include <cstdio>
#include "parser.tab.h"
#include "lex.yy.h"


int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <source-file>" << std::endl;
        return 1;
    }

    FILE* file = fopen(argv[1], "r");
    if (!file) {
        std::cerr << "Error opening file: " << argv[1] << std::endl;
        return 1;
    }

    yyin = file;

    if (argc >=3 && strcmp(argv[2],"-l")==0) {
        // Write all the tokens
        while (yylex()) {
            yylval.token->print();
        }
    }
    else {
        yyparse();
    }

    fclose(file);

    return 0;
}
