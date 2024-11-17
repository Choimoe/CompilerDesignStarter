#include "parserUtil.h"
#include "parserStruct.h"
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <optional>

namespace parserUtil {

// Updates the placeholder (i) in the list of quadruples (q) with the target
// address (t) This is typically used for backpatching jump addresses or other
// unresolved references
void backpatch(int i, int t, std::vector<parserStruct::QuadTuple>& q) {
    while (i > 0) {
        int nxt = q[i].Xfour;
        q[i].Xfour = t;  // Set the placeholder to the target address
        i = nxt;         // Move to the next backpatch placeholder, if any
    }
}

// Merges two quadruple lists (a and b), linking the end of list b to the start
// of list a Returns the updated index for the merged list
int merge(int a, int b, std::vector<parserStruct::QuadTuple>& q) {
    if (a < 0 || b < 0)
        return std::max(a, b);  // If either list is empty, return the valid one
    int x = b;
    int p;
    while (x > 0) {
        p = x;
        x = q[x].Xfour;  // Find the last element of list b
    }
    q[p].Xfour = a;  // Link the end of list b to the start of list a
    return b;        // Return the index of list b
}

// Looks up a symbol by its name in the symbol table (s) and returns its index
// in the temporary base (TB)
std::string lookup(const std::string& name,
                   const std::vector<parserStruct::Symbol>& s) {
    auto it = std::find_if(
        s.begin(), s.end(), [&name](const parserStruct::Symbol& i) {
            return i.name == name;  // Find the symbol matching the name
        });
    if (it != s.end()) {
        return "TB" + std::to_string(std::distance(
                          s.begin(), it));  // Return the index in TB
    }
    std::cout << "Syntax Error\n";
    exit(0);  // If the symbol is not found, exit with an error
}

// Looks up the type of a symbol by its name in the symbol table (s) and returns
// the symbol type
std::string look_up_type(const std::string& name,
                         const std::vector<parserStruct::Symbol>& s) {
    auto it = std::find_if(
        s.begin(), s.end(), [&name](const parserStruct::Symbol& i) {
            return i.name == name;  // Find the symbol matching the name
        });
    if (it != s.end()) {
        return it->type;  // Return the type of the found symbol
    }
    std::cout << "Syntax Error\n";
    exit(0);  // If the symbol is not found, exit with an error
}

// Generates a new temporary variable name, appending a number to a base "T"
// string k is used to determine if the temporary variable is an integer or a
// double
std::string Temp_New(int k, std::vector<int>& t) {
    t.push_back(k);  // Add the index to the temporary variable list
    std::string res =
        "T" + std::to_string(t.size() - 1);  // Create the variable name
    res +=
        (k == 0) ? "_i" : "_d";  // Append "_i" for integer or "_d" for double
    return res;                  // Return the new temporary variable name
}

}  // namespace parserUtil
