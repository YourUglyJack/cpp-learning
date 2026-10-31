#include <iostream>
#include <string>
#include "tracer.h"
#include "myutils.h"

//   构造：
//   1. 成员按声明顺序：a → b → c
//   2. 最后执行构造函数体

//   析构：
//   1. 先执行析构函数体
//   2. 成员按逆序销毁：c → b → a

// 折构函数： 对象销毁的时候，自动调用的函数，用来清理资源
class Instrument{
private:
    std::string symbol_ = "UNKNOWN"; // 默认值，默认构造函数会用到
    double tick_size_ = 0.01;
public:
    Instrument() = default;
    Instrument(std::string sym, double tick) : symbol_(std::move(sym)), tick_size_(tick) {}
    explicit Instrument(std::string sym) : Instrument(std::move(sym), 0.0) {}

    void print() const { std::cout << "Instrument: " << symbol_ << ", tick = " << tick_size_ << std::endl; }
};

class AssignInBody {
private:
    Tracer t_;
public:
    explicit AssignInBody(const std::string& n)
    {
        t_ = Tracer(n);
    }
};

class InitList{
private:
    Tracer t_;
public:
    explicit InitList(const std::string& n) : t_(n) {}
};

class RiskCheck {
private:
    // ref 必须立刻绑定，int& ref; ref = x; error 
    // const 必须立刻赋值 const int x; x = 10 error!!!!!
    // 所以必须用成员初始化列表，如果在 {} 里 赋值会出错
    // 因为 {} 前就声明了
    const Instrument& inst_; // 引用成员：注意被引用的对象必须比 RiskCheck 活得久
    const double max_pos_;
public:
    RiskCheck(const Instrument& inst, double max_pos) : inst_(inst), max_pos_(max_pos) {}
    double limit() const { return max_pos_; }
};

class Strategy {
public:
    // warning: initializer order does not match the declaration order 
    Strategy() : c_("c"), b_("b"), a_("a") {std::cout << "Strategy body runs last\n";}
    ~Strategy() { std::cout << "       ~Strategy body runs First, then member destroyed in reverse c b a\n";}
private:
    Tracer a_;
    Tracer b_;
    Tracer c_;
};

class Qty {
private:
    int v_;
public:
    explicit Qty(int v) : v_(v) {}
    int value() const { return v_; }
};

void send(Qty q) {
    std::cout << "   send qty " << q.value() << std::endl;
}

int main() {
    print_title("construction");

    Instrument i1; // 默认构造
    Instrument i2{"ES", 0.25}; // 带参构造
    Instrument i3{"AAPL"}; // 委托构造
    i1.print();
    i2.print();
    i3.print();

    print_title("init list vs assign in body");
    std::cout << "Assign in function body\n";
    {AssignInBody x{"x"};}
    std::cout << "Member initializer list\n";
    {InitList y{"y"};}

    print_title("const and ref members");
    RiskCheck rc{i2, 50};
    std::cout <<" limit = " << rc.limit() << "\n";

    print_title("construction / destruction order");
    { Strategy s; }

    print_title("explicit");
    Qty q{100};
    send(q);
    // send(200); // candidate function not viable: no known conversion from 'int' to 'Qty' for 1st argument
}
