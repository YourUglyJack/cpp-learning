#include <iostream>
#include <memory>
#include <string>
#include <utility>
#include <vector>
#include "../common/myutils.h"

// std::move 类型转换, 本身不移动任何东西
// 把 left value -> right value
// left val: 有名字 可以取地址
// right val: 临时的 没有名字的

// std::move(x)  →  变成右值  →  匹配 && 版本  →  偷内容

struct Instrument {
    std::string symbol;
    explicit Instrument(std::string s) : symbol(std::move(s)) {
        std::cout << "[construct] " << symbol << "\n";
    }
    Instrument(const Instrument& other) : symbol(other.symbol) {
        // symbol(other.symbol): 把 other.symbol 的内容 copy 一份给 this -> symbol
        std::cout << "[copy] " << symbol << "\n";
    }
    Instrument(Instrument&& other) noexcept : symbol(std::move(other.symbol)) {
        std::cout << "[move] " << symbol << "\n";
    }
    Instrument& operator=(const Instrument&) = default; // 拷贝赋值
    Instrument& operator=(Instrument&&) noexcept = default; // 移动赋值
    ~Instrument() { std::cout << "[destruct] " << symbol << "\n"; }
};

int main() {
    print_title("move semantics and ownership");
    std::vector<Instrument> portofolio;
    portofolio.reserve(2);
    portofolio.emplace_back("AAPL");
    portofolio.emplace_back("MSFT");
    std::cout << "before reallocation size = " << portofolio.size() << ", capacity = " << portofolio.capacity() << "\n";

    portofolio.reserve(100);
    std::cout << "after reallocation size = " << portofolio.size() << ", capacity = " << portofolio.capacity() << "\n";

    // 相当于：
    // 1. new Instrument("NVDA")   创建对象
    // 2. 把指针包进 unique_ptr    自动管理
    // unique_ptr 智能指针，独占所有权，自动释放内存
    // unique_ptr = 裸指针 + 析构时自动 delete; 离开zuo yong yu de
    auto a = std::make_unique<Instrument>("NVDA");
    std::vector<std::unique_ptr<Instrument>> pointers;
    pointers.push_back(std::move(a));
    std::cout << "a is nullptr = " << std::boolalpha << (a == nullptr) << "\n";
    std::cout << "owned sym = " << pointers.front()->symbol << "\n";
}



//   class Instrument {
//       // 构造函数们（没有返回值，没有 operator=）
//       Instrument();                              // 默认构造
//       Instrument(const Instrument&);             // 拷贝构造
//       Instrument(Instrument&&);                  // 移动构造

//       // 赋值运算符们（有返回值，有 operator=）
//       Instrument& operator=(const Instrument&);  // 拷贝赋值
//       Instrument& operator=(Instrument&&);       // 移动赋值
//   };