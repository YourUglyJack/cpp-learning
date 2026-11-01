#include <cmath>
#include <iostream>
#include <memory>
#include <numbers>
#include <type_traits>
#include <string>
#include <vector>
#include "../common/myutils.h"

// C++ 标准说：通过基类指针 delete 派生类对象，如果基类析构函数不是 virtual，行为未定义。
// 编译器可以做任何事，甚至"看起来正常"也不代表代码是对的。
// 最常发生的是 derive leak，还可能发生别的，但具体不知道，所以说是未定义行为

class Instrument {
public:
    explicit Instrument(std::string sym) : sym_(std::move(sym)) {}
    virtual ~Instrument() { std::cout << "~Instrument " << sym_ << "\n"; }

    virtual double price(double spot) const { return spot; }
    virtual std::string kind() const { return "Instrument"; }
    const std::string &symbol() const { return sym_; }

private:
    std::string sym_;
};


class Enquity : public Instrument
{
public:
    using Instrument::Instrument; // 继承基类所有构造函数, 当 derived class 没有 new menmber的时候，用这个方便
    std::string kind() const override { return "Enquity"; }
    ~Enquity() override { std::cout << "~Enquity\n"; }
};

class Future : public Instrument
{
private:
    double r_, t_;

public:
    Future(std::string sym, double r, double t)
        : Instrument(sym), r_(r), t_(t)
    {
    }
    ~Future() override {std::cout <<"~Future\n"; }

    std::string kind() const override { return "Future"; }
    double price(double spot) const override { return spot * std::exp(r_ * t_); }
};


class CallOption final : public Instrument {
public:
    CallOption(std::string sym, double k, double r, double vol, double t) 
        : Instrument(std::move(sym)), k_(k), r_(r), vol_(vol), t_(t), greeks_(new double[3]())
        {}
    ~CallOption() override {
        delete[] greeks_;
        std::cout << "~CallOption, free greeks buffer \n";
    }

    double price(double s) const override {
        // 标准正态分布的累积分布函数 N(x)
        // std::erfc() 定义在 <cmath> 中，C++17 支持
        // Lambda 赋值给 N，注意结束必须写 };
        auto N = [](double x) {
            return 0.5 * std::erfc(-x / std::sqrt(2.0));
        };

        // d1 = [ln(S/K) + (r + σ²/2)T] / (σ√T)
        double d1 = (std::log(s / k_) + (r_ + 0.5 * vol_ * vol_) * t_) / (vol_ * std::sqrt(t_));

        // d2 = d1 - σ√T
        double d2 = d1 - vol_ * std::sqrt(t_);

        // European Call Price = S*N(d1) - K*e^(-rT)*N(d2)
        return s * N(d1) - k_ * std::exp(-r_ * t_) * N(d2);
    }

    std::string kind() const override { return "CallOption"; }

private:
    double k_;
    double r_;
    double vol_;
    double t_;
    double* greeks_;
};

void show_by_ref(const Instrument& i, double spot) {
    std::cout << "[by ref] " << i.kind() << " " << i.symbol() << " = " << i.price(spot) << "\n";
}

// 按值接收：发生"对象切片 (Object Slicing)"，多态失效！
// 参数 i 是拷贝出来的 Instrument 对象，派生类部分被切掉了
// 只剩下基类的 sym_，调用的是 Instrument::kind() 和 Instrument::price()
void show_by_value(Instrument i, double spot) {
    std::cout << "[by value] " << i.kind() << " " << i.symbol() << " = " << i.price(spot) << "\n";
}

// ---------------------------------------------------------------------------
// 【反面教材】没有虚析构的基类
// 用于演示 delete base_ptr 的未定义行为
// dtor: destructor, ctor: constructor
// ---------------------------------------------------------------------------
struct NoVirtualDtorBase {
    ~NoVirtualDtorBase() { std::cout << "    ~NoVirtualDtorBase\n"; }  // 没有 virtual！
};
struct LeakyDerived : NoVirtualDtorBase {
    std::vector<double> big = std::vector<double>(1000);  // 1000 个 double，8KB
    ~LeakyDerived() { std::cout << "    ~LeakyDerived (you will NOT see this line)\n"; }
};

int main() {
    // LeakyDerived derived; // obj 在 stack 上，不能把 obj 给 指针
    // NoVirtualDtorBase* base = derived; // &derived 才行
    
    // undeifined bahavior
    //  ~NoVirtualDtorBase
    // NoVirtualDtorBase* base = new LeakyDerived;
    // delete base;
    print_title("portfolio through base ptr");
    std::vector<std::unique_ptr<Instrument>> book;
    book.push_back(std::make_unique<Enquity>("AAPL"));
    book.push_back(std::make_unique<Future>("ESZ6", 0.04, 0.25));
    book.push_back(std::make_unique<CallOption>("AAPL C200", 200, 0.04, 0.25, 0.5));

    // Poly, runtime 根据 inst 类型 选择 运行哪一个 price
    for (const auto& inst : book) {
        std::cout << inst->kind() << ", " << inst->symbol() << ", prince(190) = " << inst->price(190) << "\n";
    }

    print_title("ref keep poly, value slice it");
    Future f{"ESZ6", 0.04, 0.25};
    show_by_ref(f, 5000);
    show_by_value(f, 5000); // derived 的 东西都被截断了

    print_title("virtual dtor");
    {
        Instrument* base = new CallOption("X", 100, 0.01, 0.2, 1);
        delete base;
    }
    
    print_title("end");

}