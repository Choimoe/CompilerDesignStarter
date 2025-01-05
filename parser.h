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

/**
 * @class Parser
 * @brief Encapsulates the entire parsing process, including grammar rules, symbol 
 * management, and state handling for syntax analysis.
 */
class Parser {
public:
    /**
     * @brief Constructs a Parser object and initializes the necessary components.
     */
    Parser();

    /**
     * @brief Main function to process the input and generate parsed output.
     * 
     * @param res A reference to a vector of strings containing the input data.
     * @return A vector of strings representing the parsed output.
     */
    std::vector<std::string> parser(std::vector<std::string>& res);

private:
    int parserState; ///< Tracks the current state of the parser.

    std::map<std::string, int> symbolToIdMap; ///< Maps symbols to integer IDs for efficient lookup.

    std::vector<std::vector<int>> gotoTable; ///< LR(1) parsing table defining state transitions.

    std::map<std::string, std::set<std::string>> firstSets; ///< Stores first sets for grammar symbols.

    std::map<std::string, std::vector<std::pair<std::vector<std::string>, int>>> grammarRules; 
    ///< Contains grammar rules with non-terminal symbols and their production rules.

    std::vector<std::pair<std::string, std::vector<std::string>>> flattenedGrammarRules; 
    ///< Stores flattened grammar rules for easier parsing operations.

    std::vector<std::set<Expression>> itemSets; ///< Item sets used for LR parsing.

    std::stack<parserStruct::Attribute> attributeStack; ///< Stack of attributes for semantic analysis.

    std::vector<parserStruct::Symbol> symbolTable; ///< Symbol table for terminals and non-terminals.

    std::stack<int> stateStack; ///< Tracks parser states during parsing.

    std::stack<std::string> symbolStack; ///< Tracks symbols used in parsing operations.

    std::vector<parserStruct::QuadTuple> quadruples; ///< Intermediate code generated during parsing.

    std::vector<int> tempVariableIndices; ///< Tracks indices for temporary variables.

    /**
     * @brief Computes the suffix for a given expression.
     * 
     * @param I An Expression object.
     * @return A set of strings representing the suffix.
     */
    std::set<std::string> compute_suffix(const Expression& I);

    /**
     * @brief Adds the closure of a non-terminal for an expression to the given set.
     * 
     * @param I The current Expression object.
     * @param pre The set of Expressions to update.
     * @param cur_str The current string being processed.
     */
    void add_non_terminal_closure(const Expression& I,
                                  std::set<Expression>& pre,
                                  const std::string& cur_str);

    /**
     * @brief Computes the closure of a set of Expressions.
     * 
     * @param cur A set of Expressions to compute the closure for.
     * @return A set of Expressions representing the closure.
     */
    std::set<Expression> get_closure(const std::set<Expression>& cur);

    /**
     * @brief Initializes the mapping of symbols to IDs.
     */
    void init_symbol_mapping();

    /**
     * @brief Parses and processes the expressions from the input data.
     * 
     * @param tmp A vector of strings containing the input expressions.
     */
    void parse_expressions(std::vector<std::string>& tmp);

    /**
     * @brief Computes the first set for grammar symbols.
     * 
     * @param tmp A vector of strings representing the symbols.
     */
    void compute_first_set(std::vector<std::string>& tmp);

    /**
     * @brief Retrieves LR(1) item sets and associated symbols.
     * 
     * @param idx The index of the current item set.
     * @return A pair containing the item sets and associated symbols.
     */
    std::pair<std::vector<std::set<Expression>>, std::vector<std::string>> get_Items(int idx);

    /**
     * @brief Generates the LR parsing table.
     */
    void get_LR_table();

    /**
     * @brief Generates the parser's output based on intermediate results.
     * 
     * @param nxt The next state index.
     * @param quadruples A vector of QuadTuple objects representing intermediate code.
     * @param symbolTable A vector of Symbol objects for the parsed symbols.
     * @param tempVariableIndices A vector of integers for temporary variable indices.
     * @return A vector of strings representing the parser's output.
     */
    std::vector<std::string> generateParserOutput(
        int nxt,
        const std::vector<parserStruct::QuadTuple>& quadruples,
        const std::vector<parserStruct::Symbol>& symbolTable,
        const std::vector<int>& tempVariableIndices);
};

#endif  // PARSER_H
