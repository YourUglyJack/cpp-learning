#include <iostream>
#include <string>
#include <string_view>
#include "../common/myutils.h"

// string_view: {指针, 长度}
// 不拥有data，就是靠移动 ptr 和长度 计算 string
// 原始 string 销毁 或 reassign 后, string_view 可能悬空

void print_symbol(std::string_view symbol) { // 接收参数时无需复制字符串数据
    std::cout << "symbol=" << symbol << " length=" << symbol.size() << '\n';
}

int main() {
    print_title("string_view");
    std::string line = "AAPL,100,190.25";
    std::string_view view(line); // 指向 line 的 char 数组, 不拥有内存
    print_symbol(view);

    const auto delimiter = view.find(',');
    std::string_view sym = view.substr(0, delimiter);
    // stringview 内部是{ptr, len}, data() 只有 ptr, 没有长度信息
    // cout 会从 ptr 开头 print 到 \0 结束位置
    std::cout << "addr of sym = " << (void*)sym.data() << "\n";
    std::cout << "addr of line = " << (void*)line.data() << "\n";
    print_symbol(sym);

    // arg: 移除的 char 个数
    view.remove_prefix(delimiter+1); // 只移动view起点，不会改line
    print_symbol(view);
    std::cout << line << "\n";
}