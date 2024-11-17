#include "lexer.h"
#include <iostream>
#include <fstream>
#include <cctype>
#include <algorithm>

// Constructor for Lexer class
Lexer::Lexer() {}

// This function removes comments from the input string.
// It handles both block comments (/* ... */) and line comments (// ...).
std::string Lexer::removeComments(std::string str) {
    // Remove block comments (/* ... */)
    while (str.find("/*") != std::string::npos) {
        auto pos = str.find("/*");   // Find the start of a block comment
        auto pos1 = str.find("*/");  // Find the end of a block comment
        if (pos1 == std::string::npos)
            str.erase(pos);  // If no end comment is found, erase from start
        str.erase(
            pos,
            pos1 - pos +
                2);  // Erase the comment part (including the comment markers)
    }

    // Remove line comments (// ...)
    while (str.find("//") != std::string::npos) {
        auto pos = str.find("//");  // Find the start of a line comment
        auto pos1 =
            str.find("\n", pos);  // Find the newline at the end of the comment
        str.erase(pos, pos1 - pos +
                           1);  // Erase the comment from the start to the end
    }
    return str;
}

// This function adds spaces around operators in the input string to separate
// them from adjacent tokens, making it easier for tokenization.
std::string Lexer::insertSpaces(std::string str) {
    std::string s;
    auto n = str.size();
    for (size_t i = 0; i < n; i++) {
        // Check if the current character and the next one form an operator
        if (i + 2 <= n && OpKeyMap::operators.count(str.substr(i, 2)))
            s.append(" ")
                .append(str.substr(i++, 2))
                .append(" ");  // Add a space around the operator
        // Check if the current character is an operator
        else if (i + 1 <= n && OpKeyMap::operators.count(str.substr(i, 1)))
            s.append(" ").append(1, str[i]).append(
                " ");  // Add a space around the operator
        else
            s.append(1,
                     str[i]);  // Otherwise, just append the character as it is
    }
    return s;
}

// This function processes an identifier (e.g., a variable or function name)
// from the string. It assumes the identifier starts at the given index.
std::vector<std::string> Lexer::processIdentifier(std::string& token,
                                                  size_t& idx) {
    std::vector<std::string> ans;
    std::string_view str_view(token);
    // Find the first non-alphabetical character starting from the given index
    auto end = std::find_if_not(str_view.begin() + idx, str_view.end(),
                                [](char c) { return std::isalpha(c); });
    std::string identifier(str_view.substr(
        idx, end - (str_view.begin() + idx)));  // Extract the identifier
    if (!identifier.empty()) {
        ans.push_back(
            identifier +
            " IDENT\n");           // Add the identifier with "IDENT" token type
        idx += identifier.size();  // Move the index forward by the length of
                                   // the identifier
    }
    return ans;
}

// This function processes a number (integer or floating-point) from the string.
// It starts at the given index and returns a vector containing the tokenized
// number.
std::vector<std::string> Lexer::processNumber(std::string& token, size_t& idx) {
    std::vector<std::string> ans;
    std::string_view str_view(token);
    std::string h;
    bool isFloat = false;
    // Iterate over characters and check for digits and the decimal point
    for (auto it = str_view.begin() + idx;
         it != str_view.end() && (std::isdigit(*it) || *it == '.'); ++it) {
        h.push_back(*it);
        if (*it == '.') {
            isFloat = true;  // Mark as floating-point number if decimal point
                             // is found
        }
    }

    // Check if the number is a valid floating-point number
    if (isFloat) {
        if (std::count(h.begin(), h.end(), '.') > 1) {
            err_msg.push_back(std::make_pair(
                1, std::string("Malformed number: More than one decimal point "
                               "in a floating point number.\n")));
            return {};  // Return empty if malformed number
        }
        if (h.front() == '.' || h.back() == '.') {
            err_msg.push_back(std::make_pair(
                2,
                std::string("Malformed number: Decimal point at the beginning "
                            "or end of a floating point number.\n")));
            return {};  // Return empty if malformed number
        }
    }

    // Check for malformed integer numbers with leading zeros
    if (!isFloat && h.front() == '0' && h.size() > 1) {
        err_msg.push_back(std::make_pair(
            3,
            std::string("Malformed number: Leading zeros in an integer.\n")));
        return {};  // Return empty if malformed number
    }

    // Add the number to the result vector with the appropriate token type
    ans.push_back(h + (isFloat ? " DOUBLE\n" : " INT\n"));
    idx += h.size();  // Move the index forward by the length of the number
    return ans;
}

