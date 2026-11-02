#include <deque>
#include <iostream>
#include <list>
#include <string>
#include "../common/myutils.h"

// deque 分段连续数组： 指针数组 + 固定大小块
// list 双向链表

void print_deque(const std::deque<std::string>& queue) {
    std::cout << "---------- Print deque --------------\n";
    for (const std::string& msg : queue) {
        std::cout << msg << ", ";
    }
    std::cout << "\n------------End printing-----------\n";
}

void print_list(const std::list<std::string>& lst) {
    std::cout << "---------- Print list --------------\n";
    for (const std::string& msg : lst) {
        std::cout << msg << ", ";
    }
    std::cout << "\n------------End printing-----------\n";
}

int main() {
    print_title("deque and list");
    std::deque<std::string> events;
    events.push_back("trade");
    events.push_front("urgent cancel");
    events.push_back("heartbeat");
    print_deque(events);
    std::cout << "deque front = " << events.front() << ", deque back = " << events.back() << "\n";
    events.pop_front();  // consume msg, FIFO
    std::cout << "after pop_front, front = " << events.front() << "\n";    

    std::list<std::string> pending = {"order-a", "order-b"};
    std::list<std::string> processed = {"order-c"};
    auto it = pending.begin(); // order-a
    ++it; // order-b
    const std::string* addr1 = &*it; // *it 得到元素本身, &(*it) 取地址

    // 目标.splice(插入位置，来源，要转移的结点)
    // pending 的 it 放到 processed 的 末尾
    processed.splice(processed.end(), pending, it);
    const std::string* addr2 = &processed.back();
    print_list(pending);
    print_list(processed);
    std::cout << "splice preserved node addr: " << std::boolalpha << (addr1 == addr2) << "\n";

}