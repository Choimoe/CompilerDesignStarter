#ifndef PARSERUTIL_H
#define PARSERUTIL_H

#include "parserStruct.h"
#include <vector>
#include <cstring>

/**
 * @class Parser
 * @brief Forward declaration of the Parser class for use in the utility functions.
 */
class Parser;

namespace parserUtil {

/**
 * @brief Backpatches a placeholder index in the list of quadruples with a target address.
 * 
 * @param i The index of the placeholder to be backpatched.
 * @param t The target address to backpatch.
 * @param q The list of quadruples where the backpatching is performed.
 */
void backpatch(int i, int t, std::vector<parserStruct::QuadTuple>& q);

/**
 * @brief Merges two quadruple lists and returns the resulting index.
 * 
 * @param a The index of the first list to merge.
 * @param b The index of the second list to merge.
 * @param q The list of quadruples where the merge is performed.
 * @return int The index of the merged list.
 */
int merge(int a, int b, std::vector<parserStruct::QuadTuple>& q);

/**
 * @brief Looks up a symbol by name in the symbol table and returns its string representation.
 * 
 * @param name The name of the symbol to look up.
 * @param s The symbol table containing the symbols.
 * @return std::string The string representation of the symbol if found.
 */
std::string lookup(const std::string& name, const std::vector<parserStruct::Symbol>& s);

/**
 * @brief Looks up the type of a symbol by name in the symbol table and returns its type.
 * 
 * @param name The name of the symbol to look up.
 * @param s The symbol table containing the symbols.
 * @return std::string The type of the symbol if found.
 */
std::string look_up_type(const std::string& name, const std::vector<parserStruct::Symbol>& s);

/**
 * @brief Generates a new temporary variable name based on a counter and updates the index.
 * 
 * @param k The counter value used to generate the variable name.
 * @param t A vector representing the current temporary variable indices.
 * @return std::string The generated temporary variable name.
 */
std::string Temp_New(int k, std::vector<int>& t);

/**
 * @brief Finds positions of multiple delimiters in a string.
 * 
 * @param str The input string to search within.
 * @param delimiter The character used as the delimiter.
 * @return std::tuple<size_t, size_t, size_t, size_t, size_t> A tuple of positions found.
 */
std::tuple<size_t, size_t, size_t, size_t, size_t> findPositions(std::string_view str, char delimiter);

/**
 * @brief Parses a string representation of a quadruple into a QuadTuple object.
 * 
 * @param input The string input representing a quadruple.
 * @return parserStruct::QuadTuple The parsed QuadTuple object.
 */
parserStruct::QuadTuple parseQuadTuple(std::string_view input);

}  // namespace parserUtil

#endif  // PARSERUTIL_H
