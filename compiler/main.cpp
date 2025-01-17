#include "Token.hpp"
#include "Node.hpp"
#include <iostream>
#include <cstdio>
#include <cstring>
#include <filesystem>
#include "parser.tab.h"
#include "lex.yy.h"

std::string parsedFileName;

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <source-file>" << std::endl;
        return 1;
    }

    // TODO : add second arg as outputfile name
    std::filesystem::path path(argv[1]);
    parsedFileName = path.filename().string();

    std::cout << "Parsed file name: " << parsedFileName << std::endl;

    FILE* file = fopen(argv[1], "r");
    if (!file) {
        std::cerr << "Error opening file: " << argv[1] << std::endl;
        return 1;
    }

    yyin = file;

    if (argc >= 3 && strcmp(argv[2], "-t") == 0) {
        while (yylex()) {
            yylval.token->print();
        }
    } else {
        yyparse();
    }

    fclose(file);

    return 0;
}
