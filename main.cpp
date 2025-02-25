#include <algorithm>
#include <iostream>
#include <fstream>
#include "lexer.h"
#include "parser.h"
#include "objectGen.h"

#define TASK 3

void main_lexer() {
    std::string s, str;

#ifdef ONLINE_JUDGE
    std::ifstream input("1in");
    if (input.is_open()) {
        while (getline(input, s)) {
            str += s + '\n';
        }
        input.close();
    } else {
        std::cout << "Failed to open input file." << std::endl;
        return;
    }
#else
    while (getline(std::cin, s)) {
        str += s + '\n';
    }
#endif

#if (TASK == 3)
    ObjectCodeGenerator ocg;
    auto ans = ocg.generate(str);
    std::cout << ans;
    return;
#else
    Lexer lexer;
    auto ans = lexer.lexer(str);

    if (ans.empty())
        return;

    if (lexer.err_msg.size()) {
        std::sort(ans.begin(), ans.end());
        std::cout << lexer.err_msg[0].second;
        return;
    }

#if (TASK == 1)
    for (const auto& s : ans) {
        std::cout << s;
    }
#elif (TASK == 2)
#ifdef DEBUG
    std::cout << "[DEBUG] ===== lexer out =====\n";
    int dbg_cnt = 0;
    for (const auto& s : ans) {
        std::cout << "[DEBUG] l#" << ++dbg_cnt << ": " << s;
    }
    std::cout << "[DEBUG] ===== lexer end =====\n\n";
#endif
    Parser parser;
    ans = parser.parser(ans);
    for (const auto& s : ans) {
        if (s[0] == '$') exit(0);
        std::cout << s;
    }
#endif
#endif
}

int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(0);
    main_lexer();
    return 0;
}
