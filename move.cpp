#include <iostream>
#include <memory>
#include <string>
#include <utility>
#include <vector>
#include "tracer.h"

void print_title(std::string title) {
    std::cout << "============= " << title << " ======================\n";
}

class Series
{
private:
    std::size_t n_;
    double* data_;
public:
    explicit Series(std::size_t n)
        : n_(n), data_(new double[n]())
        {
            std::cout << "      Series " << n << " allocated\n";
        }

    // destructor
    ~Series() { delete[] data_; }
    // copy constructor
    Series(const Series& o)
        : n_(o.n_), data_(new double[o.n_])
        {
            std::copy(o.data_, o.data_ + n_, data_);
            std::cout << "      Series COPY allocate and copy " << n_ << "\n";
        }
    // copy assignment, copy-and-swap
    Series& operator=(Series o) {
        swap(*this, o);
        return *this;
    }
    // && move constructor, steal ptr, right value ref
    Series(Series&& o) noexcept : n_(o.n_), data_(o.data_)
    {
        o.n_ = 0;
        // move 后 ptr 必须 null，否则 会 release 同一片 空间多次，可能，崩溃
        o.data_ = nullptr;
        std::cout << "      Series MOVE, steal ptr, no allocation\n";
    }

    friend void swap(Series& a, Series& b) noexcept {
        std::swap(a.data_, b.data_);
        std::swap(a.n_, b.n_);
    }
    bool empty() const { return data_ == nullptr; }
    std::size_t size() const { return n_; }
};

Series load_price() {
    return Series(1000);
}

class Portfolio {
private:
    std::vector<std::string> names_;
public:
    // 按值接受， 不能steal const 的 ptr，因为 const 不能修改--
    //   调用者传左值 -> 1 次拷贝 + 1 次移动；传右值 -> 2 次移动，0 次拷贝
    void add(std::string name) { 
        // move 进容器
        names_.push_back(std::move(name)); 
    }
    std::size_t size() const { return names_.size(); }
};

int main() {
    print_title("copy and move construction");
    Tracer a{"a"};
    std::cout << "      a.name() " << a.name() << "\n";
    Tracer b{a};
    Tracer c = std::move(a);
    std::cout << "      After moving, a.name() " << a.name() << "\n";

    print_title("copy and move assigment");
    Tracer e{"e"};
    e = b; // copy assign， b 是 left value，拷贝数据
    e = Tracer("freash"); // right value 马上就要die了，不如直接偷指针，别copy 了
    e = std::move(c);

    print_title("a resource class: copy allocates, move steals");
    Series s1{1000000};
    Series s2{s1};
    Series s3 = std::move(s1);
    std::cout << "      s1 is empty ? " << s1.empty() << "\n";
    Series s4 = load_price();
    s4 = std::move(s2);

    print_title("containers: push_back(copy), push_back(move) and empush_back()");
    Tracer::reset();
    Tracer::verbose = false;
    std::vector<Tracer> v;
    v.reserve(3);
    Tracer x{"x"};
    v.push_back(x); // copy
    v.push_back(std::move(x));
    v.emplace_back("build-in-place");
    Tracer::print_stats("3 inserts");
    Tracer::verbose = true;

    print_title("move-only types: unique_ptr");
    auto p = std::make_unique<int>(42);
    // auto q = p; compile error
    auto q = std::move(p);
    std::cout << "      p is " << (p ? "non-null" : "null") << ", *q = " << *q << "\n";

    print_title("sink param");
    Portfolio pf;
    std::string name = "monmentum-1";
    pf.add(name);
    pf.add(std::move(name));
    pf.add("mean-rev-2");
    std::cout << "    portfolio size = " << pf.size() << '\n';

    print_title("end");
}
