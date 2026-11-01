#include <atomic>
#include <cstdint>
#include <iostream>
#include <string>
#include <vector>
#include "../common/myutils.h"

// 静态函数： 不需要 实例化 类 就可以使用
// 静态工厂： 用静态函数创建对象
class Order {
public:
    enum class Side {Buy, Sell}; // nested, Order::Side::buy

    static Order buy(std::string sym, double px, int qty) {
        return Order(std::move(sym), Side::Buy, px, qty);
    }
    static Order sell(std::string sym, double px, int qty) {
        return Order(std::move(sym), Side::Sell, px, qty);
    }

    Order(const Order& o) 
        : id_(next_id_++), sym_(o.sym_), side_(o.side_), px_(o.px_), qty_(o.qty_)
        {
            ++alive_;
        }

    ~Order() { -- alive_; }

    Order& operator=(const Order& o) = delete;

    std::uint64_t id() const { return id_; }
    // static 没有 this，const 保护 this，两个放一起没意义
    static int alive() { return alive_; }
    
    friend std::ostream& operator<<(std::ostream& os, const Order& o) {
        // << 不是 成员函数，没有 权限访问 private
        return os << "#" << o.id_ << " " << (o.side_ == Side::Buy ? "Buy " : "Sell ") << o.qty_ << " " << o.sym_ << " @ " << o.px_ << "\n";
    }

    friend class MatchingEngine;

private:
    // 私有构造
    Order(std::string sym, Side side, double px, int qty)
        : id_(next_id_++), sym_(std::move(sym)), side_(side), px_(px), qty_(qty)
        {
            ++alive_;
        }

    std::uint64_t id_;
    std::string sym_;
    Side side_;
    double px_;
    int qty_;

    static inline std::uint64_t next_id_ = 1;
    static inline int alive_ = 0;
};

class MatchingEngine {
public:
    static void fill(Order& o, int q) { o.qty_ -= q; }
    static int remaining(const Order& o) { return o.qty_; }
};

class Config {
public:
    static Config& instance() {
        static Config cfg;
        return cfg;
    }
    double max_notional = 1e6;
private:
    Config() {
        std::cout << "      Config constructed once\n";
    }
};

int main() {
    print_title("static member and factory ");
    {
        Order a = Order::buy("AAPL", 189.5, 100);
        Order b = Order::sell("MSFT", 410.1, 100);
        std::cout << "      " << a << "\n";
        std::cout << "      " << b << "\n";
        std::cout << "      " << "alive: " << Order::alive() << "\n";

        print_title("friend class");
        MatchingEngine::fill(a, 30);
        std::cout << "      alive order: " << Order::alive() << "\n";
        std::cout << "      after fill, remaining of a: " << MatchingEngine::remaining(a) << "\n";
    }
    std::cout << "      quit {}, alive order: " << Order::alive() << "\n";

    print_title("singleton via static local");
    Config::instance().max_notional=2e6;
    std::cout << "      max_notional: " << Config::instance().max_notional << "\n";
}