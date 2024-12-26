#include "parser.h"
#include "opkeymap.h"
#include "parserUtil.h"
#include <cassert>
#include <sstream>

constexpr int BUFFER = 1000;

// Constructor for the Parser class
Parser::Parser()
    : parserState(0), gotoTable(BUFFER, std::vector<int>(BUFFER, 0)) {}

// Computes the suffix for a given expression I
std::set<std::string> Parser::compute_suffix(const Expression& I) {
    std::set<std::string> suffix;
    bool is_empty = true;
    size_t i = I.pos + 1;  // Start looking after the current position in I.S

    // Iterate through the symbols in I starting from the position I.pos + 1
    while (is_empty && i < I.S.size()) {
        is_empty = false;
        const std::string& symbol = I.S[i];

        // If the symbol is a terminal, add it to the suffix set
        if (OpKeyMap::terminal.count(symbol)) {
            suffix.emplace(symbol);
            continue;
        }

        // If the symbol is non-terminal, add its corresponding first set to the
        // suffix set
        for (const std::string& w : firstSets[symbol]) {
            if (w == "^") {
                is_empty = true;  // If the symbol is epsilon, mark as empty
            } else {
                suffix.emplace(w);  // Add the symbol to suffix
            }
        }

        // If the suffix is empty, move to the next symbol
        if (is_empty) {
            ++i;
        }
    }

    // If we still have an empty suffix, add the suffix from expression I
    if (is_empty) {
        for (const std::string& w : I.suffix) {
            suffix.emplace(w);
        }
    }

    return suffix;  // Return the computed suffix set
}

// Adds the non-terminal closure for the current expression I
void Parser::add_non_terminal_closure(const Expression& I,
                                      std::set<Expression>& pre,
                                      const std::string& cur_str) {
    // Loop through the grammar rules for the current non-terminal
    for (const auto& X : grammarRules[cur_str]) {
        auto x = X.first;
        auto y = X.second;

        // If the position is at the end of the expression, add closure with the
        // suffix
        if (I.pos == I.S.size() - 1) {
            pre.emplace(cur_str, x, 0, y, I.suffix);
        } else {
            std::string nxt_str = I.S[I.pos + 1];

            // If the next symbol is terminal, use its first set; otherwise,
            // compute suffix
            pre.emplace(cur_str, x, 0, y,
                        OpKeyMap::terminal.count(nxt_str) ? firstSets[nxt_str]
                                                          : compute_suffix(I));
        }
    }
}

std::set<Expression> Parser::get_closure(const std::set<Expression>& cur) {
    auto pre = cur;
    auto result = cur;

    auto merge_closures = [&](const std::set<Expression>& pre) {
        for (auto I : pre) {
            // Check if the expression already exists in the result set
            auto it = std::find_if(result.begin(), result.end(),
                                   [&](const Expression& e) {
                                       return e.idx == I.idx && e.pos == I.pos;
                                   });

            // If it exists, merge the suffix
            if (it != result.end()) {
                for (const auto& w : it->suffix) {
                    I.suffix.emplace(w);
                }
                result.erase(it);
            }

            // Add the new expression to the result set
            result.emplace(I.T, I.S, I.pos, I.idx, I.suffix);
        }
    };

    // Lambda function to compute the closure and return whether changes were
    // made
    auto process_closure = [&]() -> bool {
        size_t sz = result.size();

        // Loop through each expression in the current set of expressions
        for (const auto& I : result) {
            // If the position is already at the end of the expression, skip it
            if (I.pos >= I.S.size()) {
                continue;
            }

            std::string cur_str = I.S[I.pos];

            // If the current symbol is non-terminal, add its closure
            if (!OpKeyMap::terminal.count(cur_str)) {
                add_non_terminal_closure(I, pre, cur_str);
            }
        }

        // Merge the computed closures using the lambda
        merge_closures(pre);

        // If the result set has grown, return true to indicate further
        // processing is needed
        return result.size() > sz;
    };

    // Keep processing until no more changes happen
    while (process_closure())
        ;

    return result;  // Return the computed closure
}

