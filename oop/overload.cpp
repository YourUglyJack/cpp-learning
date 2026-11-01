#include <compare>
#include <cstdint>
#include <iomanip>
#include <iostream>
#include <map>
#include <unordered_map>
#include <vector>
#include "../common/myutils.h"

class Money {
private:
    std::int64_t cents_{0};
public:
    // constexpr 在 compile time 就可以进行 compute，run time 时 没有开销
    constexpr Money() = default;
    constexpr explicit Money(std::int64_t cents) : cents_(cents) {}
    static constexpr Money from_units(double units) { 
        // 9.99 * 100 = 998.99999
        return Money(static_cast<std::int64_t>(units * 100 + (units >= 0 ? 0.5 : -0.5)));
    }

    // 小对象 传 value 更快
    Money& operator+=(Money o) { cents_ += o.cents_; return *this; }
    Money& operator-=(Money o) { cents_ -= o.cents_; return *this; }
    Money& operator*=(std::int64_t k) { cents_ *= k; return *this; }

    Money operator-() const { return Money(-cents_); }

    // ++a;   // 前置：先加，返回加后的值
    // a++;   // 后置：先返回旧值，再加
    Money& operator++() {
        ++cents_;
        return *this;
    }

    Money operator++(int) { // 用 int 区分 后置++
        // a++ 更慢 因为 要存储一份 old 
        Money old = *this;
        ++cents_;
        return old;
    }

    // if (m), while (m)
    // explicit 防止 隐式转换
    explicit operator bool() const { return cents_ != 0; }

    std::int64_t cents() const { return cents_; }

    // 输出：友元非成员函数（左操作数是 ostream）
    friend std::ostream& operator<<(std::ostream& os, Money m)
    {
        std::int64_t c = m.cents_ < 0 ? -m.cents_ : m.cents_;
        return os << (m.cents_ < 0 ? "-" : "") << "$" << c / 100 << '.' << std::setw(2) << std::setfill('0') << c % 100 << std::setfill(' ');
    }
};

//   普通调用：
//   main() → 跳转 → add() → 跳回 → main()
//            压栈         出栈

//   内联展开： 可以 省去 函数调用的开销 eg 压站 跳转 出战
//   main() 里直接放 add 的代码，不跳转

//   int x = add(1, 2);
//   变成
//   int x = 1 + 2;


// 二元运算符：非成员，基于复合赋值实现（参数按值接收，正好拿来当结果）
// 该自己 返回 ref，算 new value 返回 obj
inline Money operator+(Money a, Money b) { return a += b; }
inline Money operator-(Money a, Money b) { return a -= b; }
inline Money operator*(Money a, std::int64_t k) { return a *= k; } // m*3
inline Money operator*(std::int64_t k, Money a) { return a *= k; } // 3*m

// 让 Money 能作为 unordered_map 的 key：特化 std::hash
template<> 
struct std::hash<Money>
{
    // std::hash<Type>{}() return hash code
    std::size_t operator()(Money m) const noexcept {
        // std::hash<std::int64_t>{} 匿名对象
        // std::hash<std::int64_t> hasher;
        // hasher(42);
        return std::hash<std::int64_t>{}(m.cents());
    }
};


class TimeSeries {
public:
    // size_t 是无符号整数
    // ptrdiff+t 有符号版本的 size_t
    void push(double v) { data_.push_back(v); }
    double& operator[](std::ptrdiff_t i) { 
        std::size_t idx = (i >= 0) ? i : data_.size() + i;
        return data_[idx];
    }
    const double& operator[](std::ptrdiff_t i) const { 
        std::size_t idx = (i >= 0) ? i : data_.size() + i;
        return data_[idx];
    }
    std::size_t size() const { return data_.size(); }
private:
    std::vector<double> data_;
};

// exponentially weighted moving average
// alpha large, more senstive
//.      small, more smooth,
// EMA with small alpha has more lag, slow response
class Ewma {
public:
    explicit Ewma(double alpha) : a_(alpha) {}
    double operator()(double x) {
        if (init_) {
            s_ = a_ * x + (1 - a_) * s_;
        } else {
            init_ = true;
        }
        return s_;
    }
private:
    double a_, s_ = 0;
    bool init_ = false;
};


int main() {
    print_title("arithmetic");
    Money price = Money::from_units(189.52);
    Money fee = Money(150);
    Money total = price * 100 + fee;
    std::cout << "price: " << price << " fee: " << fee << " total(100 shares): " << total << "\n";
    std::cout << "3*fee: " << 3 * fee << " fee * 3: " << fee * 3 << " -fee: " << -fee << "\n";
    total -= Money::from_units(1000);
    std::cout << "after -= $1000: " << total << "\n";

    print_title("++prefix and postfix");
    Money m{99};
    Money old = m++; // return old, and add
    std::cout << "m++ returned: " << old << "  cur m: " << m << "\n";

    print_title("bool operator");
    if (Money{10}) {
        std::cout << "you have money!\n";
    } else {
        std::cout << "you dont have money, go to work!\n";
    }
    // int bad = Money(10); // no viable conversion from 'Money' to 'int'

    print_title("operator[]");
    TimeSeries ts;
    for (double x: {100.0, 200.0, 300.0}) ts.push(x);
    ts[0] = 90.0;
    std::cout << "ts[0]: " << ts[0] << " ts[-1]: " << ts[-1] << "\n";
    Ewma ewma(0.5);
    for (double x : {100.0, 100.0, 100.0}) {
        std:: cout << ewma(x) << std::endl;
    }
}
