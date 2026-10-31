#include <iostream>
#include <stdexcept>
#include <string>

class Account {
private:
    std::string owner_;
    double balance_{0.0};
    int tx_count{0};
public:
    Account(std::string owner, double balance) : owner_(owner), balance_(balance) {}

    explicit Account(std::string owner) : Account(owner, 0.0) {}

    Account& deposit(double amount) {
        if (amount <= 0) {
            throw std::invalid_argument("amount must be positive");
        }
        balance_ += amount;
        return *this;
    }
    Account& withdraw(double amount); // 只声明，定义在外面写

    double balance() const { return balance_;}
    const std::string& owner() const {return owner_;}

    void print_info() const {
        std::cout << owner_ << ": balance = " << balance_ << std::endl; 
    }
};

Account& Account::withdraw(double amount) {
    if (amount <= 0) throw std::invalid_argument("amount must be positive");
    if (amount > balance_) throw std::runtime_error("insufficient fund for " + owner_);
    balance_ -= amount;
    ++tx_count;
    return *this;
}

// ---------------------------------------------------------------------------
// struct 和 class 唯一的区别：struct 默认 public，class 默认 private。
// 习惯：只有数据、没有不变量要维护的“纯数据包”用 struct；有行为、要保护内部状态的用 class。
// ---------------------------------------------------------------------------
struct Quote {
    std::string symbol;
    double bid = 0, ask = 0;
    double mid() const { return 0.5*(bid+ask); }
};

void print_title(std::string title) {
    std::cout << "============= " << title << " ======================\n";
}

int main() {

    print_title("Create Obj");

    Account a("Alice", 100.0);
    Account b{"bob"};

    // Account c = std::string("dave"); explicit 了 所以 不能隐式转换
    // c.print_info();

    a.deposit(50).deposit(30);
    a.print_info();

    a.withdraw(30);
    a.print_info();

    b.print_info();

    print_title("Access contorl");
    // a.balance_ = 1e9; error! private variable
    try {
        b.withdraw(10);
    } catch (const std::exception& e) {
        std::cout << "caught: " << e.what() << "\n";
    }

    print_title("const obj");
    const Account frozen{"Carol", 10.0}; // const 该obj 不能改， frozen 是这个 obj 变量的名字。...
    frozen.print_info();
    // frozen.deposit(1); const 对象 调用 非 const 成员 func 编译错误

    print_title("Alias");
    Account& alias = a; // a的引用
    alias.print_info();

    print_title("struct");
    Quote q{"AAPL", 200.0, 189.1}; // 聚合初始化， 按照成员顺序复制
    std::cout << q.symbol << " bid: " << q.bid << " ask: " << q.ask << "\n";

    // 对象空间 = 成员变量占的内存
    // 函数不在对象空间里，这样 多个 obj 可以 shared the same code
    print_title("obj size is fixed at compile time");
    std::cout << "sizeof(Account) = " << sizeof(Account) << "\n";
    std::cout << "sizeof(Quote) = " << sizeof(Quote) << "\n";
    return 0;
}