#ifndef EXPRESSION_H
#define EXPRESSION_H

#include <algorithm>
#include <iostream>
#include <vector>
#include <set>
#include <tuple>

// The Expression structure represents an expression in the parser or compiler,
// encapsulating the expression's type, symbols, position, index, and suffix.
struct Expression {
    std::string T;  // The type of the expression (e.g., "int", "float")
    std::vector<std::string>
        S;           // The list of symbols (e.g., variables, operators)
    size_t pos = 0;  // The position of the expression (used for tracking)
    int idx = 0;     // The index of the expression (can be used for ordering or
                     // referencing)
    std::set<std::string>
        suffix;  // The set of suffixes associated with the expression (e.g.,
                 // possible continuations or references)

    // Default constructor
    Expression() = default;

    // Constructor for initializing an Expression with specific values
    explicit Expression(std::string T,
                        const std::vector<std::string>& S,
                        size_t pos,
                        int idx,
                        const std::set<std::string>& suffix)
        : T(std::move(T)), S(S), pos(pos), idx(idx), suffix(suffix) {}

    // Overloaded stream insertion operator for printing the expression
    friend std::ostream& operator<<(std::ostream& os, const Expression& expr) {
        os << "T: " << expr.T << "\n";  // Print the type of the expression
        os << "S:";
        for (const auto& str : expr.S) {  // Print each symbol in the expression
            os << " " << str;
        }
        os << "\n";
        os << "pos: " << expr.pos << "\n";  // Print the position
        os << "idx: " << expr.idx << "\n";  // Print the index
        os << "suffix:";
        for (const auto& suf : expr.suffix) {  // Print each suffix
            os << " " << suf;
        }
        os << "\n";
        return os;
    }
};

// Overloaded operator< for comparing two expressions
// This is used in ordered data structures like std::set or std::map
inline bool operator<(const Expression& lhs, const Expression& rhs) {
    return std::tie(lhs.T, lhs.S, lhs.pos, lhs.idx, lhs.suffix) <
           std::tie(rhs.T, rhs.S, rhs.pos, rhs.idx, rhs.suffix);
}

// Overloaded operator== for comparing if two expressions are equal
inline bool operator==(const Expression& lhs, const Expression& rhs) {
    return std::tie(lhs.T, lhs.S, lhs.pos, lhs.idx, lhs.suffix) ==
           std::tie(rhs.T, rhs.S, rhs.pos, rhs.idx, rhs.suffix);
}

#endif  // EXPRESSION_H
