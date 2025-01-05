#include "objectGen.h"
#include <climits>

std::string ObjectCodeGenerator::generate(const std::string& input) {
    std::istringstream iss(input);
    parseInput(iss);
    if (quadruples.empty()) {
        return "halt\n";
    }

    analyzeBlocks();
    analyzeVariableUsage();
    generateCode();
    return formatOutput();
}

void ObjectCodeGenerator::parseInput(std::istringstream& iss) {
    int n = 0;
    iss >> n;

    for (int i = 0; i < n; i++) {
        objectStruct::MemoryNode node;
        std::string t;
        iss >> node.name >> node.type >> t >> node.offset;
        symbolTable.push_back(node);
        variableSet.insert(node.name);
    }

    if (!symbolTable.empty()) {
        offset = symbolTable.back().offset;
        offset += (symbolTable.back().type == 0) ? 4 : 8;
    }

    iss >> temporaryVarSize;
    temporaryVariables.resize(temporaryVarSize);

    iss >> numberOfQuadruples;
    labelFlags.resize(numberOfQuadruples);
    resultCode.resize(numberOfQuadruples);

    std::string line;
    std::getline(iss, line);
    for (int i = 0; i < numberOfQuadruples; i++) {
        std::getline(iss, line);
        quadruples.push_back(parserUtil::parseQuadTuple(line));
    }
}

void ObjectCodeGenerator::analyzeBlocks() {
    std::vector<bool> basicBlockEntryPoints(quadruples.size(), false);
    basicBlockEntryPoints[0] = true;

    // Identify all basic block entry points
    for (size_t quadIndex = 0; quadIndex < quadruples.size(); ++quadIndex) {
        const auto& currentQuad = quadruples[quadIndex];

        // Using string_view for more efficient string operations
        std::string_view operation{currentQuad.operation};

        // Check for jump instructions
        if (operation[0] == 'j') {
            const auto targetIndex = std::stoi(currentQuad.destination);
            basicBlockEntryPoints[targetIndex] = true;

            // For conditional jumps, mark the fall-through block
            if (operation != "j" && quadIndex < quadruples.size() - 1) {
                basicBlockEntryPoints[quadIndex + 1] = true;
            }
        }

        // Mark I/O operations as block boundaries
        if (operation == "W" || operation == "R") {
            basicBlockEntryPoints[quadIndex] = true;
        }
    }

    // Construct basic blocks
    blocks.clear();  // Ensure blocks container is empty

    for (size_t currentIndex = 0;
         currentIndex < basicBlockEntryPoints.size();) {
        if (!basicBlockEntryPoints[currentIndex]) {
            ++currentIndex;
            continue;
        }

        // Find the end of the current basic block
        auto blockEndIndex = currentIndex + 1;
        for (; blockEndIndex < basicBlockEntryPoints.size(); ++blockEndIndex) {
            const auto& prevQuad = quadruples[blockEndIndex - 1];
            if (basicBlockEntryPoints[blockEndIndex] ||
                prevQuad.operation[0] == 'j' || prevQuad.operation == "End") {
                break;
            }
        }

        // Create the basic block
        blocks.emplace_back(currentIndex, blockEndIndex - 1);
        currentIndex = blockEndIndex;
    }
}

