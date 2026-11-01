#include <cstring>
#include <iostream>
#include <string>
#include <typeinfo>
#include "../common/myutils.h"

struct Plain { 
    double x;
    void f() {}
};

struct  WithVirtual
{
    double x;
    virtual void f() {}
    virtual ~WithVirtual() = default;
};

struct TwoVirtuals
{
    double x;
    virtual void f() {}
    virtual void g() {}
    virtual ~TwoVirtuals() = default;
};

class Strategy {
public:
    Strategy() {
        // 构造的时候 call 了 virtual
        std::cout << "Strategy ctor calls name() " << name() << "\n";
    }
    virtual ~Strategy() { std::cout << "Strategy dtor calls name() " << name() << "\n"; }
    virtual std::string name() const { return "Strategy base"; }
    void start() { std::cout << "start() calls name() " << name() << "\n"; }
};

class Momentum : public Strategy {
public:
    Momentum() : lookback_(20) { std::cout << "Momentum ctor done\n"; }
    std::string name() const override { return "Momentum Strategy\n"; }
private:
    int lookback_;
};

int main() {
    // 虚表指针
    // 有虚函数的类，每个对象多了 8 字节 vptr（虚表指针），指向 vtable（虚函数表）
    // vtable 每个 class 只有一个，其他 obj 的 vptr 都指向 这一个
    print_title("hidden vptr");
    std::cout << "sizeof Plain        = " << sizeof(Plain) << "\n";
    std::cout << "sizeof WithVitual   = " << sizeof(WithVirtual) << "\n";
    std::cout << "sizeof TwoVirtuals  = " << sizeof(TwoVirtuals) << "\n";

    WithVirtual a, b;
    auto vptr_of = [](const void* obj) { 
        // void* 通用指针
        const void* v;
        // 从 obj 开头 拷贝 8 bytes 到 v, vptr 是 hidden 的，这是强行读取
        std::memcpy(&v, obj, sizeof(v));
        return v;
    };
    std::cout << "vptr(a) = " << vptr_of(&a) << ", vptr(b) = " << vptr_of(&b) << "\n";
    std::cout << "same vptr for the same class\n";

    TwoVirtuals c;
    std::cout << "vptr(c) = " << vptr_of(&c) << "\n";
    std::cout << "A different class has different vtable\n";

    print_title("virtual calls inside construction");
    {
        // 构造函数里 call vitural func
        // 刚开始 只有 base clz，所以 调用 base clz 的 virtual
        // 当 构造到 derived的时候，vptr 指向 derived class 的 vtable，此时就可以 call derived clz 的 virtual function了
        Momentum m;
        m.start();
    }
}

