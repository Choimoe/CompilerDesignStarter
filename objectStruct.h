#ifndef OBJECTSTRUCT_H
#define OBJECTSTRUCT_H

#include <string>
#include <utility>

namespace objectStruct {

/**
 * @struct MemoryNode
 * @brief Represents a memory entity with attributes such as name, type, offset,
 * and value.
 */
struct MemoryNode {
    std::string name;  ///< The name of the memory node.
    int type;          ///< The type of the memory node.
    int offset;        ///< The offset of the memory node.
    int value;         ///< The value of the memory node.

    /**
     * @brief Constructs a MemoryNode with specified attributes.
     *
     * @param nodeName The name of the memory node.
     * @param nodeType The type of the memory node.
     * @param nodeOffset The offset of the memory node.
     * @param nodeValue The value of the memory node.
     */
    MemoryNode(std::string nodeName,
               int nodeType,
               int nodeOffset,
               int nodeValue)
        : name(std::move(nodeName)),
          type(nodeType),
          offset(nodeOffset),
          value(nodeValue) {}

    /**
     * @brief Default constructor for MemoryNode.
     *
     * Initializes type, offset, and value to 0, and name to an empty string.
     */
    MemoryNode() : type(0), offset(0), value(0) {}
};

/**
 * @struct UsageInfo
 * @brief Represents the usage and lifetime information of a variable.
 */
struct UsageInfo {
    int usageStatus;  ///< Indicates if the variable is used (-1 for unset, 1
                      ///< for used, 0 for not used).
    int lifetime;     ///< Indicates the lifetime status (-1 for unset, 1 for
                      ///< active, 0 for inactive).

    /**
     * @brief Constructs a UsageInfo with specified usage and lifetime statuses.
     *
     * @param usage The usage status of the variable.
     * @param life The lifetime status of the variable.
     */
    UsageInfo(int usage, int life) : usageStatus(usage), lifetime(life) {}

    /**
     * @brief Default constructor for UsageInfo.
     *
     * Initializes usageStatus and lifetime to -1.
     */
    UsageInfo() : usageStatus(-1), lifetime(-1) {}
};

}  // namespace objectStruct

#endif  // OBJECTSTRUCT_H