void ObjectCodeGenerator::analyzeVariableUsage() {
    // Initialize data structures with meaningful names
    usageTable.resize(quadruples.size());
    memoryUsage.resize(symbolTable.size(), {-1, 1});
    temporaryUsage.resize(temporaryVarSize, {-1, 0});

    // Process each basic block
    for (const auto& block : blocks) {
        // Analyze instructions in reverse order within each block
        for (int quadIndex = block.second; quadIndex >= block.first;
             --quadIndex) {
            const auto& quad = quadruples[quadIndex];

            // Create array of arguments for uniform processing
            const std::array<std::string_view, 3> quadArguments{
                quad.argument1, quad.argument2, quad.destination};

            // Process each argument in reverse order
            for (int argIndex = 2; argIndex >= 0; --argIndex) {
                const auto& currentArg = quadArguments[argIndex];

                if (currentArg.empty() || currentArg[0] != 'T') {
                    continue;
                }

                const bool isDestination = (argIndex == 2);

                if (currentArg[1] == 'B') {  // Symbol table variable
                    const auto symbolIndex =
                        std::stoi(std::string(currentArg.substr(2)));
                    usageTable[quadIndex][argIndex] = memoryUsage[symbolIndex];
                    memoryUsage[symbolIndex] = {isDestination ? -1 : quadIndex,
                                                isDestination ? 0 : 1};
                } else {  // Temporary variable
                    const auto underscore_pos = currentArg.find('_');
                    const auto tempIndex = std::stoi(
                        std::string(currentArg.substr(1, underscore_pos - 1)));
                    usageTable[quadIndex][argIndex] = temporaryUsage[tempIndex];
                    temporaryUsage[tempIndex] = {isDestination ? -1 : quadIndex,
                                                 isDestination ? 0 : 1};
                }
            }
        }
    }
}

void ObjectCodeGenerator::generateCode() {
    for (const auto& [blockStart, blockEnd] : blocks) {
        // Clear register allocation state for new block
        for (auto& pair : registerValues) {
            pair.second.clear();
        }
        for (auto& pair : availableExpressions) {
            pair.second.clear();
        }
        usePosition.clear();

        // Process each quadruple in the block
        for (int i = blockStart; i <= blockEnd; i++) {
            const auto& quad = quadruples[i];

            if (quad.operation[0] != 'j' && quad.operation != "W" &&
                quad.operation != "R" && quad.operation != "End") {
                handleArithmeticOperation(quad, i);
            } else if (quad.operation == "R" || quad.operation == "W") {
                handleIOOperation(quad, i);
            } else {
                handleJumpOperation(quad, i);
            }
        }

        saveActiveVariables(blockEnd);
    }
}

void ObjectCodeGenerator::handleArithmeticOperation(
    const parserStruct::QuadTuple& quad,
    int index) {
    updateUsePosition(quad.argument1, usageTable[index][0].usageStatus);
    updateUsePosition(quad.argument2, usageTable[index][1].usageStatus);
    updateUsePosition(quad.destination, usageTable[index][2].usageStatus);

    std::string targetReg = allocateRegister(quad, index);
    std::string arg1 = findRegister(quad.argument1);
    std::string arg2 = quad.argument2;
    if (arg2 != "-") {
        arg2 = findRegister(arg2);
    }

    if (arg1 == targetReg) {
        if (arg2 != "-") {
            std::string operand2 = (arg2[0] == 'T') ? getAddress(arg2) : arg2;
            transferOperation(quad.operation, targetReg, operand2, index);
        }
        if (quad.operation == "!") {
            resultCode[index].push_back("not " + arg1);
        }
        availableExpressions[quad.argument1].erase(targetReg);
    } else {
        std::string operand1 = (arg1[0] == 'T') ? getAddress(arg1) : arg1;
        resultCode[index].push_back("mov " + targetReg + ", " + operand1);

        if (arg2 != "-") {
            std::string operand2 = (arg2[0] == 'T') ? getAddress(arg2) : arg2;
            transferOperation(quad.operation, targetReg, operand2, index);
        }
    }

    // Update register allocation state
    if (arg2 == targetReg) {
        availableExpressions[quad.argument2].erase(targetReg);
    }
    registerValues[targetReg].clear();
    registerValues[targetReg].insert(quad.destination);
    availableExpressions[quad.destination].clear();
    availableExpressions[quad.destination].insert(targetReg);
}

