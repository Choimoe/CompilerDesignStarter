#ifndef OPKEYMAP_H
#define OPKEYMAP_H

#include <string>
#include <unordered_map>

namespace OpKeyMap {
static const std::unordered_map<std::string, std::string> operators{
    {"=", "AO"},    {"==", "RO"},      {">", "RO"},    {">=", "RO"},
    {"<", "RO"},    {"<=", "RO"},      {"||", "LO"},   {"&&", "LO"},
    {"!", "LO"},    {"!=", "RO"},      {"+", "PLUS"},  {"-", "MINUS"},
    {"*", "TIMES"}, {"/", "DIVISION"}, {",", "COMMA"}, {"(", "BRACE"},
    {")", "BRACE"}, {"{", "BRACE"},    {"}", "BRACE"}, {";", "SEMICOLON"}};

static const std::unordered_map<std::string, std::string> keywords{
    {"int", "INTSYM"},       {"double", "DOUBLESYM"}, {"scanf", "SCANFSYM"},
    {"printf", "PRINTFSYM"}, {"if", "IFSYM"},         {"then", "THENSYM"},
    {"while", "WHILESYM"},   {"do", "DOSYM"}};
}  // namespace OpKeyMap

#endif  // OPERATOR_KEYWORD_MAP_H
