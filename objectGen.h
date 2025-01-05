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

class ObjectCodeGenerator {
public:
    ObjectCodeGenerator() = default;
    std::string generate(const std::string& input);

private:
    // Data members
    std::vector<objectStruct::MemoryNode> symbolTable;
    std::set<std::string> variableSet;
    std::vector<parserStruct::QuadTuple> quadruples;
    std::vector<std::pair<int, int>> blocks;
    std::unordered_map<std::string, int> usePosition;
    std::unordered_map<std::string, std::set<std::string>> availableExpressions;
    std::unordered_map<std::string, std::set<std::string>> registerValues;
    std::vector<std::vector<std::string>> resultCode;
    std::vector<bool> labelFlags;
    std::vector<int> temporaryVariables;
    int offset = 0;
    int numberOfQuadruples = 0;
    int temporaryVarSize = 0;

    // Utility functions
    void parseInput(std::istringstream& iss);
    void analyzeBlocks();
    void generateCode();
    std::string formatOutput() const;

    // Register allocation and variable handling
    std::string getAddress(const std::string& variable);
    std::string allocateRegister(const parserStruct::QuadTuple& quad,
                                 int quadIndex);
    std::string findRegister(const std::string& variable);

    // Code generation helpers
    void handleArithmeticOperation(const parserStruct::QuadTuple& quad,
                                   int index);
    void handleJumpOperation(const parserStruct::QuadTuple& quad, int index);
    void handleIOOperation(const parserStruct::QuadTuple& quad, int index);
    void transferOperation(const std::string& operation,
                           const std::string& x,
                           const std::string& y,
                           int index);

    // Usage analysis
    void analyzeVariableUsage();
    void updateUsePosition(const std::string& variable, int status);

    // Memory management
    void saveActiveVariables(int blockEnd);

    // Variable tracking
    std::vector<std::array<objectStruct::UsageInfo, 3>> usageTable;
    std::vector<objectStruct::UsageInfo> memoryUsage;
    std::vector<objectStruct::UsageInfo> temporaryUsage;
};

#endif