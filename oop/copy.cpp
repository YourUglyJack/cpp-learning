#include <algorithm>
#include <cstring>
#include <iostream>
#include <utility>
#include <vector>
#include "tracer.h"
#include "../common/myutils.h"

// ⏺ 哈哈，因为 C++ 要让你完全控制每个细节：

//   对象怎么创建？     → 构造函数
//   对象怎么复制？     → 拷贝构造
//   对象怎么移动？     → 移动构造
//   对象怎么赋值？     → 赋值运算符
//   对象怎么销毁？     → 析构函数

// shalow copy
// 没写 拷贝构造函数，默认逐成员拷贝
// 成员是指针，拷贝地址，浅拷贝，share memory
class ShallowBuffer {
public:
    // new T[n]() = 分配 n 个 T，并初始化为 0
    explicit ShallowBuffer(std::size_t n) : n_(n), data_(new double[n]()) {}
    double* data() const { return data_; }
    void set(std::size_t i, double v) { data_[i] = v; }
private:
    std::size_t n_;
    double* data_;
};

// deep copy: 折构 拷贝构造 拷贝赋值
class DeepBuffer {
public:
    explicit DeepBuffer(std::size_t n)
        : n_(n), data_(new double[n]())
        {}
    
    // 折构
    ~DeepBuffer() {delete[] data_; }

    // 拷贝构造
    DeepBuffer(const DeepBuffer& o)
        : n_(o.n_), data_(new double[o.n_])
        {
            std::copy(o.data_, o.data_ + n_, data_);
            std::cout << "    DeepBuffer copy ctor: new memory allocated\n";
        }
    
    // 赋值拷贝
    DeepBuffer& operator=(DeepBuffer o) { // pass value, copy
        swap(*this, o);
        std::cout << "      DeepBuffer copy assign via copy-and-swap\n";
        return *this;
    }

    // noexcept 保证不抛异常，更快
    friend void swap(DeepBuffer& a, DeepBuffer& b) noexcept {
        // swap 是 normal function 不是member func
        // 但是想要 访问 private member
        // friend: 让外部函数访问 私有变量
        std::swap(a.n_, b.n_);
        std::swap(a.data_, b.data_);
    }
    double* data() const { return data_; }
    void set(std::size_t i, double v) {data_[i] = v;}

    private:
    std::size_t n_;
    double* data_;
};

class ZeroBuffer {
public:
    explicit ZeroBuffer(std::size_t n) : data_(n) {}
    void set(std::size_t i, double v) { data_[i] = v; }
    const double* data() const { return data_.data(); }
private:
    std::vector<double> data_;
};

class Connection {
public:
    Connection() = default;
    Connection(const Connection&) = delete; // 拷贝构造 禁止
    Connection& operator=(const Connection&) = delete; // 拷贝赋值 禁止
};

int main() {
    print_title("copy vs copy assigment");
    Tracer a("a");
    Tracer b = a;  // 拷贝构造，b 是新对象
    Tracer c("c");
    std::cout << "      Tracer c " << c.name() << "\n";
    c = a; // 拷贝赋值
    std::cout << "      Tracer b " << b.name() << "\n";
    std::cout << "      Tracer c " << c.name() << "\n";
    std::vector<Tracer> v;
    v.reserve(2);
    v.push_back(a); // 拷贝构造
    Tracer::print_stats("after section 1");

    print_title("shallow copy share memory");
    ShallowBuffer s1(3);
    ShallowBuffer s2 = s1; // copy ptr
    s2.set(0, 42.0);
    std::cout << "      s1.data() " << s1.data() << " s2.data() " << s2.data() << "\n";
    std::cout << s1.data()[0] << "\n";

    print_title("deep copy");
    DeepBuffer d1(3);
    DeepBuffer d2 = d1; // 拷贝构造
    d2.set(0, 42.0);
    std::cout << "  d1[0] " << d1.data()[0] << " d2[0] " << d2.data()[0] << "\n";
    std::cout << d1.data() << " | " << d2.data();

    DeepBuffer d3(5);
    d3 = d1; // 拷贝赋值
    d3 = d3; 

    print_title("zero buff");
    ZeroBuffer z1(3);
    ZeroBuffer z2 = z1; // 拷贝构造， 没写拷贝构造，默认成员拷贝，vector内部自己实现了沈拷贝
    z2.set(0, 42.0);
    std::cout << "    z1[0] = " << z1.data()[0] << ", z2[0] = " << z2.data()[0] << "  <- 自动深拷贝\n";
}