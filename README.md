Tips: 可以在右边的 [Release 页面](https://github.com/Choimoe/CompilerDesignStarter/releases/tag/2024) 找到可以通过的代码。

# CompilerDesignStarter

A simple and modular C++17-based compiler framework designed to handle lexical analysis, parsing, intermediate code generation, and object code generation.

## Overview

This project provides a basic yet extendable framework for a compiler frontend, focusing on the following phases of compilation:

1. **Lexical Analysis (Lexer)**: Tokenizes the input source code into meaningful components like keywords, identifiers, numbers, and operators.
2. **Parsing (Parser)**: Implements LR(1) parsing to analyze the syntax and generate intermediate code in the form of quadruples.
3. **Object Code Generation (ObjectCodeGen)**: Converts intermediate code into object code, simulating the low-level machine code generation process.

The goal is to provide a lightweight and understandable implementation of a basic compiler, while being modular and easy to extend for further learning or development.

## Project Structure

```
.
├── build.sh          # Build script to compile the project
├── expression.h      # Handles expression representation
├── lexer.cpp         # Lexical analyzer implementation
├── lexer.h           # Lexical analyzer header
├── main.cpp          # Main entry point for the compiler
├── objectGen.cpp     # Generates object code from intermediate code
├── objectGen.h       # Object code generation header
├── objectStruct.h    # Data structures for objects and memory management
├── opkeymap.h        # Mapping operators and keywords for the lexer
├── parser.cpp        # Parsing logic implementation
├── parser.h          # Parser header
├── parserStruct.h    # Data structures used in parsing
├── parserUtil.cpp    # Helper functions for parser operations
├── parserUtil.h      # Parser utility functions header
└── README.md         # Project documentation
```

## Build

To build the project, run the following command:

```bash
./build.sh
```

The build script will:

1. Compile all C++ source files using the `g++` compiler with C++17 support.
2. Generate an executable named `Main`.
3. Clean up temporary object files after compilation.

If you're working in a local environment and need to reverse the process for debugging, use:

```bash
./build.sh --online-judge
```

This ensures that the build process accommodates specific configurations for local debugging.

### Build Requirements:

- **G++** compiler (C++17 support)
- **Unix-like environment** (for the provided build script)
- Standard C++ libraries

## Usage

After building the project, you can run the compiled executable as follows:

```bash
./Main
```

The program will:

1. Perform **lexical analysis** on the input source code, generating tokens.
2. Parse the tokens using **LR(1) parsing**.
3. Generate **intermediate code** in the form of quadruples.
4. Convert the intermediate code into **object code** using **ObjectCodeGen**.

Simply input your source code when prompted, and the program will handle the tokenization, parsing, and code generation phases.

## Components

### 1. Lexical Analyzer (Lexer)

The lexer is responsible for breaking the input source code into tokens. It identifies keywords, identifiers, numbers, operators, and handles comments.

Key Features:

- **Comment Removal**: Strips out comments from the source code.
- **Tokenization**: Breaks the input into tokens representing meaningful entities.
- **Error Reporting**: Provides error messages during the lexing phase.

Key Methods:

- `lexer(const std::string& str)`: Tokenizes the input string into a list of tokens.
- `removeComments()`: Removes comments from the input source code.
- `processIdentifier()`, `processNumber()`, `processOperator()`: Processes different types of tokens.

### 2. Parser (LR(1) Parsing)

The parser implements **LR(1) parsing**, a deterministic bottom-up parsing technique. It handles grammar rule management, symbol tables, and intermediate code generation.

Key Features:

- **Grammar Management**: Stores and processes grammar rules.
- **LR Parsing Table Generation**: Builds the table to drive the parsing process.
- **First Set Calculation**: Calculates first sets for non-terminals.
- **Intermediate Code Generation**: Outputs quadruples representing the intermediate code.

Key Methods:

- `parser(std::vector<std::string>& res)`: Main function to parse the input tokens.
- `get_LR_table()`: Generates the LR(1) parsing table.
- `compute_first_set()`: Computes the first sets for the grammar symbols.
- `generateParserOutput()`: Generates intermediate code as quadruples.

### 3. Object Code Generation (ObjectCodeGen)

The object code generation module takes the intermediate code and converts it into object code, simulating the creation of low-level machine code. This phase is crucial for compiling the intermediate representation into executable code.

Key Features:

- **Object Code Generation**: Translates intermediate quadruples into object code.
- **Memory Management**: Manages memory addresses and values for object code.
- **Instruction Representation**: Generates instructions suitable for a target architecture.

Key Methods:

- `generateObjectCode()`: Converts intermediate quadruples into object code instructions.
- `manageMemory()`: Manages memory allocation and symbolic address assignments.
- `outputObjectCode()`: Outputs the generated object code for further analysis or compilation.

## Implementation Details

- **Lexer**:
  - The lexer implements a single-pass tokenizer that processes the input string character by character.
  - It handles various token types, including identifiers, numbers, operators, and keywords.
  - The lexer also removes comments and adds necessary spaces to separate tokens.
- **Parser**:
  - Implements **LR(1)** parsing, a more advanced form of LL(1) parsing, providing better error detection and parsing precision.
  - It generates an **LR(1) table** to map the current parser state and lookahead symbol to the next parser state or rule.
  - The parser constructs **symbol tables** for variable names and operators, processes the grammar rules, and generates intermediate code in **quadruple format**.
- **Intermediate Code**:
  - The generated intermediate code uses **quadruples** to represent operations. Each quadruple consists of an operation (like `ADD`), two operands, and the result.
- **Object Code Generation**:
  - The **ObjectCodeGen** module takes these quadruples and translates them into low-level instructions. This involves generating memory addresses and instructions tailored for an idealized target architecture, simulating what would be done in a real machine code compiler.

## Features

1. **Modular Design**:
   - The project is split into clearly defined modules: lexer, parser, intermediate code generation, and object code generation. This separation makes the codebase easier to maintain and extend.
2. **LR(1) Parsing**:
   - The use of **LR(1) parsing** provides more robust error detection and supports a wide range of grammars.
3. **Intermediate and Object Code Generation**:
   - The project generates **quadruples** as intermediate code and then converts that into **object code**, simulating the low-level machine code generation process.
4. **Error Handling**:
   - The lexer captures errors in tokenization and provides informative error messages to help developers debug input source code.
5. **Modern C++ Features**:
   - Utilizes C++17 features and standard libraries for efficient memory management and cleaner code.

## Contributing

Feel free to contribute by:

1. Reporting issues or bugs.
2. Submitting pull requests with bug fixes or new features.
3. Suggesting improvements for better performance or usability.
4. Adding documentation or examples to help others understand the code.

We welcome contributions and encourage you to get involved with the development of this project!
