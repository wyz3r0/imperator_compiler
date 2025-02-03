# Imperator Compiler

A .imp compiler project for JFTT 2024 at Wrocław University of Science and Technology.

![C++](https://img.shields.io/badge/language-C%2B%2B-blue)
![Flex](https://img.shields.io/badge/language-Flex-yellow)
![Bison](https://img.shields.io/badge/language-Bison-green)

## Table of Contents

- [Imperator Compiler](#imperator-compiler)
  - [Table of Contents](#table-of-contents)
  - [Introduction](#introduction)
  - [Features](#features)
  - [Installation](#installation)
  - [Usage](#usage)
  - [Example](#example)
  - [File Structure](#file-structure)
  - [License](#license)
  - [Authors](#authors)

## Introduction

The Imperator Compiler is designed to compile `.imp` files into machine-readable `.mr` files. This project is part of the JFTT 2024 course at Wrocław University of Science and Technology.

## Features

- Supports a wide range of control structures including loops and conditionals.
- Handles procedures and nested procedures (recursion is not supported).
- Supports arrays and basic arithmetic operations.
- Provides detailed error handling and reporting.

## Installation

To install and build the compiler, you need to have `g++ (C++20)`, `flex`, and `bison` installed on your system.

1. Clone from github:
    ```sh
    git clone https://github.com/yourusername/imperator-compiler.git
    cd compiler
    make
    ```

2. Build from zip:
    ```sh
    unzip *index_number*.zip
    cd compiler
    make
    ```

## Usage

To compile a `.imp` file, use the following command:

```sh
./compiler <source-file> <output-file> [-t]
```

- `<source-file>`: The input `.imp` file to be compiled.
- `<output-file>`: The output `.mr` file.
- `-t`: Optional flag to print tokens.

## Example

```sh
./compiler input.imp output.mr
```

## File Structure

- `compiler/`: Contains the source code for the compiler.
  - `Token.hpp`: Defines the `Token` class and related enums.
  - `Node.hpp`: Defines the `Node` class and its derived classes for AST.
  - `postprocessing.hpp`: Contains functions for post-processing the generated assembly code.
  - `preprocessing.hpp`: Contains functions for pre-processing the source code.
  - `parser.y`: Bison file for parsing the `.imp` source code.
  - `lexer.l`: Flex file for lexical analysis of the `.imp` source code.
  - `main.cpp`: The main entry point for the compiler.
  - `Makefile`: Build script for the compiler.
- `.gitignore`: Gitignore file.
- `labor4.pdf`: Specyfication in polish by [dr Maciej Gębala](https://cs.pwr.edu.pl/gebala/).
- `labor4.zip`: VM source code and examples by [dr Maciej Gębala](https://cs.pwr.edu.pl/gebala/).
- `LICENSE`: LICENSE
- `README.md`: This README file.

## License

This project is licensed under the MIT License. See the [LICENSE](LICENSE) file for details.

## Authors

- **Author**: [Dobrosław Dębicki](https://github.com/wyz3r0)
- **Professor**: [dr Maciej Gębala](https://cs.pwr.edu.pl/gebala/)

