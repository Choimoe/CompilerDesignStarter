#ifndef LEXER_H
#define LEXER_H

#include <string>
#include <vector>
#include <map>
#include "opkeymap.h"

/**
 * @class Lexer
 * @brief Performs lexical analysis on input strings by tokenizing them into meaningful components, 
 * such as keywords, identifiers, numbers, and operators.
 */
class Lexer {
public:
    /**
     * @brief Stores error messages encountered during lexical analysis.
     */
    std::vector<std::pair<int, std::string>> err_msg;

    /**
     * @brief Constructs a Lexer object and initializes necessary components.
     */
    Lexer();

    /**
     * @brief Analyzes the input string and generates a list of tokens.
     * 
     * @param str The input string to be tokenized.
     * @return A vector of strings representing the tokenized components.
     */
    std::vector<std::string> lexer(const std::string& str);

private:
    /**
     * @brief Removes comments from the input string.
     * 
     * @param str The input string potentially containing comments.
     * @return A string with comments removed.
     */
    std::string removeComments(std::string str);

    /**
     * @brief Inserts spaces where necessary to separate tokens in the input string.
     * 
     * @param str The input string to be processed.
     * @return A string with spaces inserted for token separation.
     */
    std::string insertSpaces(std::string str);

    /**
     * @brief Processes identifiers such as variable or function names.
     * 
     * @param token The current token being analyzed.
     * @param idx The current index in the string being processed.
     * @return A vector of strings containing the identified tokens.
     */
    std::vector<std::string> processIdentifier(std::string& token, size_t& idx);

    /**
     * @brief Processes numbers, including integers and floating-point values.
     * 
     * @param token The current token being analyzed.
     * @param idx The current index in the string being processed.
     * @return A vector of strings containing the identified number tokens.
     */
    std::vector<std::string> processNumber(std::string& token, size_t& idx);

    /**
     * @brief Processes operators such as '+', '-', '*', and so on.
     * 
     * @param token The current token being analyzed.
     * @param idx The current index in the string being processed.
     * @return A vector of strings containing the identified operator tokens.
     */
    std::vector<std::string> processOperator(std::string& token, size_t& idx);

    /**
     * @brief Processes keywords such as 'if', 'else', 'while', etc.
     * 
     * @param token The current token being analyzed.
     * @return A vector of strings containing the identified keyword tokens.
     */
    std::vector<std::string> processKeyword(std::string& token);

    /**
     * @brief Processes a single token and identifies its components.
     * 
     * @param token The token to be processed.
     * @return A vector of strings representing the processed token components.
     */
    std::vector<std::string> processToken(std::string token);
};

#endif  // LEXER_H