// Initializes the symbol-to-ID mapping for both terminal and non-terminal
// symbols
void Parser::init_symbol_mapping() {
    int cnt = 0;

    // Map non-terminal symbols to unique IDs
    for (const auto& [x, _] : OpKeyMap::Non_terminal) {
        symbolToIdMap[x] = cnt++;
    }

    // Map terminal symbols to unique IDs
    for (const auto& [x, _] : OpKeyMap::terminal) {
        symbolToIdMap[x] = cnt++;
    }
}

// Parses a list of expressions and updates the grammar rules
void Parser::parse_expressions(std::vector<std::string>& tmp) {
    std::string str;
    int cnt = 1;

    // Loop through each expression in the input vector
    for (const auto& x : tmp) {
        std::stringstream S(x);
        std::string K, _;
        std::vector<std::string> h;

        // Read the left-hand side of the rule and discard the separator
        S >> K >> _;

        // Read the right-hand side of the rule and store the symbols
        while (S >> str) {
            h.emplace_back(str);
        }

        // Add the parsed rule to the grammar rules and the flattened grammar
        grammarRules[K].emplace_back(h, cnt++);
        flattenedGrammarRules.emplace_back(K, h);
    }
}

// Computes the FIRST sets for all non-terminal symbols in the grammar
void Parser::compute_first_set(std::vector<std::string>& tmp) {
    // Initialize FIRST sets for terminal symbols by adding the terminal itself
    for (const auto& [x, _] : OpKeyMap::terminal) {
        firstSets[x].emplace(x);
    }
    auto check_empty = [&](const auto& s) -> bool {
        bool ret = true;

        // Process the symbols in the production
        for (size_t i = 0; i < s.size(); ++i) {
            ret = false;

            // Add the FIRST sets of the symbols to tmp
            for (const auto& xx : firstSets[s[i]]) {
                if (xx == "^") {
                    ret = true;  // If epsilon, mark as empty
                } else {
                    tmp.push_back(xx);  // Add non-epsilon symbols to tmp
                }
            }

            if (!ret)
                return false;
        }

        return true;
    };

    auto compute_step = [&]() -> bool {
        // Iterate through all grammar rules
        for (const auto& [x, y] : grammarRules) {
            size_t prev_size = firstSets[x].size();

            // Process each production in the grammar rule
            for (const auto& [s, _] : y) {
                // If the production is empty (epsilon), add "^" to tmp
                if (check_empty(s)) {
                    tmp.push_back("^");
                }

                // Add the symbols in tmp to the FIRST set of the current
                // non-terminal
                for (const auto& xx : tmp) {
                    firstSets[x].insert(xx);
                }
                tmp.clear();
            }

            // If the FIRST set size increased, continue processing
            if (prev_size < firstSets[x].size()) {
                return true;
            }
        }
        return false;
    };

    while (compute_step())
        ;
}

// Retrieves the item sets and symbols for a given index in itemSets
std::pair<std::vector<std::set<Expression>>, std::vector<std::string>>
Parser::get_Items(int idx) {
    std::set<std::string> symbols_to_process;
    std::vector<std::set<Expression>> item_sets;
    std::vector<std::string> symbols;

    // Collect symbols that are yet to be processed in the current item set
    for (auto I : itemSets[idx]) {
        if (I.pos < I.S.size() && I.S[I.pos] != "^") {
            symbols_to_process.insert(I.S[I.pos]);
        }
    }

    // For each symbol to process, create new item sets
    for (const auto& symbol : symbols_to_process) {
        std::set<Expression> tmp_set;
        for (const auto& I : itemSets[idx]) {
            if (I.pos < I.S.size() && I.S[I.pos] == symbol) {
                tmp_set.emplace(I.T, I.S, I.pos + 1, I.idx,
                                I.suffix);  // Update position
            }
        }
        item_sets.emplace_back(std::move(tmp_set));
        symbols.emplace_back(symbol);
    }

    // Return the item sets and corresponding symbols
    return std::make_pair(std::move(item_sets), std::move(symbols));
}

