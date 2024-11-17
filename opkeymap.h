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

static const std::unordered_map<std::string, int> Non_terminal{
    {"PROG", 1},     {"SUBPROG", 1},     {"M", 1},
    {"N", 1},        {"VARIABLES", 1},   {"STATEMENT", 1},
    {"VARIABLE", 1}, {"T", 1},           {"ASSIGN", 1},
    {"SCANF", 1},    {"PRINTF", 1},      {"L", 1},
    {"B", 1},        {"EXPR", 1},        {"ORITEM", 1},
    {"ANDITEM", 1},  {"RELITEM", 1},     {"NOITEM", 1},
    {"ITEM", 1},     {"FACTOR", 1},      {"BORTERM", 1},
    {"BANDTERM", 1}, {"PLUS_MINUS", 1},  {"MUL_DIV", 1},
    {"REL", 1},      {"SCANF_BEGIN", 1}, {"PRINTF_BEGIN", 1},
    {"ID", 1}};

static const std::unordered_map<std::string, int> terminal{
    {"int", 1},    {"double", 1}, {"scanf", 1}, {"printf", 1}, {"if", 1},
    {"then", 1},   {"while", 1},  {"do", 1},    {",", 1},      {";", 1},
    {"+", 1},      {"-", 1},      {"*", 1},     {"/", 1},      {"=", 1},
    {"==", 1},     {"!=", 1},     {"<", 1},     {"<=", 1},     {">", 1},
    {">=", 1},     {"(", 1},      {")", 1},     {"{", 1},      {"}", 1},
    {"!", 1},      {"&&", 1},     {"||", 1},    {"id", 1},     {"UINT", 1},
    {"UFLOAT", 1}, {"^", 1},      {"#", 1}};

static const std::unordered_map<std::string, int> typr_to_int{{"int", 0},
                                                              {"double", 1}};

static const std::vector<std::string> language{
    "PROG -> SUBPROG",
    "SUBPROG -> M VARIABLES  STATEMENT",
    "M -> ^",
    "N -> ^",
    "VARIABLES -> VARIABLES VARIABLE ;",
    "VARIABLES -> VARIABLE ;",
    "T -> int",
    "T -> double",
    "ID -> id",
    "VARIABLE -> T ID",
    "VARIABLE -> VARIABLE , ID",
    "STATEMENT -> ASSIGN",
    "STATEMENT -> SCANF",
    "STATEMENT -> PRINTF",
    "STATEMENT -> ^",
    "STATEMENT -> { L ; }",
    "STATEMENT -> while N B do N STATEMENT",
    "STATEMENT -> if B then N STATEMENT",
    "ASSIGN -> ID = EXPR",
    "L -> L ; N STATEMENT",
    "L -> STATEMENT",
    "EXPR -> EXPR || ORITEM",
    "EXPR -> ORITEM",
    "ORITEM -> ORITEM && ANDITEM",
    "ORITEM -> ANDITEM",
    "ANDITEM -> NOITEM",
    "ANDITEM -> ! NOITEM",
    "NOITEM -> NOITEM REL RELITEM",
    "NOITEM -> RELITEM",
    "RELITEM -> RELITEM PLUS_MINUS ITEM",
    "RELITEM -> ITEM",
    "ITEM -> FACTOR",
    "ITEM -> ITEM MUL_DIV FACTOR",
    "FACTOR -> ID",
    "FACTOR -> UINT",
    "FACTOR -> UFLOAT",
    "FACTOR -> ( EXPR )",
    "FACTOR -> PLUS_MINUS FACTOR",
    "B -> B || N BORTERM",
    "B -> BORTERM",
    "BORTERM -> BORTERM && N BANDTERM",
    "BORTERM -> BANDTERM",
    "BANDTERM -> ( B )",
    "BANDTERM ->  ! BANDTERM",
    "BANDTERM -> BFACTOR REL BFACTOR",
    "BANDTERM -> BFACTOR",
    "BFACTOR -> UINT",
    "BFACTOR -> UFLOAT",
    "BFACTOR -> ID",
    "PLUS_MINUS -> +",
    "PLUS_MINUS -> -",
    "MUL_DIV -> *",
    "MUL_DIV -> /",
    "REL -> ==",
    "REL -> !=",
    "REL -> <",
    "REL -> <=",
    "REL -> >",
    "REL -> >=",
    "SCANF -> SCANF_BEGIN )",
    "SCANF_BEGIN -> SCANF_BEGIN , ID",
    "SCANF_BEGIN -> scanf ( ID",
    "PRINTF -> PRINTF_BEGIN )",
    "PRINTF_BEGIN -> printf ( ID",
    "PRINTF_BEGIN -> PRINTF_BEGIN , ID"};
}  // namespace OpKeyMap

#endif  // OPERATOR_KEYWORD_MAP_H
