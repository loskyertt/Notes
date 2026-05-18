#include "json_parser.h"  // 只能包含PUBLIC头文件
// #include "internal/string_utils.h"  // 错误！找不到PRIVATE头文件

int main() {
    JsonParser parser;
    parser.parse(R"({"name": "test", "value": 42})");
    
    std::string name = parser.getString("name");
    int value = parser.getInt("value");
    
    return 0;
}