// Constructs the LR parsing table
void Parser::get_LR_table() {
    std::set<std::string> suffix;
    suffix.insert("#");      // End-of-input symbol
    std::string s = "PROG";  // Start symbol
    Expression e(s, grammarRules[s][0].first, 0, grammarRules[s][0].second,
                 suffix);
    std::set<Expression> st;
    st.insert(e);

    // Get the closure for the start symbol and initialize itemSets
    auto e1 = get_closure(st);
    itemSets.emplace_back(e1);

    int CN = 0;
    // Iterate over the item sets
    for (size_t i = 0; i < itemSets.size(); i++) {
        auto [sets, sets_words] = get_Items(i);  // Get items and symbols

        int cnt = 0;
        // Process each set of items
        for (const auto& x : sets) {
            auto do_word = sets_words[cnt++];

            // Get the closure of the current set of items
            auto nowset = get_closure(x);
            auto it = std::find(itemSets.begin(), itemSets.end(), nowset);
            size_t idx = (it == itemSets.end()) ? itemSets.size()
                                                : it - itemSets.begin();

            // If the closure is not in itemSets, add it
            if (it == itemSets.end()) {
                itemSets.emplace_back(nowset);
            }

            // Update the goto table with the index of the new set
            gotoTable[i][symbolToIdMap[do_word]] = idx;
            CN++;
        }
    }

    // Fill in the GoTo table for transitions
    size_t i = 0;
    for (const auto& x : itemSets) {
        for (const auto& I : x) {
            // If the item is a completed production, update the goto table with
            // suffix symbols
            if (I.pos >= I.S.size() || I.S[0] == "^") {
                for (const auto& y : I.suffix) {
                    gotoTable[i][symbolToIdMap[y]] =
                        -I.idx;  // Negative index for reductions
                }
            }
        }
        i++;
    }
}

// Generates the output for the parser, including symbol table, quadruples, and
// other details
std::vector<std::string> Parser::generateParserOutput(
    int nxt,
    const std::vector<parserStruct::QuadTuple>& quadruples,
    const std::vector<parserStruct::Symbol>& symbolTable,
    const std::vector<int>& tempVariableIndices) {
    std::vector<std::string> output;

    // Add the size of the symbol table
    output.push_back(std::to_string(symbolTable.size()) + "\n");

    // Add the symbol table entries
    for (const auto& symbol : symbolTable) {
        output.push_back(symbol.name + " " +
                         std::to_string(OpKeyMap::typr_to_int.at(symbol.type)) +
                         " " + "null " + std::to_string(symbol.offset) + "\n");
    }

    // Add the size of temporary variables
    output.push_back(std::to_string(tempVariableIndices.size()) + "\n");

    // Add the next available temporary variable index
    output.push_back(std::to_string(nxt) + "\n");

    // Add the quadruples to the output
    for (std::size_t i = 0; i < quadruples.size(); i++) {
        std::string line = std::to_string(i) + ": (" + quadruples[i].operation +
                           "," + quadruples[i].argument1 + "," +
                           quadruples[i].argument2 + ",";

        // Handle jump operations in quadruples
        if (quadruples[i].operation[0] == 'j') {
            if (quadruples[i].Xfour > 0)
                line += std::to_string(quadruples[i].Xfour);
            else
                line += std::to_string(quadruples.size() - 1);
        } else {
            line += quadruples[i].destination;
        }

        line += ")\n";
        output.push_back(line);
    }

    return output;  // Return the generated output as a vector of strings
}

#define POP_STACK_1()            \
    att1 = attributeStack.top(); \
    attributeStack.pop();
#define POP_STACK_2()            \
    POP_STACK_1()                \
    att2 = attributeStack.top(); \
    attributeStack.pop();
#define POP_STACK_3()            \
    POP_STACK_2()                \
    att3 = attributeStack.top(); \
    attributeStack.pop();
#define POP_STACK_4()            \
    POP_STACK_3()                \
    att4 = attributeStack.top(); \
    attributeStack.pop();
// General macro to pop 'n' elements from the attribute stack, dynamically
// determining which POP_STACK macro to use
#define POP_STACK(n)    \
    do {                \
        POP_STACK_##n() \
    } while (0)
