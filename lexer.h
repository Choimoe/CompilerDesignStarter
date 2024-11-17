#ifndef LEXER_H
#define LEXER_H

#include <string>
#include <vector>
#include <map>
#include "opkeymap.h"

class Lexer {
public:
    std::vector<std::pair<int, std::string> > err_msg;

    Lexer();
    std::vector<std::string> lexer(const std::string& str);

private:
    std::string removeComments(std::string str);
    std::string insertSpaces(std::string str);
    std::vector<std::string> processIdentifier(std::string& token, size_t& idx);
    std::vector<std::string> processNumber(std::string& token, size_t& idx);
    std::vector<std::string> processOperator(std::string& token, size_t& idx);
    std::vector<std::string> processKeyword(std::string& token);
    std::vector<std::string> processToken(std::string token);
};

#endif
