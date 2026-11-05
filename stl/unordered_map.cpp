#include <functional>
#include <iostream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include "../common/myutils.h"


// struct & class
// 默认访问权限不同 public private
// 默认继承方式不同 public private
// 除此之外 都一样
// struct 更适合 数据聚合 就是 都是数据操作等，不需要对外界隐藏 属性啥的 不需要有复杂的生命周期

struct Key {
    std::string symbol;
    int venue;
    bool operator==(const Key& other) const noexcept {
        return symbol == other.symbol && venue == other.venue;
    }
    // 左操作数 ostream&
    friend std::ostream& operator<<(std::ostream& os, const Key& k) {
        return os << k.symbol << ": " << k.venue;
    }
};

struct KeyHash {
    std::size_t operator() (const Key& k) const noexcept {
        // std::hash<std::string> hasher;
        // const auto h1 = hasher(k.symbol);
        const auto h1 = std::hash<std::string>{}(k.symbol); // {} 创建临时对象, (k.symbol) 调用 () operator 计算 hash
        const auto h2 = std::hash<int>{}(k.venue);
        // 随便组合一下==
        return h1 ^ (h2 + 0x9e3779b9U + (h1 << 6U) + (h1 >> 2U));
    }
};

int main() {
    print_title("unordered map and set");
    std::unordered_map<std::string, int> quantities;
    quantities.max_load_factor(0.75f); // 负载因子
    // 桶数量通常选质数
    // 标准库内部有一个 预定义 的 质数序列，会从里面挑选 >= needed_num
    quantities.reserve(100); // -> 137 not 100
    std::cout << "buckets after reserve=" << quantities.bucket_count() << "\n";

    quantities.try_emplace("AAPL", 100);
    quantities.try_emplace("AAPL", 99);
    quantities["MSFT"] += 20; // 原先没有的时候，会create 这个key，默认值为0
    for (const auto& [key, value] : quantities) {
        std::cout << key << ": " << value << " ";
    }
    std::cout << "\n";
    // #elem / #bucket
    std::cout << "load_factor=" << quantities.load_factor() << "\n";

    if (auto it = quantities.find("NVDA"); it == quantities.end()) {
        std::cout << "NVDA absent; no insertion\n";
    }

//   template<
//       class Key,                              // 键类型
//       class Value,                            // 值类型
//       class Hash = std::hash<Key>,            // 哈希函数（第三个）
//       class KeyEqual = std::equal_to<Key>,    // 相等比较器（第四个）
//       class Allocator = ...                   // 分配器（第五个）
//   > class unordered_map;
    // 第三个参数，告诉 unordered_map 怎么算 Key 的 hash
    std::unordered_map<Key, double, KeyHash> last_price;
    last_price.emplace(Key{"AAPL", 1}, 190.25);
    last_price.emplace(Key{"AAPL", 2}, 190.30);
    for (const auto& [key, value] : last_price) {
         // std::cout << key 不知道怎么输出 Key 类型
        std::cout << key << " " << value << ", ";
    }
    std::cout << "\n";

    std::unordered_set<std::string> seen = {"AAPL", "AAPL", "MSFT"};
    std::cout << "unique symbols=" << seen.size() << "\n";
}