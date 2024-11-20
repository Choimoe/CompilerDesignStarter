#ifndef PARSER_H
#define PARSER_H

#include "expression.h"
#include "parserStruct.h"
#include <map>
#include <set>
#include <stack>
#include <string>
#include <vector>
#include <functional>

// The Parser class encapsulates the entire parsing process, including symbol
// management, grammar rules, and state handling.
class Parser {
public:
    Parser();  // Constructor to initialize the parser

    // Main parser function, processes input and returns a vector of strings
    // (likely the parsed output)
    std::vector<std::string> parser(std::vector<std::string>& res);

private:
    int parserState;  // Variable to track the current parser state

    // Maps symbols (e.g., terminal and non-terminal names) to integer IDs for
    // easy lookup
    std::map<std::string, int> symbolToIdMap;

    // The LR(1) parsing table (goto table), which defines the transition rules
    // for parser states
    std::vector<std::vector<int>> gotoTable;

    // First sets for each non-terminal symbol, which define the set of terminal
    // symbols that can begin a string derived from the non-terminal
    std::map<std::string, std::set<std::string>> firstSets;

    // Grammar rules stored in a map where each non-terminal has a list of
    // production rules
    std::map<std::string, std::vector<std::pair<std::vector<std::string>, int>>>
        grammarRules;

    // Flattened grammar rules, potentially for easier handling during parsing
    std::vector<std::pair<std::string, std::vector<std::string>>>
        flattenedGrammarRules;

    // Item sets used in LR parsing, likely for managing items in an item set
    // (LR items)
    std::vector<std::set<Expression>> itemSets;

    // Stack of attributes used during parsing (e.g., semantic values)
    std::stack<parserStruct::Attribute> attributeStack;

    // Symbol table holding the parsed symbols (both terminals and
    // non-terminals)
    std::vector<parserStruct::Symbol> symbolTable;

    // Stack of parser states for managing the LR parsing process
    std::stack<int> stateStack;

    // Stack of symbols for tracking the symbols in the current production rule
    std::stack<std::string> symbolStack;

    // List of quadruples (intermediate code) generated during parsing
    std::vector<parserStruct::QuadTuple> quadruples;

    // Temporary variable indices used to track the indices of temporary
    // variables in the intermediate code
    std::vector<int> tempVariableIndices;

    // Computes the suffix for a given Expression (used for parsing operations)
    std::set<std::string> compute_suffix(const Expression& I);

    // Adds the non-terminal closure for a given Expression to a set of
    // Expressions
    void add_non_terminal_closure(const Expression& I,
                                  std::set<Expression>& pre,
                                  const std::string& cur_str);

    // Computes the closure of a given set of Expressions (closure operation in
    // LR parsing)
    std::set<Expression> get_closure(const std::set<Expression>& cur);

    // Initializes the symbol mapping (e.g., mapping strings to token types)
    void init_symbol_mapping();

    // Parses the expressions from the input list and fills out necessary
    // grammar structures
    void parse_expressions(std::vector<std::string>& tmp);

    // Computes the first set for the given symbols (used for parsing and
    // lookahead)
    void compute_first_set(std::vector<std::string>& tmp);

    // Retrieves the LR item sets and corresponding symbols
    std::pair<std::vector<std::set<Expression>>, std::vector<std::string>>
    get_Items(int idx);

    // Generates the LR parsing table (used for syntax analysis)
    void get_LR_table();

    // Generates the output for the parser in the form of quadruples and symbols
    std::vector<std::string> generateParserOutput(
        int nxt,
        const std::vector<parserStruct::QuadTuple>& quadruples,
        const std::vector<parserStruct::Symbol>& symbolTable,
        const std::vector<int>& tempVariableIndices);
};

#endif  // PARSER_H
