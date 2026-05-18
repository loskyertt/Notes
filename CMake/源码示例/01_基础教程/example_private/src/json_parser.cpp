#include "json_parser.h"
#include "internal/string_utils.h"  // PRIVATE头文件 - 只有自己用

using namespace internal;

bool JsonParser::parse(const std::string& json) {
    // 使用内部工具函数
    std::string trimmed = trim(json);
    // 解析逻辑...
    return true;
}

std::string JsonParser::getString(const std::string& key) {
    return data_[key];
}

int JsonParser::getInt(const std::string& key) {
    return std::stoi(data_[key]);
}
