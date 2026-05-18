#pragma once
#include <string>
#include <map>

// 公共API - 使用者会看到
class JsonParser {
public:
    bool parse(const std::string& json);
    std::string getString(const std::string& key);
    int getInt(const std::string& key);
    
private:
    std::map<std::string, std::string> data_;
};
