// =============================================================================
// common/tracer.h —— “会说话”的对象：每次被构造、拷贝、移动、赋值、析构都打印一行
// -----------------------------------------------------------------------------
// 学 C++ OOP 最有效的办法，就是亲眼看到“哪一行代码触发了哪个特殊成员函数”。
// 很多教程只告诉你规则，Tracer 让你在自己的终端里验证规则。
// 在任何类里放一个 Tracer 成员，或者直接用 Tracer 当作元素类型，就能观察它的生命周期。
// =============================================================================
#pragma once
#include <iostream>
#include <string>
#include <utility>

// 统计各种操作发生的次数（放在类外：g++ 不允许在外层类定义完成前使用嵌套类的默认成员初始化器）
struct TracerStats { int ctor = 0, copy_ctor = 0, move_ctor = 0, copy_assign = 0, move_assign = 0, dtor = 0; };

class Tracer {
public:
    using Stats = TracerStats;
    static inline Stats stats{};      // static inline：所有 Tracer 共享一份，C++17 起可在类内定义
    static inline bool verbose = true;

    explicit Tracer(std::string name = "anon") : name_(std::move(name))
    {
        ++stats.ctor;
        log("ctor");
    }
    Tracer(const Tracer& other) : name_(other.name_ + "'")       // 拷贝出来的对象名字加一撇，方便辨认
    {
        ++stats.copy_ctor;
        log("COPY ctor   <- " + other.name_);
    }
    Tracer(Tracer&& other) noexcept : name_(std::move(other.name_))
    {
        other.name_ = "(moved-from)";
        ++stats.move_ctor;
        log("MOVE ctor");
    }
    Tracer& operator=(const Tracer& other)
    {
        ++stats.copy_assign;
        log("COPY assign <- " + other.name_);
        name_ = other.name_ + "'";
        return *this;
    }
    Tracer& operator=(Tracer&& other) noexcept
    {
        ++stats.move_assign;
        log("MOVE assign <- " + other.name_);
        name_ = std::move(other.name_);
        other.name_ = "(moved-from)";
        return *this;
    }
    ~Tracer()
    {
        ++stats.dtor;
        log("dtor");
    }

    const std::string& name() const { return name_; }

    static void reset() { stats = {}; }
    static void print_stats(const char* title)
    {
        std::cout << "  [" << title << "] ctor=" << stats.ctor << " copy=" << stats.copy_ctor
                  << " move=" << stats.move_ctor << " copy_assign=" << stats.copy_assign
                  << " move_assign=" << stats.move_assign << " dtor=" << stats.dtor << '\n';
    }

private:
    void log(const std::string& what) const
    {
        if (verbose) std::cout << "    Tracer(" << name_ << ") " << what << '\n';
    }
    std::string name_;
};

// 打印分节标题
inline void section(const char* title) { std::cout << "\n=== " << title << " ===\n"; }
