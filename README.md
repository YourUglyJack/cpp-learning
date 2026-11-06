# C++ OOP 学习笔记

手敲代码学习 C++ 面向对象编程的核心概念。

## 文件列表

| 文件 | 主题 | 核心知识点 |
|------|------|-----------|
| `class_basic.cpp` | 类基础 | 成员变量、成员函数、访问控制、this 指针 |
| `construct.cpp` | 构造函数 | 默认构造、参数构造、初始化列表、explicit |
| `obj_lifetime.cpp` | 对象生命周期 | 栈对象、堆对象、RAII、作用域 |
| `copy.cpp` | 拷贝语义 | 拷贝构造、拷贝赋值、深拷贝 vs 浅拷贝、Rule of 3 |
| `move.cpp` | 移动语义 | 右值引用、std::move、移动构造、移动赋值、Rule of 5 |
| `static_friend_nested.cpp` | 静态/友元/嵌套 | static 成员、friend 函数、嵌套类 |
| `overload.cpp` | 运算符重载 | operator+、operator<<、operator[]、operator() |
| `inherit.cpp` | 继承基础 | public 继承、protected、构造/析构顺序、函数隐藏 |
| `virtual_poly.cpp` | 虚函数与多态 | virtual、override、虚析构、对象切片 |
| `abc.cpp` | 抽象类与接口 | 纯虚函数、抽象类、依赖注入 |

## 辅助文件

| 文件 | 作用 |
|------|------|
| `tracer.h` | 追踪对象生命周期（构造/拷贝/移动/析构） |
| `myutils.h` | 打印标题等工具函数 |

## 编译运行

```bash
# 编译单个文件
g++ -std=c++20 -Wall -I. move.cpp -o build/move

# 运行
./build/move
```

## 学习顺序

```
1. class_basic     类的基本结构
2. construct       构造函数
3. obj_lifetime    对象生命周期
4. copy            拷贝语义（Rule of 3）
5. move            移动语义（Rule of 5）
6. static_friend   静态成员与友元
7. overload        运算符重载
8. inherit         继承
9. virtual_poly    多态
10. abc            抽象类与接口
```

## 关键概念速查

### Rule of 3/5/0

| 规则 | 需要手写的特殊成员函数 |
|------|----------------------|
| Rule of 0 | 全部 = default，让编译器生成 |
| Rule of 3 | 析构、拷贝构造、拷贝赋值 |
| Rule of 5 | Rule of 3 + 移动构造、移动赋值 |

### 虚函数 vs 非虚函数

| 类型 | 调用依据 | 关键字 |
|------|---------|--------|
| 非虚函数 | 变量声明类型（编译期） | 无 |
| 虚函数 | 实际对象类型（运行期） | virtual |

### 拷贝 vs 移动

| 操作 | 语义 | 性能 |
|------|------|------|
| 拷贝 | 复制数据 | 慢（分配内存 + 复制） |
| 移动 | 偷指针 | 快（只交换指针） |

### 四种 Cast

| Cast | 用途 | 安全性 |
|------|------|--------|
| `static_cast` | 常规类型转换 | 较安全 |
| `dynamic_cast` | 多态向下转换（运行时检查） | 安全 |
| `const_cast` | 去掉/加上 const | 中等 |
| `reinterpret_cast` | 强制重新解释比特位 | 危险 |

```cpp
// static_cast：常规转换
double d = 3.14;
int i = static_cast<int>(d);  // 3

// dynamic_cast：多态向下转换（失败返回 nullptr）
Base* base = new Derived();
Derived* derived = dynamic_cast<Derived*>(base);  // 成功
if (derived) { derived->foo(); }

// const_cast：去掉 const
const int* cp = &i;
int* p = const_cast<int*>(cp);  // 去掉 const

// reinterpret_cast：重新解释比特位（危险）
int* ptr = new int(42);
uintptr_t addr = reinterpret_cast<uintptr_t>(ptr);  // 指针→整数
```

**使用原则**：优先用 `static_cast`，多态用 `dynamic_cast`，尽量避免 `reinterpret_cast`。
