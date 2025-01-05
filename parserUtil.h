#ifndef PARSERUTIL_H
#define PARSERUTIL_H

#include "parserStruct.h"
#include <vector>
#include <cstring>

// Forward declaration of Parser class for use in the functions
class Parser;

namespace parserUtil {

// Backpatches a placeholder (i) in the list of quadruples (q) with a target
// address (t)
void backpatch(int i, int t, std::vector<parserStruct::QuadTuple>& q);

// Merges two quadruple lists (a and b) and returns the resulting index
int merge(int a, int b, std::vector<parserStruct::QuadTuple>& q);

// Looks up a symbol by name in the symbol table (s) and returns its
// corresponding string representation
std::string lookup(const std::string& name,
                   const std::vector<parserStruct::Symbol>& s);

// Looks up the type of a symbol by its name in the symbol table (s) and returns
// its type as a string
std::string look_up_type(const std::string& name,
                         const std::vector<parserStruct::Symbol>& s);

// Generates a new temporary variable name, based on a counter (k) and updates
// the temporary variable index (t)
std::string Temp_New(int k, std::vector<int>& t);

std::tuple<size_t, size_t, size_t, size_t, size_t> findPositions(
    std::string_view str,
    char delimiter);

parserStruct::QuadTuple parseQuadTuple(std::string_view input);

}  // namespace parserUtil

#endif  // PARSERUTIL_H
