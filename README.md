# CompilerDesignStarter

A lightweight compiler implementation focusing on lexical analysis and parsing, built with modern C++17.

## Overview

This project implements a basic compiler frontend with two main components:
- A Lexical Analyzer (Lexer) for tokenization
- A Parser implementing LR(1) parsing with intermediate code generation

## Building the Project

The project can be built using the provided build script:

```bash
./start.sh
```

For online judge environments, use:
```bash
./start.sh --online-judge
```

The build script will:
1. Compile all C++ source files using g++ with C++17 support
2. Generate an executable named `Main`
3. Clean up temporary object files automatically

## Components

### Lexical Analyzer (Lexer)

The lexer performs the following operations:
- Removes comments from source code
- Tokenizes input strings into meaningful components
- Recognizes and processes:
  - Identifiers (variable names, function names)
  - Numbers (integers, floating-point values)
  - Operators
  - Keywords
- Provides error handling and reporting through `err_msg` vector

Key methods:
- `lexer(const std::string& str)`: Main entry point for lexical analysis
- `removeComments(string str)`: Strips comments from source code
- `insertSpaces(string str)`: Adds spacing for token separation
- Various token processing methods for different types of lexemes

### Parser

The parser implements LR(1) parsing with the following features:
- Symbol management and grammar rule handling
- LR(1) parsing table generation
- First set computation
- Item set management
- Intermediate code generation using quadruples

Key components:
- Grammar rule management using production rules
- State management through parser states
- Symbol tables for parsed elements
- Attribute stack for semantic analysis
- Intermediate code generation using quadruples

Key methods:
- `parser(vector<string>& res)`: Main parsing function
- `get_LR_table()`: Generates the LR parsing table
- `compute_first_set()`: Computes first sets for grammar symbols
- `get_closure()`: Implements the LR closure operation
- `generateParserOutput()`: Produces intermediate code output

## Project Structure

```
.
├── lexer.h          # Lexical analyzer header
├── parser.h         # Parser header
├── expression.h     # Expression handling
├── parserStruct.h   # Parser data structures
├── opkeymap.h       # Operator and keyword mappings
└── start.sh         # Build script
```

## Build Requirements

- G++ compiler with C++17 support
- Standard C++ libraries
- Unix-like environment (for build script)

## Features

1. Modern C++ Implementation
   - Uses C++17 features
   - STL containers and algorithms
   - RAII-compliant resource management

2. Robust Error Handling
   - Error message collection during lexical analysis
   - Structured error reporting

3. Extensible Design
   - Modular component structure
   - Clear separation of lexing and parsing phases
   - Well-defined interfaces between components

4. Performance Optimizations
   - Efficient symbol table lookups using maps
   - Optimized state management in parsing
   - Minimal memory footprint through smart container usage

## Usage

After building the project:

1. Run the executable:
```bash
./Main
```

2. The program will:
   - Perform lexical analysis on the input
   - Generate tokens
   - Parse the tokens according to the grammar
   - Produce intermediate code if parsing is successful

## Implementation Notes

- The lexer implements a single-pass tokenization strategy
- The parser uses LR(1) parsing technique for better error handling and precise parsing
- Intermediate code is generated using quadruples for optimization opportunities
- Symbol table management is optimized for quick lookups and minimal memory usage

## Contributing

Feel free to contribute to this project by:
1. Reporting issues
2. Submitting pull requests
3. Suggesting improvements
4. Adding documentation