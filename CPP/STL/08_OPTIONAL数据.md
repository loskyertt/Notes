> 在 C++17 之前，处理“可能不存在的数据”通常比较麻烦。我们不得不使用特殊值（如 `-1` 或 `0`）、空指针 `nullptr`，或者一个额外的 `bool` 变量来表示数据是否有效。

> `std::optional` 的出现提供了一个**更优雅、类型安全**的容器，专门用于表示“有值”或“无值”的状态。

---

# 1. 基本语法

> s`std::optional` 位于 `<optional>` 头文件中。

1. 初始化

```cpp
#include <optional>
#include <string>

std::optional<int> score;            // 初始状态：无值 (nullopt)
std::optional<int> score2 = 100;     // 初始状态：有值
std::optional<int> score3 = std::nullopt; // 显式声明无值
```

2. 检查与读取值

```cpp
if (score.has_value()) {             // 检查是否有值
    int val = score.value();         // 获取值（若无值会抛出异常）
}

// 或者是更简洁的写法：
if (score) {                         // 类似指针的 bool 转换
    int val = *score;                // 类似指针的解引用
}
```

3. 安全读取：`value_or`

> 这是最常用的功能。如果没值，就返回一个你指定的默认值。

```cpp
// 如果 score 没值，就返回 0
int finalScore = score.value_or(0); 
```

---

# 2. 使用场景

> `std::optional` 不分配堆内存，这是相比指针的最大优势。`std::optional<T>` 的数据是 **直接存在栈上的**（它内部包含一个 `T` 类型的数据成员和一个 `bool` 标志）。它 **没有动态内存分配** 的开销，性能和原生 `T` 几乎一样快。

## 2.1 场景 A：作为函数的返回值

当一个函数可能无法计算出结果时，返回 `std::optional`。

```cpp
std::optional<std::string> GetName(int id) {
    if (id == 1) return "Alice";
    return std::nullopt;  // 找不到，不返回错误代码，直接返回“空”
}

int main() {
    auto name = GetName(2);
    std::cout << name.value_or("Unknown User");
}
```

## 2.2 场景 B：可选的类数属性

有些数据成员并不是每个对象都必须有的。比如，`User` 类里的“中间名”。

```cpp
struct User {
    std::string firstName;
    std::optional<std::string> middleName; // 有些人没中间名
    std::string lastName;
};
```

## 2.3 场景 C：延迟初始化

有时候你不想在构造函数里立即初始化某个对象，可以用 `optional` 占位，稍后再赋值。

---

# 4. 其他

在最新的 C++23 中，`optional` 变得像函数式编程一样强大，支持链式调用：

```cpp
// 伪代码示例：如果 name 有值，则转大写，否则返回默认
auto result = GetName(1)
                .transform([](auto s) { return ToUpper(s); })
                .value_or("NONE");
```


> Q：什么时候用？

|**想要表达...**|**推荐工具**|
|---|---|
|可能不存在的简单数值|`std::optional<T>`|
|可能不存在的大型对象且不想拷贝|`std::optional<T>` 或 **指针**|
|函数执行失败（需包含错误信息）|`std::expected` (C++23)|
|必须指向存在的对象|引用 `T&`|