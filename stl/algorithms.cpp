#include <algorithm>
#include <iostream>
#include <numeric>
#include <string>
#include <vector>
#include "../common/myutils.h"

struct Quote
{
    std::string symbol;
    int price;
};

std::ostream& operator<<(std::ostream& os, const Quote& q) {
    return os << q.symbol << ": " << q.price;
}

int main() {
    print_title("generic algorithms\n");
    std::vector<int> prices = {105, 101, 109, 103, 103};
    std::sort(prices.begin(), prices.end()); // ascending
    print_vector(prices, "sorted");

    // 返回 第一个 >=103 的 迭代器, 对于 vector 来说，迭代器就是地址
    // 当 两个 ptr diff 的时候， 编译器会 自动除以 elem 大小，这样就得到了 index
    auto lower = std::lower_bound(prices.begin(), prices.end(), 103);
    std::cout << "first >= 103, index=" << std::distance(prices.begin(), lower) << "\n";

    auto high = std::find_if(prices.begin(), prices.end(), [](int x){ return x > 106; });
    if (high != prices.end()) std::cout << "first >= 106 " << *high << "\n";

    long long total = std::accumulate(prices.begin(), prices.end(), 0LL);
    std::cout << "sum = " << total << "\n";

    std::vector<int> doubled(prices.size());
    std::transform( // 直接 写入 doubled 指向的位置，不会自动扩容
        prices.begin(), prices.end(),
        doubled.begin(),
        [](int x) { return x*2; }
    );

    std::vector<Quote> quotes = {{"first", 100}, {"second", 100}, {"third", 90}};
    std::stable_sort(quotes.begin(), quotes.end(), [](const Quote& a, const Quote& b){ return a.price < b.price; });
    print_vector(quotes, "stable_sort");
}