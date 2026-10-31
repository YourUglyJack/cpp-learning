#include <iostream>
#include <memory>
#include <vector>
#include "tracer.h"
#include "myutils.h"

Tracer g_global("global"); // 静态存储 main 之前构造 main结束 折构

Tracer make_tracker(const std::string& n) {
    Tracer t(n);
    return t;
}

// 不要返回 局部 ref ！！！！
// 可以返回 局部 obj 因为 会拷贝 return
const Tracer& dangling() {
    Tracer local("local"); // 局部变量，函数结束就没了
    return local; // 返回了一个即将死掉的ref
}

const Tracer& safe() {
    static Tracer safe("static-local"); // static 只创建一次 程序结束后才销毁
    return safe;
}

void take_by_value(Tracer t) {
    std::cout << "      inside take_by_value " << t.name() << "\n"; 
}

//   非 const 引用 (Tracer&)
//     → 你可能想修改它
//     → 但临时对象马上就死了
//     → 修改它没意义
//     → 不允许

//   const 引用 (const Tracer&)
//     → 你只是读取，不修改
//     → 临时对象死之前读一下没问题
//     → 允许
void take_by_ref(const Tracer& t) {
    std::cout << "      inside take_by_ref " << t.name() << "\n";
}

int main() {
    print_title("stack obj");
    {
        Tracer s("Stack");
        std::cout << "    using s....\n";
    } // 折构

    print_title("heap obj");
    Tracer* raw = new Tracer("raw-heap");
    delete raw;

    print_title("heap obj with unique_ptr");
    {
        // 创建 指针
        auto up = std::make_unique<Tracer>("unique");
        std::cout << "    up....." << up->name() << "\n";
    } // 折构时 自动 delete

    print_title("temporary obj die at the end of the full express");
    take_by_ref(Tracer("temp")); // 右值 没名字 临时对象
    std::cout << "      next statement:\n";
    const Tracer& extended = Tracer("lifetime-extended");
    std::cout << "      extended is still alive " << extended.name() << "\n";
    
    print_title("Pass by value copies, pass by const& does not");
    Tracer arg("arg");
    take_by_value(arg); // copy value 直接pass value 是要copy的
    take_by_ref(arg);  // pass directly, no copy 

    print_title("return by value, copy elision");
    Tracer r = make_tracker("returned");
    std::cout << "      got " << r.name() << "\n";

    print_title("static local: constructed on first use");
    std::cout << "before first call \n";
    const Tracer& first = safe();
    std::cout << "      first call: " << first.name() << "\n";
    std::cout << "      second call: " << safe().name() << "\n";


    print_title("push_back and emplace_bake");
    {
        std::vector<Tracer> v;
        std::cout << "---- push_back ----- \n";
        v.push_back(Tracer("v0"));
        std::cout << "---- emplace_back ----- \n";
        v.emplace_back("v1");
        std::cout << "test end \n";
    }

    print_title("obj inside a vector live on the heap and move when it grows");
    {
        std::vector<Tracer> v;
        v.emplace_back("v0"); // 直接在 container 里 构造 
        v.emplace_back("v1"); // 扩容，v0 会移动
        v.emplace_back("v2"); // 扩容， v0 v1 会移动
        
        std::cout << "      with reserve(3) the moves disappear\n";
        std::vector<Tracer> w;
        w.reserve(3);
        w.emplace_back("w0");
        w.emplace_back("w1");
        w.emplace_back("w2");
        std::cout << "test end \n";
    }
    print_title("end");
}