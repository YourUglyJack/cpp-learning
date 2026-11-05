#include <iostream>
#include <map>
#include <set>
#include <stdexcept>
#include <string>
#include "../common/myutils.h"

// map, AVL Tree, order with key
int main() {
    print_title("map and set");
    std::map<int, int> bids; 
    bids.emplace(10200, 5);
    bids.emplace(10100, 5);
    bids.emplace(10300, 1);
    for (const auto& [price, qty] : bids) {
        std::cout << price << ":" << qty << " ";
    }
    std::cout << "\n";

    std::cout <<"before [] size = " << bids.size() << "\n";
    int missing_qty = bids[9999]; // 不存在，插入 {9999, 0}
    std::cout << "bids[9999] = " << bids[9999] << ", after [] size = " << bids.size() << "\n";
    
    // read-only recommond at() or find(), 不会 create key-value accidently
    if (auto it = bids.find(10100); it != bids.end()) {
        std::cout << "found 10100, qty = " << it->second << "\n";
    }

    try {
        std::cout << bids.at(777) << "\n";
    } catch (const std::out_of_range&) {
        std::cout << "bids.at(777) throws, no insertion\n";
    }

    auto it = bids.lower_bound(10100); // find fist key >= 10150
    if (it != bids.end()) {
        std::cout << "lower_bound(10100) = " << it->first << "\n";
    }
    auto it_upper = bids.upper_bound(10100); // find fist key > 10150
    if (it_upper != bids.end()) {
        std::cout << "upper_bound(10100) = " << it_upper->first << "\n";
    }

    std::map<int, int, std::greater<int>> descending_bids; // 降序
    descending_bids[10100] = 3;
    descending_bids[10300] = 1;
    std::cout << "best bid=" << descending_bids.begin()->first << "\n";

    // begin() 返回的是一个 迭代器，迭代器 不是 elem 本身，需要 解引用*
    std::set<std::string> symbols = {"MSFT", "AAPL", "AAPL"};
    std::cout << "set size=" << symbols.size() << ", first=" << *symbols.begin() << "\n";
}