void ObjectCodeGenerator::handleJumpOperation(
    const parserStruct::QuadTuple& quad,
    int index) {
    if (quad.operation == "j") {
        resultCode[index].push_back("jmp ?" + quad.destination);
        if (!labelFlags[std::stoi(quad.destination)]) {
            labelFlags[std::stoi(quad.destination)] = true;
        }
    } else if (quad.operation == "jnz") {
        std::string reg = quad.argument1;
        reg = findRegister(reg);

        if (reg == quad.argument1) {
            reg = allocateRegister(quad, index);
            resultCode[index].push_back("mov " + reg + ", " +
                                        getAddress(quad.argument1));
        }

        resultCode[index].push_back("cmp " + reg + ", 0");
        resultCode[index].push_back("jne ?" + quad.destination);

        if (!labelFlags[std::stoi(quad.destination)]) {
            labelFlags[std::stoi(quad.destination)] = true;
        }
    } else if (quad.operation == "End") {
        resultCode[index].push_back("halt");
    } else {  // Other conditional jumps
        std::string arg1 = findRegister(quad.argument1);
        std::string arg2 = findRegister(quad.argument2);

        if (arg1 == quad.argument1) {
            arg1 = allocateRegister(quad, index);
            resultCode[index].push_back("mov " + arg1 + ", " +
                                        getAddress(quad.argument1));
        }

        if (arg2[0] == 'T') {
            resultCode[index].push_back("cmp " + arg1 + ", " +
                                        getAddress(arg2));
        } else {
            resultCode[index].push_back("cmp " + arg1 + ", " + arg2);
        }

        resultCode[index].push_back(OpKeyMap::jumpAssembler.at(quad.operation) +
                                    " ?" + quad.destination);

        if (!labelFlags[std::stoi(quad.destination)]) {
            labelFlags[std::stoi(quad.destination)] = true;
        }
    }
}

void ObjectCodeGenerator::handleIOOperation(const parserStruct::QuadTuple& quad,
                                            int index) {
    if (quad.operation == "R") {
        resultCode[index].push_back("jmp ?read(" +
                                    getAddress(quad.destination) + ")");
    } else {  // quad.operation == "W"
        resultCode[index].push_back("jmp ?write(" +
                                    getAddress(quad.destination) + ")");
    }
}

std::string ObjectCodeGenerator::getAddress(const std::string& variable) {
    std::string result = "[ebp-";
    int varOffset = 0;

    if (variable[0] == 'T' && variable[1] == 'B') {
        int idx = std::stoi(variable.substr(2));
        varOffset = symbolTable[idx].offset;
    } else {
        size_t pos = variable.find('_');
        int idx = std::stoi(variable.substr(1, pos - 1));

        if (temporaryVariables[idx] != 0) {
            varOffset = temporaryVariables[idx];
        } else {
            if (variable[variable.size() - 1] == 'i') {
                offset += 4;
                varOffset = offset - 4;
            } else {
                offset += 8;
                varOffset = offset - 8;
            }
            availableExpressions[variable].insert(variable);
            temporaryVariables[idx] = varOffset;
        }
    }

    return result + std::to_string(varOffset) + "]";
}

std::string ObjectCodeGenerator::formatOutput() const {
    std::ostringstream oss;

    for (size_t i = 0; i < blocks.size(); ++i) {
        const auto& [start, end] = blocks[i];

        if (labelFlags[start]) {
            oss << "?" << start << ":\n";
        }

        for (int j = start; j <= end; ++j) {
            for (const auto& instruction : resultCode[j]) {
                oss << instruction << "\n";
            }
        }
    }

    return oss.str();
}

void ObjectCodeGenerator::updateUsePosition(const std::string& variable,
                                            int status) {
    if (variable[0] == 'T') {
        usePosition[variable] = (status == -1) ? INT_MAX : status;
    }
}

