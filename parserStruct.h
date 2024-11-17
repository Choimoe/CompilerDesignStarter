#ifndef PARSERSTRUCT_H
#define PARSERSTRUCT_H

#include <string>
#include <utility>

namespace parserStruct {

// QuadTuple structure represents a single instruction in the intermediate code
// (three-address code). It stores the operation, operands, destination, and an
// optional field for extended use.
struct QuadTuple {
    std::string
        operation;  // The operation or operator (e.g., "+", "-", "=", etc.)
    std::string argument1;    // The first operand
    std::string argument2;    // The second operand
    std::string destination;  // The result or destination for the operation
    int Xfour;  // An optional field used for extended purposes (e.g.,
                // backpatching)

    // Constructor for initializing the fields of a QuadTuple
    QuadTuple(std::string op,
              std::string arg1,
              std::string arg2,
              std::string dest,
              int x4 = -1)
        : operation(std::move(op)),
          argument1(std::move(arg1)),
          argument2(std::move(arg2)),
          destination(std::move(dest)),
          Xfour(x4) {}

    // Default constructor, initializes Xfour to -1
    QuadTuple() : Xfour(-1) {}
};

// Symbol structure represents an entry in the symbol table.
// It stores information about a symbol such as its name, type, and memory
// offset.
struct Symbol {
    std::string name;  // The name of the symbol (e.g., variable name)
    std::string type;  // The type of the symbol (e.g., int, float, etc.)
    int offset;        // The memory offset of the symbol (used for addressing)

    // Constructor for initializing a Symbol with name, type, and offset
    Symbol(std::string n, std::string t, int off)
        : name(std::move(n)), type(std::move(t)), offset(off) {}

    // Default constructor, initializes offset to 0
    Symbol() : offset(0) {}
};

// Attribute structure represents an attribute associated with a symbol in the
// parsing process. It stores various information including the quad number,
// jump lists, width, and variable name/type.
struct Attribute {
    int quad;  // The quad number (used to reference a specific instruction in
               // intermediate code)
    int nextlist;      // The address of the next instruction in the linked list
                       // (used for jump operations)
    int truelist;      // The address of the jump list for true conditions
    int falselist;     // The address of the jump list for false conditions
    int width;         // The width or size of the data (in bytes)
    std::string name;  // The name of the variable (or temporary variable)
    std::string type;  // The type of the variable (e.g., int, float)
    std::string op;    // The operator used in the expression
    std::string place;  // The location where the value is stored (e.g.,
                        // register or temporary variable)

    // Default constructor initializes all fields to default values
    Attribute() : quad(0), nextlist(0), truelist(0), falselist(0), width(0) {}
};

}  // namespace parserStruct

#endif  // PARSERSTRUCT_H
