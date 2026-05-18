#pragma once
#include <string>
#include <vector>

// 内部工具函数 - 使用者不应该知道
namespace internal {
    std::string trim(const std::string& str);
    std::vector<std::string> split(const std::string& str, char delimiter);
    bool isWhitespace(char c);
}
