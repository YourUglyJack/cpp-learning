#include <array>
#include <cstdint>
#include <stdexcept>
#include <vector>
#include <iostream>
#include "../common/myutils.h"


void print_state(const::std::vector<int>& v, const char* stage) {
    std::cout << stage << ", size=" << v.size() << " capacity=" << v.capacity() << "\n";
}

int main() {
    print_title("vector and array");
    std::vector<int> prices;
    print_state(prices, "initial");

    prices.reserve(3);
    print_state(prices, "after reverse");
    prices.push_back(10);
    prices.emplace_back(102); // int 简单类型，和 push_back 性能相当
    print_state(prices, "after inserting 2 elems");

    // uintptr_t 无符号整数类型, 在 指针和整数之间转换使用
    // int long 存 ptr 可能会因为 byte 不够 8 而丢失 info
    auto old_addr = reinterpret_cast<std::uintptr_t>(prices.data());
    std::cout << prices.data() << "\n";
    prices.reserve(100);
    auto new_addr = reinterpret_cast<std::uintptr_t>(prices.data());
    std::cout << "storage changed after reserve(100)\n";
    std::cout << "old=" << old_addr << ", new=" << new_addr << "\n";
    // 扩容后，指针会更换地址，在这之前取得的 ref, iterator 都会失效
    print_state(prices, "after reserve(100)");

    // resize 补满5个元素，with -1
    prices.resize(5, -1);
    print_state(prices, "after resize(5, -1)");
    for (int x: prices) std::cout << x << " "; 
    std::cout << std::endl;

    // at() will check boundary, but [] will not it will trigger UB
    try {
        std::cout << prices.at(100) << "\n";
    } catch (const std::out_of_range&) {
        std::cout << "at(100) throws out_of_range\n";
    }

    std::array<int, 3> fixed = {1, 2, 3};
    std::cout << "array size = " << fixed.size() << ", data[1] = " << fixed[1] << "\n";
}