// Macro to save an element (r) to the stack and also add the current quadruple
// to the 'quadruples' vector
#define SAVE_STACK(r)             \
    do {                          \
        quadruples.push_back(qt); \
        nxt++;                    \
        attributeStack.push(r);   \
    } while (0)
#define MAKE_QUAD(a, b, c, d) qt = parserStruct::QuadTuple((a), (b), (c), (d))
#define _GEN_EMPTY_CASE(t) auto case##t = [&](){}
#define _GEN_SYM_CASE(t, s) \
    auto case##t = [&](){attd.op = s; attributeStack.push(attd);}
#define _GEN_IO_CASE(t, p, f)                                               \
    auto case##t = [&](){                                                   \
        POP_STACK(p);                                                       \
        MAKE_QUAD(f, "-", "-", parserUtil::lookup(att1.name, symbolTable)); \
        SAVE_STACK(attd);}
std::vector<std::string> Parser::parser(std::vector<std::string>& res) {
    parserStruct::Attribute att1, att2, att3, att4, att5, attd;
    parserStruct::QuadTuple qt;
    parserStruct::Symbol symb;
    init_symbol_mapping();  // Initialize the symbol mappings (e.g., for
                            // tokenization)
    std::vector<std::string> tmp =
        OpKeyMap::language;  // Retrieve the language's operation key mappings
    parse_expressions(tmp);  // Parse expressions based on language rules
    tmp.clear();
    compute_first_set(tmp);  // Compute the first set for parsing
    get_LR_table();          // Get the LR parsing table for syntax analysis

    std::vector<std::pair<std::string, std::string>> temp(res.size());
    for (size_t i = 0; i < res.size(); i++) {
        std::stringstream S(res[i]);
        S >> temp[i].first;
        S >> temp[i].second;
    }

    temp.emplace_back(
        "#", "$");  // Add a special symbol to mark the end of the input
    stateStack.push(0);
    symbolStack.emplace("#");
    int offset = 0;  // Initialize offset (typically used for error handling or
                     // lookahead)
    int nxt = 0;     // Initialize the next quadruple index
    auto clear_attributes = [&]() {
        att1 = parserStruct::Attribute();
        att2 = parserStruct::Attribute();
        att3 = parserStruct::Attribute();
        att4 = parserStruct::Attribute();
        att5 = parserStruct::Attribute();
        attd = parserStruct::Attribute();
        qt = parserStruct::QuadTuple();
        symb = parserStruct::Symbol();
    };
    _GEN_EMPTY_CASE(0);
    _GEN_EMPTY_CASE(1);
    auto case2 = [&]() {
        POP_STACK(3);
        MAKE_QUAD("End", "-", "-", "-");
        quadruples.push_back(qt);
        nxt++;
        att2.name = "SUBPROG";
        attributeStack.push(att2);
    };
    auto case3 = [&]() {
        offset = 0;
        attributeStack.push(att1);
    };
    auto case4 = [&]() {
        att1.quad = nxt;
        attributeStack.push(att1);
    };
    auto case5 = [&]() { attributeStack.pop(); };
    _GEN_EMPTY_CASE(6);
    auto case7 = [&]() {
        att1.type = "int";
        att1.width = 4;
        attributeStack.push(att1);
    };
    auto case8 = [&]() {
        att1.type = "double";
        att1.width = 8;
        attributeStack.push(att1);
    };
    auto case9 = [&]() {
        att1.name = symbolStack.top();
        attributeStack.push(att1);
    };
    auto case11 = [&]() {
        POP_STACK(2);
        symb = parserStruct::Symbol(att1.name, att2.type, offset);

        bool isSameName = 0;
        for (auto& i : symbolTable) {
            if (symb.name == i.name) {
                isSameName = 1;
                break;
            }
        }
        if (isSameName) {
            std::cout << "Syntax Error\n";
            exit(0);
        }
        symbolTable.push_back(symb);
        offset += att2.width;
        att3.type = att2.type;
        att3.width = att2.width;
        attributeStack.push(att3);
    };
    auto case14 = [&]() {
        att1.nextlist = -1;
        attributeStack.pop();
        attributeStack.push(att1);
    };
    auto case15 = [&]() {
        att1.nextlist = -1;
        attributeStack.push(att1);
    };
    _GEN_EMPTY_CASE(16);
    auto case17 = [&]() {
        POP_STACK(4);
        parserUtil::backpatch(att1.nextlist, att4.quad, quadruples);
        parserUtil::backpatch(att3.truelist, att2.quad, quadruples);
        att5.nextlist = att3.falselist;
        attributeStack.push(att5);
        MAKE_QUAD("j", "-", "-", "");
        qt.Xfour = att4.quad;
        quadruples.push_back(qt);
        nxt++;
    };
    auto case18 = [&]() {
        POP_STACK(3);
        parserUtil::backpatch(att3.truelist, att2.quad, quadruples);
        att4.nextlist =
            parserUtil::merge(att3.falselist, att1.nextlist, quadruples);
        attributeStack.push(att4);
    };
    auto case19 = [&]() {
        POP_STACK(2);
        MAKE_QUAD("=", att1.place, "-",
                  parserUtil::lookup(att2.name, symbolTable));
        SAVE_STACK(att1);
    };
    auto case20 = [&]() {
        POP_STACK(3);
        parserUtil::backpatch(att3.nextlist, att2.quad, quadruples);
        att4.nextlist = att1.nextlist;
        attributeStack.push(att4);
    };
    _GEN_EMPTY_CASE(21);
    auto case22 = [&]() {
        POP_STACK(2);
        att3.place = parserUtil::Temp_New(0, tempVariableIndices);
        att3.type = "int";
        MAKE_QUAD("||", att2.place, att1.place, att3.place);
        SAVE_STACK(att3);
    };
    _GEN_EMPTY_CASE(23);
    auto case24 = [&]() {
        POP_STACK(2);
        att3.place = parserUtil::Temp_New(0, tempVariableIndices);
        att3.type = "int";
        MAKE_QUAD("&&", att2.place, att1.place, att3.place);
        SAVE_STACK(att3);
    };
    _GEN_EMPTY_CASE(25);
    _GEN_EMPTY_CASE(26);
    auto case27 = [&]() {
        POP_STACK(1);
        att3.place = parserUtil::Temp_New(0, tempVariableIndices);
        att3.type = "int";
        MAKE_QUAD("!", att1.place, "-", att3.place);
        SAVE_STACK(att3);
    };
    auto case28 = [&]() {
        POP_STACK(3);
        attd.place = parserUtil::Temp_New(0, tempVariableIndices);
        attd.type = "int";
        MAKE_QUAD(att2.op, att3.place, att1.place, attd.place);
        SAVE_STACK(attd);
    };
    _GEN_EMPTY_CASE(29);
    auto case30 = [&]() {
        POP_STACK(3);
        attd.place = parserUtil::Temp_New(OpKeyMap::typr_to_int.at(att3.type),
                                          tempVariableIndices);
        attd.type = att3.type;
        MAKE_QUAD(att2.op, att3.place, att1.place, attd.place);
        SAVE_STACK(attd);
    };
    _GEN_EMPTY_CASE(31);
    _GEN_EMPTY_CASE(32);
    auto case33 = [&]() {
        POP_STACK(3);
        attd.place = parserUtil::Temp_New(OpKeyMap::typr_to_int.at(att1.type),
                                          tempVariableIndices);
        attd.type = att1.type;
        MAKE_QUAD(att2.op, att3.place, att1.place, attd.place);
        SAVE_STACK(attd);
    };
    auto case34 = [&]() {
        POP_STACK(1);
        attd.place = parserUtil::lookup(att1.name, symbolTable);
        attd.type = parserUtil::look_up_type(att1.name, symbolTable);
        attributeStack.push(attd);
    };
    auto case35 = [&]() {
        attd.place = parserUtil::Temp_New(0, tempVariableIndices);
        attd.type = "int";
        MAKE_QUAD("=", symbolStack.top(), "-", attd.place);
        SAVE_STACK(attd);
    };
    auto case36 = [&]() {
        attd.place = parserUtil::Temp_New(1, tempVariableIndices);
        attd.type = "double";
        MAKE_QUAD("=", std::to_string(stof(symbolStack.top())), "-",
                  attd.place);
        SAVE_STACK(attd);
    };
    _GEN_EMPTY_CASE(37);
    auto case38 = [&]() {
        POP_STACK(2);
        attd.place = parserUtil::Temp_New(OpKeyMap::typr_to_int.at(att1.type),
                                          tempVariableIndices);
        attd.type = att1.type;
        MAKE_QUAD(att2.op, "0", att1.place, attd.place);
        SAVE_STACK(attd);
    };
    auto case39 = [&]() {
        POP_STACK(3);
        parserUtil::backpatch(att3.falselist, att2.quad, quadruples);
        attd.truelist =
            parserUtil::merge(att3.truelist, att1.truelist, quadruples);
        attd.falselist = att1.falselist;
        attributeStack.push(attd);
    };
    _GEN_EMPTY_CASE(40);
    auto case41 = [&]() {
        POP_STACK(3);
        parserUtil::backpatch(att3.truelist, att2.quad, quadruples);
        attd.falselist =
            parserUtil::merge(att3.falselist, att1.falselist, quadruples);
        attd.truelist = att1.truelist;
        attributeStack.push(attd);
    };
    _GEN_EMPTY_CASE(42);
    _GEN_EMPTY_CASE(43);
    auto case44 = [&]() {
        POP_STACK(1);
        attd.falselist = att1.truelist;
        attd.truelist = att1.falselist;
        attributeStack.push(attd);
    };
    auto case45 = [&]() {
        POP_STACK(3);
        attd.truelist = nxt;
        attd.falselist = nxt + 1;
        MAKE_QUAD("j" + att2.op, att3.place, att1.place, "0");
        quadruples.push_back(qt);
        nxt++;
        MAKE_QUAD("j", "-", "-", "0");
        SAVE_STACK(attd);
    };
    auto case46 = [&]() {
        POP_STACK(1);
        attd.truelist = nxt;
        attd.falselist = nxt + 1;
        MAKE_QUAD("jnz", att1.place, "-", "0");
        quadruples.push_back(qt);
        nxt++;
        MAKE_QUAD("j", "-", "-", "0");
        SAVE_STACK(attd);
    };
    auto case47 = [&]() {
        attd.place = parserUtil::Temp_New(0, tempVariableIndices);
        attd.type = "int";
        MAKE_QUAD("=", symbolStack.top(), "-", attd.place);
        SAVE_STACK(attd);
    };
    auto case48 = [&]() {
        attd.place = parserUtil::Temp_New(1, tempVariableIndices);
        attd.type = "double";
        MAKE_QUAD("=", std::to_string(stof(symbolStack.top())), "-",
                  attd.place);
        SAVE_STACK(attd);
    };
    auto case49 = [&]() {
        POP_STACK(1);
        attd.place = parserUtil::lookup(att1.name, symbolTable);
        attd.type = parserUtil::look_up_type(att1.name, symbolTable);
        attributeStack.push(attd);
    };
    _GEN_SYM_CASE(50, "+");
    _GEN_SYM_CASE(51, "-");
    _GEN_SYM_CASE(52, "*");
    _GEN_SYM_CASE(53, "/");
    _GEN_SYM_CASE(54, "==");
    _GEN_SYM_CASE(55, "!=");
    _GEN_SYM_CASE(56, "<");
    _GEN_SYM_CASE(57, "<=");
    _GEN_SYM_CASE(58, ">");
    _GEN_SYM_CASE(59, ">=");

    _GEN_IO_CASE(61, 2, "R");
    _GEN_IO_CASE(62, 1, "R");
    _GEN_IO_CASE(64, 1, "W");
    _GEN_IO_CASE(65, 2, "W");

    _GEN_EMPTY_CASE(60);
    _GEN_EMPTY_CASE(63);
    _GEN_EMPTY_CASE(66);
    _GEN_EMPTY_CASE(67);

    auto reduce = [&](int idx) {
        // Initialize attribute structures to store data during parsing
        clear_attributes();

#define MIN_CASE_VALUE 2
#define MAX_CASE_VALUE 65

        std::vector<std::function<void()>> switchFunc = {
            case0,  case1,  case2,  case3,  case4,  case5,  case6,  case7,
            case8,  case9,  case11, case11, case14, case14, case14, case15,
            case16, case17, case18, case19, case20, case21, case22, case23,
            case24, case25, case26, case27, case28, case29, case30, case31,
            case32, case33, case34, case35, case36, case37, case38, case39,
            case40, case41, case42, case43, case44, case45, case46, case47,
            case48, case49, case50, case51, case52, case53, case54, case55,
            case56, case57, case58, case59, case60, case61, case62, case63,
            case64, case65, case66, case67};

        // Execute the corresponding case if the index is valid
        if (idx >= MIN_CASE_VALUE && idx <= MAX_CASE_VALUE &&
            switchFunc[idx] != nullptr) {
            switchFunc[idx]();  // Call the corresponding case function
        }
    };
    int cur = 0;  // Initialize current index for processing temp elements

#ifdef DEBUG
    int dbg_cnt = 0;
#endif

    // Start parsing loop
    while (true) {
        auto [x, y] = temp[cur];  // Get current element from temp (x is the
                                  // symbol, y is its type)
        int nxtsymbol{};          // Initialize the next symbol identifier

        // Determine the symbol ID based on its type (IDENT, INT, DOUBLE, etc.)
        if (y == "IDENT") {
            nxtsymbol =
                symbolToIdMap["id"];  // Map IDENT to corresponding symbol ID
        } else if (y == "INT") {
            nxtsymbol = symbolToIdMap["UINT"];  // Map INT to UINT ID
        } else if (y == "DOUBLE") {
            nxtsymbol = symbolToIdMap["UFLOAT"];  // Map DOUBLE to UFLOAT ID
        } else {
            nxtsymbol =
                symbolToIdMap[x];  // Map any other symbol to its respective ID
        }

        // Retrieve the action to be taken (goto state) from the goto table
        // using the current state and symbol
        auto to_be_gone = gotoTable[stateStack.top()][nxtsymbol];

        // If the action is -1, it means we are at the end or error state
        if (to_be_gone == -1) {
            break;  // End parsing loop
        }

        // If no valid action exists for this symbol, report a syntax error
        if (!to_be_gone) {
            std::cout << "Syntax Error" << "\n";
            return {"$"};  // Return error code
        }

        // If the action is negative, it indicates a reduction (negative value
        // represents reduction rule index)
        if (to_be_gone < 0) {
            reduce(-to_be_gone);  // Call the reduce function to apply the
                                  // corresponding reduction rule

            // Retrieve the production rule (left-hand side and right-hand side)
            auto [e1, e2] = flattenedGrammarRules[-to_be_gone - 1];
            size_t accept_size =
                e2.size();  // Get the size of the RHS of the rule

            // If the RHS is empty (epsilon rule), set accept_size to 0
            if (e2[0] == "^") {
                accept_size = 0;
            }

            // Pop symbols and states from the stacks based on the reduction's
            // RHS size
            for (size_t i = 0; i < accept_size; i++) {
                stateStack.pop();   // Pop the state stack
                symbolStack.pop();  // Pop the symbol stack
            }

#ifdef DEBUG
            std::cout << "[DEBUG] reduce#" << ++dbg_cnt
                      << ": idx = " << -to_be_gone << ", x = " << x
                      << ", y = " << y << "\n";
#endif

            // Push the LHS symbol of the reduction rule to the symbol stack
            symbolStack.push(e1);
            // Push the new state corresponding to the LHS symbol to the state
            // stack
            stateStack.push(
                gotoTable[stateStack.top()][symbolToIdMap[symbolStack.top()]]);
        } else {
            // If the action is positive, it indicates a shift (shift to new
            // state)
            stateStack.push(to_be_gone);  // Push the new state onto the stack
            symbolStack.push(x);          // Push the symbol onto the stack
            cur++;  // Move to the next symbol in the input
        }
    }

    // After parsing is complete, generate the parser output (quadruples, symbol
    // table, etc.)
    return generateParserOutput(nxt, quadruples, symbolTable,
                                tempVariableIndices);
}