// This function processes operators (e.g., +, -, *, /, etc.) from the string.
// It starts at the given index and returns the corresponding operator token.
std::vector<std::string> Lexer::processOperator(std::string& token,
                                                size_t& idx) {
    std::vector<std::string> ans;
    // Check for two-character operators first (e.g., "==", "+=")
    if (idx + 2 <= token.size() &&
        OpKeyMap::operators.count(token.substr(idx, 2))) {
        ans.push_back(token.substr(idx, 2) + " " +
                      OpKeyMap::operators.at(token.substr(idx, 2)) + "\n");
        idx += 2;
    }
    // Check for single-character operators (e.g., "+", "-", "*")
    else if (OpKeyMap::operators.count(token.substr(idx, 1))) {
        ans.push_back(token.substr(idx, 1) + " " +
                      OpKeyMap::operators.at(token.substr(idx, 1)) + "\n");
        idx += 1;
    }
    return ans;
}

// This function processes a keyword (e.g., "if", "else", "while") from the
// string. If the token matches a known keyword, it returns the keyword with its
// token type.
std::vector<std::string> Lexer::processKeyword(std::string& token) {
    std::vector<std::string> ans;
    if (OpKeyMap::keywords.count(token)) {
        ans.push_back(token + ' ' + OpKeyMap::keywords.at(token) + '\n');
    }
    return ans;
}

// This function processes a token by categorizing it into identifiers, numbers,
// operators, or keywords. It returns a vector of corresponding tokens.
std::vector<std::string> Lexer::processToken(std::string token) {
    std::vector<std::string> ans;
    size_t idx = 0;
    bool alp = false;
    bool dig = false;
    // First, try processing the token as a keyword
    auto keywordResult = processKeyword(token);
    ans.insert(ans.end(), keywordResult.begin(), keywordResult.end());
    if (!ans.empty())
        return ans;  // Return early if it's a keyword

    // Otherwise, process the token character by character
    while (idx < token.size()) {
        if (isalpha(token[idx])) {
            // If it's a letter, process it as an identifier
            auto result = processIdentifier(token, idx);
            ans.insert(ans.end(), result.begin(), result.end());
            alp = true;
        } else if (isdigit(token[idx]) || token[idx] == '.') {
            // If it's a digit or a decimal point, process it as a number
            auto result = processNumber(token, idx);
            ans.insert(ans.end(), result.begin(), result.end());
            dig = true;
        } else if (!isdigit(token[idx]) && !isalpha(token[idx])) {
            // If it's neither a letter nor a digit, process it as an operator
            auto result = processOperator(token, idx);
            ans.insert(ans.end(), result.begin(), result.end());
        }
        // Handle unrecognizable characters
        if (idx < token.size() &&
            (!isdigit(token[idx]) && !isalpha(token[idx])) &&
            (dig == 0 && alp == 0)) {
            err_msg.push_back(
                std::make_pair(4, std::string("Unrecognizable characters.\n")));
            return {};  // Return empty if unrecognizable character found
        }
        idx++;  // Move to the next character
    }
    return ans;
}

// The main lexer function that processes the input string by first removing
// comments, adding spaces around operators, and then breaking the string into
// tokens.
std::vector<std::string> Lexer::lexer(const std::string& str) {
    std::vector<std::string> ans;
    std::string tempStr = removeComments(str);  // Remove comments
    tempStr = insertSpaces(tempStr);            // Add spaces around operators
    std::string token;
    int n = tempStr.size();
    // Split the string into tokens based on spaces, newlines, and tabs
    for (int i = 0; i < n; i++) {
        if (tempStr[i] == ' ' || tempStr[i] == '\n' || tempStr[i] == '\t') {
            if (!token.empty()) {
                // Process the token when a whitespace is encountered
                auto tokens = processToken(token);
                ans.insert(ans.end(), tokens.begin(), tokens.end());
            }
            token = "";  // Clear the current token
        } else {
            token += tempStr[i];  // Append the current character to the token
        }
    }
    return ans;
}
