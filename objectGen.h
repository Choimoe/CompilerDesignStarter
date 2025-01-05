#ifndef OBJGEN_H
#define OBJGEN_H

#include <algorithm>
#include <iostream>
#include <cstring>
#include <utility>
#include <limits>
#include <vector>
#include <set>
#include <sstream>
#include "objectStruct.h"
#include "parserStruct.h"
#include "parserUtil.h"
#include "opkeymap.h"

/**
 * @class ObjectCodeGenerator
 * @brief This class generates object code from intermediate representations
 * such as quadruples.
 *
 * The ObjectCodeGenerator is responsible for parsing input, analyzing basic
 * blocks, allocating registers, and generating optimized object code. It
 * manages symbol tables, temporary variables, and usage analysis for efficient
 * code generation.
 */
class ObjectCodeGenerator {
public:
    /**
     * @brief Default constructor.
     */
    ObjectCodeGenerator() = default;

    /**
     * @brief Generates object code from the provided intermediate
     * representation.
     *
     * @param input The intermediate representation as a string.
     * @return A formatted string containing the generated object code.
     */
    std::string generate(const std::string& input);

private:
    // Data members

    /// Symbol table containing memory nodes representing variables and their
    /// attributes.
    std::vector<objectStruct::MemoryNode> symbolTable;

    /// Set of all variable names encountered during code generation.
    std::set<std::string> variableSet;

    /// Vector of quadruples representing the intermediate representation.
    std::vector<parserStruct::QuadTuple> quadruples;

    /// Basic blocks represented as pairs of start and end indices.
    std::vector<std::pair<int, int>> blocks;

    /// Tracks the last usage position of variables for optimization purposes.
    std::unordered_map<std::string, int> usePosition;

    /// Maps available expressions to their respective sets of variables for
    /// reuse.
    std::unordered_map<std::string, std::set<std::string>> availableExpressions;

    /// Maps registers to their currently held variable values.
    std::unordered_map<std::string, std::set<std::string>> registerValues;

    /// Vector to store the generated object code, organized by lines.
    std::vector<std::vector<std::string>> resultCode;

    /// Flags indicating whether a label exists at a given quadruple index.
    std::vector<bool> labelFlags;

    /// Temporary variables used during code generation.
    std::vector<int> temporaryVariables;

    /// Offset for memory allocation during symbol table processing.
    int offset = 0;

    /// Total number of quadruples in the intermediate representation.
    int numberOfQuadruples = 0;

    /// Size of the temporary variable pool.
    int temporaryVarSize = 0;

    // Utility functions

    /**
     * @brief Parses the input string into intermediate structures such as
     * quadruples.
     *
     * @param iss Input string stream containing the intermediate
     * representation.
     */
    void parseInput(std::istringstream& iss);

    /**
     * @brief Analyzes basic blocks within the intermediate representation.
     */
    void analyzeBlocks();

    /**
     * @brief Generates object code from the analyzed intermediate
     * representation.
     */
    void generateCode();

    /**
     * @brief Formats the generated object code into a final string output.
     *
     * @return The formatted object code as a string.
     */
    std::string formatOutput() const;

    // Register allocation and variable handling

    /**
     * @brief Retrieves the memory address of a given variable.
     *
     * @param variable The variable name.
     * @return A string representation of the variable's memory address.
     */
    std::string getAddress(const std::string& variable);

    /**
     * @brief Allocates a register for a specific quadruple operation.
     *
     * @param quad The quadruple requiring a register.
     * @param quadIndex The index of the quadruple in the list.
     * @return The name of the allocated register.
     */
    std::string allocateRegister(const parserStruct::QuadTuple& quad,
                                 int quadIndex);

    /**
     * @brief Finds the register currently holding a specific variable's value.
     *
     * @param variable The variable name.
     * @return The name of the register holding the variable, or an empty string
     * if none.
     */
    std::string findRegister(const std::string& variable);

    // Code generation helpers

    /**
     * @brief Handles the generation of object code for arithmetic operations.
     *
     * @param quad The quadruple representing the arithmetic operation.
     * @param index The index of the quadruple in the list.
     */
    void handleArithmeticOperation(const parserStruct::QuadTuple& quad,
                                   int index);

    /**
     * @brief Handles the generation of object code for jump operations.
     *
     * @param quad The quadruple representing the jump operation.
     * @param index The index of the quadruple in the list.
     */
    void handleJumpOperation(const parserStruct::QuadTuple& quad, int index);

    /**
     * @brief Handles the generation of object code for input/output operations.
     *
     * @param quad The quadruple representing the I/O operation.
     * @param index The index of the quadruple in the list.
     */
    void handleIOOperation(const parserStruct::QuadTuple& quad, int index);

    /**
     * @brief Transfers an operation to object code with the given operands.
     *
     * @param operation The operation type (e.g., ADD, SUB).
     * @param x The first operand.
     * @param y The second operand.
     * @param index The index of the operation in the quadruples list.
     */
    void transferOperation(const std::string& operation,
                           const std::string& x,
                           const std::string& y,
                           int index);

    // Usage analysis

    /**
     * @brief Analyzes variable usage across all quadruples for optimization.
     */
    void analyzeVariableUsage();

    /**
     * @brief Updates the usage position of a variable in the usePosition map.
     *
     * @param variable The variable name.
     * @param status The new usage position of the variable.
     */
    void updateUsePosition(const std::string& variable, int status, int mask=0);

    // Memory management

    /**
     * @brief Saves active variables to memory at the end of a block.
     *
     * @param blockEnd The index marking the end of the block.
     */
    void saveActiveVariables(int blockEnd);

    // Variable tracking

    /// Tracks variable usage information for registers, memory, and
    /// temporaries.
    std::vector<std::array<objectStruct::UsageInfo, 3>> usageTable;

    /// Usage information for memory variables.
    std::vector<objectStruct::UsageInfo> memoryUsage;

    /// Usage information for temporary variables.
    std::vector<objectStruct::UsageInfo> temporaryUsage;
};

#endif