void ObjectCodeGenerator::saveActiveVariables(int blockEnd) {
    for (size_t i = 0; i < symbolTable.size(); i++) {
        std::string var = "TB" + std::to_string(i);
        if (!availableExpressions[var].empty() &&
            availableExpressions[var].count(var) == 0) {
            for (const auto& reg : availableExpressions[var]) {
                if (reg[0] == 'R') {
                    resultCode[blockEnd].push_back(
                        "mov [ebp-" + std::to_string(symbolTable[i].offset) +
                        "], " + reg);
                    break;
                }
            }
        }
    }
}

void ObjectCodeGenerator::transferOperation(const std::string& operation,
                                            const std::string& x,
                                            const std::string& y,
                                            int index) {
    auto it = OpKeyMap::optAssembler.find(operation);
    if (it != OpKeyMap::optAssembler.end()) {
        resultCode[index].push_back(it->second + x + ", " + y);

        it = OpKeyMap::setAssembler.find(operation);
        if (it != OpKeyMap::setAssembler.end()) {
            resultCode[index].push_back(OpKeyMap::setAssembler.at(operation) +
                                        x);
        }
    }
}

std::string ObjectCodeGenerator::findRegister(const std::string& variable) {
    // Check if the variable is already in a register
    for (const auto& reg : availableExpressions[variable]) {
        if (reg[0] == 'R') {
            return reg;
        }
    }
    return variable;  // Return original variable if no register is found
}

std::string ObjectCodeGenerator::allocateRegister(
    const parserStruct::QuadTuple& quad,
    int quadIndex) {
    // Special case: no register allocation needed for jumps and I/O operations
    if (quad.operation[0] == 'j' || quad.operation == "W" ||
        quad.operation == "R" || quad.operation == "End") {
        return "R0";  // Default register for simple operations
    }

    // Try to reuse register if possible
    for (const auto& reg : availableExpressions[quad.argument1]) {
        const auto& values = registerValues[reg];
        bool isSingleReference =
            (values.size() == 1 && *values.begin() == quad.argument1);
        bool isNotLiving = (quad.argument1 == quad.destination ||
                            usageTable[quadIndex][0].lifetime == 0);

        if (isSingleReference && isNotLiving) {
            return reg;
        }
    }

    // Check for empty registers
    const std::string registers[] = {"R0", "R1", "R2"};
    for (const auto& reg : registers) {
        if (registerValues[reg].empty()) {
            return reg;
        }
    }

    // Find best register to spill based on usage patterns
    std::string selectedReg;

    // First try: find register with all variables already in memory
    auto isInMemory = [this](const std::string& var) {
        return availableExpressions[var].count(var) > 0;
    };

    for (const auto& reg : registers) {
        bool allInMemory = std::all_of(registerValues[reg].begin(),
                                       registerValues[reg].end(), isInMemory);

        if (allInMemory) {
            selectedReg = reg;
            break;
        }
    }

    // Second try: find register with variables used farthest in future
    if (selectedReg.empty()) {
        int maxUseDistance = -1;

        for (const auto& reg : registers) {
            int minUseDistance = std::numeric_limits<int>::max();

            // Find the earliest usage of any variable in this register
            for (const auto& var : registerValues[reg]) {
                minUseDistance = std::min(minUseDistance, usePosition[var]);
            }

            // Select register with variables used farthest in future
            if (minUseDistance > maxUseDistance) {
                selectedReg = reg;
                maxUseDistance = minUseDistance;
            }
        }
    }

    // Save current register contents to memory if necessary
    for (const auto& var : registerValues[selectedReg]) {
        if (!availableExpressions[var].count(var) && var != quad.destination) {
            resultCode[quadIndex].push_back("mov " + getAddress(var) + ", " +
                                            selectedReg);
        }

        // Update variable locations
        if (var == quad.argument1 ||
            (var == quad.argument2 &&
             registerValues[selectedReg].count(quad.argument1))) {
            availableExpressions[var] = {var, selectedReg};
        } else {
            availableExpressions[var] = {var};
        }
    }

    // Clear register's current values
    registerValues[selectedReg].clear();

    return selectedReg;
}