#include <algorithm>
#include <iostream>
#include <fstream>
#include "lexer.h"

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

    Lexer lexer;
    auto ans = lexer.lexer(str);
    
    if (ans.empty())
        return;

    if (lexer.err_msg.size()) {
        std::sort(ans.begin(), ans.end());
        std::cout << lexer.err_msg[0].second;
        return;
    }

    for (const auto& s : ans) {
        std::cout << s;
    }
}

int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(0);
    main_lexer();
    return 0;
}
