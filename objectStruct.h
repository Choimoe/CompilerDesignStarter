#ifndef OBJECTSTRUCT_H
#define OBJECTSTRUCT_H

#include <string>
#include <utility>

namespace objectStruct {
// MemoryNode represents a memory entity with attributes such as name, type,
// offset, and value
struct MemoryNode {
    std::string name;  // The name of the memory node
    int type;          // The type of the memory node
    int offset;        // The offset of the memory node
    int value;         // The value of the memory node

    // Constructor for initializing the fields of a MemoryNode
    MemoryNode(std::string nodeName,
               int nodeType,
               int nodeOffset,
               int nodeValue)
        : name(std::move(nodeName)),
          type(nodeType),
          offset(nodeOffset),
          value(nodeValue) {}

    // Default constructor
    MemoryNode() : type(0), offset(0), value(0) {}
};

// UsageInfo represents the usage and lifetime information of a variable
struct UsageInfo {
    int usageStatus;  // Indicates if the variable is used (-1 for unset, 1 for
                      // used, 0 for not used)
    int lifetime;  // Indicates the lifetime status (-1 for unset, 1 for active,
                   // 0 for inactive)

    // Constructor for initializing the fields of a UsageInfo
    UsageInfo(int usage, int life) : usageStatus(usage), lifetime(life) {}

    // Default constructor
    UsageInfo() : usageStatus(-1), lifetime(-1) {}
};

}  // namespace objectStruct

#endif  // PARSERSTRUCT_H
