#include <algorithm>
#include <iostream>
#include <set>
#include <vector>
#include "../common/myutils.h"



int main() {
    print_title("iterators and invalidation");
    std::vector<int> values = {1, 2, 3, 4, 5, 6};
    print_vector(values, "origin");

    for (auto it = values.begin(); it != values.end();) {
        if (*it % 2 == 0) {
            it = values.erase(it); // return 被删元素的下一个位置的iter
        } else {
            ++it;
        }
    }
    print_vector(values, "after del by iter");

    std::vector<int> large = {1, 2, 3, 4, 5, 6};
    // 把符合条件的元素 move 到前面，返回新的 end, 本身的size 并不会变化, 注意 是返回 end()!!!
    auto logical_end = std::remove_if(large.begin(), large.end(), [](int x){return x % 2 == 0;});
    print_vector(large, "larger");
    std::cout << "size after remove_if but before erase: " << large.size() << "\n";
    large.erase(logical_end, large.end());
    std::cout << "size after erase: " << large.size() << "\n";

    std::set<int> unique = {1, 2, 3, 4, 5, 6};
    for (auto it = unique.begin(); it != unique.end();) {
        if (*it % 2 == 0) {
            it = unique.erase(it); // c++17 返回下一个有效的 it
        }
        else {
            ++ it;
        }
    }
    print_set(unique, "set after erasing");
}

