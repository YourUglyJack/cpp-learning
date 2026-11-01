// =============================================================================
// OOP 11：抽象类与接口 —— 面向接口编程、依赖注入、可测试性
// =============================================================================
//
// 【核心概念】
// ┌─────────────────────────────────────────────────────────────────────────────┐
// │  纯虚函数：virtual double price() const = 0;   // "= 0" 表示没有实现         │
// │  抽象类：含有纯虚函数的类，不能创建对象（只能当基类）                          │
// │  接口：只有纯虚函数 + 虚析构的类（C++ 没有 interface 关键字，靠约定）          │
// └─────────────────────────────────────────────────────────────────────────────┘
//
//
// 【金融应用场景】
//   量化交易系统有很多"可替换组件"：
//   - IMarketData：行情源（可以是实盘行情、历史回放、模拟数据）
//   - IOrderGateway：下单通道（可以是真实交易所、模拟撮合、纸交易）
//   - ILogger：日志系统（可以是 stdout、文件、Kafka）
//
//   用接口定义这些组件，业务代码只依赖接口，不关心具体实现。
//   这就是"面向接口编程"的核心思想。
//
// 【依赖注入 (Dependency Injection, DI)】
//   类不自己 new 依赖对象，而是从构造函数接收"接口"。
//   好处：
//   - 生产环境注入真实行情 → 连接交易所
//   - 单元测试注入假行情（Mock）→ 可控可预测
//   - 同一份业务代码不用改！
//
//   ★ 这是工作中写可测试 C++ 代码最重要的一个习惯。
// =============================================================================
#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <vector>
#include "myutils.h"

// interface: pure vitual func + pure dtor
// start with I

// 接口 1：IMarketData 行情源
// 【金融】提供股票/期货/期权的最新价格
// 实盘：连接交易所，获取实时行情
// 回测：从历史数据库读取
// 测试：返回固定价格（Mock）
class IMarketData {
public:
    virtual ~IMarketData() = default;
    virtual double last_price(const std::string& sym) const = 0;
};

// 接口 2：IOrderGateway 下单通道
// 【金融】向交易所发送订单
// 实盘：通过 FIX 协议连接交易所
// 模拟：本地撮合引擎
// 测试：记录订单但不真正执行（Mock）
class IOrderGateway {
public:
    virtual ~IOrderGateway() = default;
    // 发送订单
    virtual bool send(const std::string& sym, int qty, double px) = 0;
};

class ILogger {
public:
    virtual ~ILogger() = default;
    virtual void info(const std::string& msg) = 0;
};

// ===========================================================================
// 业务类：Rebalancer（组合再平衡）
// ===========================================================================
// 【金融背景】
//   再平衡 (Rebalancing) = 调整持仓到目标权重
//   例如：目标权重 AAPL 50%，MSFT 30%，NVDA 20%
//   当价格变化后，实际权重偏离目标，需要买卖调整
//
// 【依赖注入的关键】
//   Rebalancer 只依赖三个接口（IMarketData, IOrderGateway, ILogger）
//   它不知道也不关心：
//   - 行情从哪来（交易所？历史回放？固定值？）
//   - 订单去哪里（真实交易所？模拟器？黑洞？）
//   - 日志写到哪（屏幕？文件？什么都不做？）
class Rebalancer {
public:
    // 依赖注入
    Rebalancer(const IMarketData& md, IOrderGateway& gw, ILogger& log)
        : md_(md), gw_(gw), log_(log)
        {}
    int rebalance(const std::map<std::string, double>& target_w, double capital, std::map<std::string, int>& holdings) {
        int orders = 0;
        for (const auto& [sym, w] : target_w) {
            double px = md_.last_price(sym);
            int target = static_cast<int>(capital * w / px);
            int diff = target - holdings[sym];
            if (diff==0) continue;

            if (gw_.send(sym, diff, px)) {
                holdings[sym] += diff;
                ++orders;
                log_.info("sent " + sym + " " + std::to_string(diff));
            } else {
                log_.info("rejected " + sym);
            }
        }
        return orders;
    }
private:
    const IMarketData& md_;
    IOrderGateway& gw_;
    ILogger& log_;
};

class ConsoleLogger : public ILogger {
public:
    void info(const std::string& msg) override {
        std::cout << "[log] " << msg << "\n"; 
    }
};

class StaticMarketData : public IMarketData {
public:
    double last_price(const std::string& sym) const override {
        static const std::map<std::string, double> px {
            {"AAPL", 190.0},   // 苹果
            {"MSFT", 410.0},   // 微软
            {"NVDA", 950.0},    // 英伟达
            {"VOO", 100.0}
        };
        return px.at(sym); // map::operator[] key 不存在的时候 会插入新元素, 所以 [] 不能用在 const map
    }
};


class SimGateway : public IOrderGateway {
public:
    bool send(const std::string& sym, int qty, double px) override {
        std::cout << "[gateway] " << (qty > 0 ? "BUY " : "SELL ")
                  << std::abs(qty) << ' ' << sym << " @ " << px << '\n';
        return true;  // 总是成功
    }
};

class MockGateway : public IOrderGateway {
public:
    std::vector<std::string> sent;
    std::string reject_sym;

    bool send(const std::string& sym, int qty, double px) override {
        if (sym == reject_sym) return false;
        sent.push_back(sym + " : " + std::to_string(qty));
        return true;
    }
    
    void print_sent() const {
        for (const std::string& msg : sent) {
            std::cout << msg << "\n";
        }
    }
};

class SilentLogger : public ILogger {
public:
    int lines = 0;
    void info(const std::string& ) override { ++lines; }
};

#define CHECK(cond) std::cout << (cond ? "  PASS " : "  FAIL ") << #cond << '\n'

int main() {
    // 我需要某个东西才能工作，但我不负责创建它，你从外面给我
    print_title("dependence injection");
    StaticMarketData md;
    SimGateway gw;
    ConsoleLogger log;

    Rebalancer r(md, gw, log); // injection

    std::map<std::string, int> holdings{{"AAPL", 100}};
    r.rebalance(
        {{"AAPL", 0.5}, {"MSFT", 0.3}, {"NVDA", 0.2}},
        100'000,
        holdings
    );

    MockGateway mock;
    mock.reject_sym = "NVDA";
    SilentLogger quiet;

    print_title("other rebalancer");
    Rebalancer rt(md, mock, log);
    std::map<std::string, int> h;
    rt.rebalance(
        {{"AAPL", 0.5}, {"VOO", 0.4}, {"NVDA", 0.1}},
        100'000,
        h
    );
    mock.print_sent();
}