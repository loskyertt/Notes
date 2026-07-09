> 在 C++17 之前，如果我们想在一个变量里存储多种不同类型的数据（比如既能存 `int` 又能存 `string`），通常会使用 C 语言风格的 `union`。但 `union` 非常危险：它不知道自己当前存的是哪种类型，如果你存入的是 `string` 却按 `int` 去读取，程序会直接崩溃或产生乱码。**`std::variant`** 应运而生。它被称为“类型安全的联合体”（Type-safe Union）。

---

# 1. 原理

从底层原理上看，`std::variant` 实际上是一个**结构体**。它包含：

1. **一个足够大的缓冲区**：大小足以容纳所有可能类型中最大的那个。
    
2. **一个类型索引（Index）**：一个整数，记录当前到底存的是哪种类型。

---

# 2. 基本语法

> `std::variant` 位于 `<variant>` 头文件中。

1. 初始化与赋值

```cpp
#include <variant>
#include <string>
#include <iostream>

std::variant<int, float, std::string> v; // 默认初始化为第一种类型的默认值（这里是 0）

v = 10;            // 现在存的是 int
v = 3.14f;         // 自动切换为 float
v = "Hello World"; // 自动切换为 string，旧数据会自动析构
```

2. 读取数据：`std::get`

读取时必须指定类型。如果类型不匹配，会抛出 `std::bad_variant_access` 异常。

```cpp
try {
    auto val = std::get<std::string>(v);
    std::cout << val << std::endl;
} catch (const std::bad_variant_access& e) {
    std::cout << "类型错误！" << e.what() << std::endl;
}
```

3. 安全检查：`std::get_if`

如果你不想用异常，可以使用指针版本的 `get_if`。如果类型不对，它会返回 `nullptr`。

```cpp
if (auto p = std::get_if<int>(&v)) {
    std::cout << "值是 int: " << *p << std::endl;
}
```

---

# 3. 使用场景

## 3.1 场景 A：处理多种类型的数据流

> 如解析 JSON/配置。

在一个配置项里，值可能是数字、字符串或布尔值。

```cpp
struct Setting {
    std::string name;
    std::variant<int, std::string, bool> value;
};

std::vector<Setting> config = {
    {"Volume", 80},
    {"Username", "Admin"},
    {"Mute", false}
};
```

## 3.2 场景 B：作为状态机的状态存储

在游戏逻辑或协议解析中，状态机在不同状态下需要携带不同的数据。

```cpp
struct IdleState {};
struct MoveState { float x, y; };
struct AttackState { int targetID; };

using State = std::variant<IdleState, MoveState, AttackState>;
```

## 3.3 场景 C：替代简单的类继承（静态多态）

如果你有一组固定的类型，且不想为了使用多态而去写复杂的虚函数和基类指针，`std::variant` 配合 `std::visit` 是极佳的选择。

---

# 4. `std::visit`

`std::visit` 能根据变量当前的类型，自动选择对应的处理逻辑。

```cpp
struct DisplayVisitor {
    void operator()(int i) { std::cout << "整数: " << i << std::endl; }
    void operator()(float f) { std::cout << "浮点数: " << f << std::endl; }
    void operator()(const std::string& s) { std::cout << "字符串: " << s << std::endl; }
};

std::variant<int, float, std::string> myVar = 42;
std::visit(DisplayVisitor{}, myVar); // 输出：整数: 42
```

