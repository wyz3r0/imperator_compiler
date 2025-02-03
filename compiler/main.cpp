#include "Token.hpp"
#include "Node.hpp"
#include <iostream>
#include <cstdio>
#include <cstring>
#include <filesystem>
#include "parser.tab.h"
#include "lex.yy.h"

std::string parsedFileName;
std::string outputFileName;

int main(int argc, char* argv[]) {
    if (argc < 3) {
        std::cerr << "Usage: " << argv[0] << " <source-file> <output-file> [-t]" << std::endl;
        return 1;
    }

    std::filesystem::path path(argv[1]);
    if (path.extension() != ".imp") {
        std::cerr << "Error: Input file must have a .imp extension" << std::endl;
        return 1;
    }

    parsedFileName = path.filename().string();
    outputFileName = argv[2];

    std::filesystem::path outputPath(outputFileName);
    if (outputPath.extension() != ".mr") {
        outputFileName += ".mr";
    }

    bool printTokens = (argc >= 4 && strcmp(argv[3], "-t") == 0);

    std::cout << "Parsed file name: " << parsedFileName << std::endl;
    std::cout << "Output file name: " << outputFileName << std::endl;

    FILE* file = fopen(argv[1], "r");
    if (!file) {
        std::cerr << "Error opening file: " << argv[1] << std::endl;
        return 1;
    }

    yyin = file;

    if (printTokens) {
        while (yylex()) {
            yylval.token->print();
        }
    } else {
        yyparse();
    }

    fclose(file);

    return 0;
}
