#include <algorithm>
#include <iostream>
#include <string>
#include <tuple>
#include <vector>
#include "../common/myutils.h"

struct Order
{
    int price_cents;
    unsigned long long seq;
    std::string id;
};

std::ostream& operator<<(std::ostream& os, const Order& o) {
    return os << o.id << " @ " << o.price_cents << " seq=" << o.seq;
}

int main() {
    print_title("comparator and lambda");
    
    std::vector<Order> bids = {
        {10000, 2, "later"},
        {10010, 3, "best"},
        {10000, 1, "earlier"}
    };

    // comp(a, b) true: return a, false return b, a 应该 在 b 前面吗？
    std::sort(bids.begin(), bids.end(), [](const Order& a, Order& b){
        if (a.price_cents != b.price_cents) return a.price_cents > b.price_cents;
        return a.seq < b.seq;
    });
    print_vector(bids, "bids");

    int threshold = 100;
    auto by_value = [threshold](int x) { return x > threshold; }; // threshold 被 copy 进 []，快照，定格瞬间
    auto by_ref = [&threshold](int x) { return x > threshold; }; // ref capture 实时值
    threshold = 200;
    std::cout << std::boolalpha << "value capture(150) = " << by_value(150) << "\n"; //t
    std::cout << std::boolalpha << "ref capture(150) = " << by_ref(150) << "\n"; // f
}
