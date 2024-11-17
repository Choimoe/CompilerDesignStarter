#ifndef LEXER_H
#define LEXER_H

#include <string>
#include <vector>
#include <map>
#include "opkeymap.h"

// Lexer class performs lexical analysis on input strings.
// It tokenizes the input string into a list of meaningful components, such as
// keywords, identifiers, numbers, and operators.
class Lexer {
public:
    std::vector<std::pair<int, std::string>>
        err_msg;  // Stores error messages, if any, during lexing.

    // Constructor for the Lexer class
    Lexer();

    // Main function to process the input string and return a list of tokenized
    // strings
    std::vector<std::string> lexer(const std::string& str);

private:
    // Removes comments from the input string, returning the cleaned string.
    std::string removeComments(std::string str);

    // Inserts spaces where necessary to separate tokens in the input string.
    std::string insertSpaces(std::string str);

    // Processes identifiers (e.g., variable names, function names) and returns
    // a list of tokens.
    std::vector<std::string> processIdentifier(std::string& token, size_t& idx);

    // Processes numbers (e.g., integers, floating-point values) and returns a
    // list of tokens.
    std::vector<std::string> processNumber(std::string& token, size_t& idx);

    // Processes operators (e.g., '+', '-', '*', etc.) and returns a list of
    // tokens.
    std::vector<std::string> processOperator(std::string& token, size_t& idx);

    // Processes keywords (e.g., 'if', 'else', 'while', etc.) and returns a list
    // of tokens.
    std::vector<std::string> processKeyword(std::string& token);

    // Processes a single token and returns the corresponding list of tokens.
    std::vector<std::string> processToken(std::string token);
};

#endif  // LEXER_H
