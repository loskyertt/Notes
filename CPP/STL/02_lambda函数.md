# 1. 基本概念与语法

C++11 引入了 lambda 表达式，它是一种轻量级的匿名函数对象。Lambda 表达式通常用于封装代码片段，特别是在需要将函数作为参数传递给 **STL 算法**、处理回调函数或进行简单的局部逻辑封装时非常有用。

## 1.1 基本语法结构

一个完整的 lambda 表达式包含以下几个部分：

```cpp
[ 捕获列表 ] ( 形参列表 ) 可选项 -> 返回类型 {
    // 函数体
};
```

- **捕获列表**：指定 lambda 如何访问其定义作用域中的外部变量。
- **形参列表**：与普通函数类似，对于无参 lambda，括号 `()` 可以省略（C++11 起在无捕获时可用，C++14 起完全支持省略）。
- **可选项**：包括 `mutable`、`constexpr` (C++17)、`consteval` (C++20) 等。
- **返回类型**：可省略，编译器会根据 `return` 语句自动推导。

## 1.2 最简示例

以下是一个最基础的 lambda 定义与调用示例：

```cpp
#include <iostream>

int main() {
    // 定义一个 lambda 表达式并将其赋值给变量 greet
    auto greet = []() {
        std::cout << "Hello, lambda!" << std::endl;
    };
    
    greet();  // 调用 lambda 函数，输出：Hello, lambda!
}
```

---

# 2. 捕获列表

捕获列表是 lambda 表达式的核心，它决定了 lambda 函数体如何访问外部作用域的变量。捕获方式主要分为值捕获和引用捕获。

## 2.1 捕获方式详解

| 语法 | 含义 |
| :--- | :--- |
| `[]` | 不捕获任何外部变量。 |
| `[x]` | 以值方式捕获变量 `x`。lambda 内部生成 `x` 的副本，修改不影响外部。 |
| `[&x]` | 以引用方式捕获变量 `x`。修改会直接影响外部变量。 |
| `[=]` | 以值方式捕获所有外部变量（包括 `this` 指针）。 |
| `[&]` | 以引用方式捕获所有外部变量。 |
| `[=, &x]` | 默认值捕获所有变量，但变量 `x` 例外，使用引用捕获。 |
| `[&, x]` | 默认引用捕获所有变量，但变量 `x` 例外，使用值捕获。 |
| `[this]` | 捕获当前类的 `this` 指针，用于在 lambda 内部访问类成员。 |
| `[*this]` | C++17 起，以值方式捕获当前对象的副本，防止悬垂指针。 |

## 2.2 值捕获与引用捕获的区别

**值捕获**类似于函数传值参数，lambda 创建时会拷贝变量。默认情况下，值捕获的变量在 lambda 内是只读的（除非使用 `mutable`）。

```cpp
int a = 10;
auto f = [a]() { // 值捕获 a
    // a = 20;   // 错误：无法修改只读变量
    return a;
};
```

**引用捕获**类似于函数传引用参数，lambda 内部直接操作外部变量。

```cpp
int a = 10;
auto f = [&a]() { // 引用捕获 a
    a = 20;       // 正确：直接修改外部变量 a
};
```

---

# 3. 函数体特性

## 3.1 mutable 关键字

默认情况下，lambda 的 `operator()` 是 `const` 成员函数。这意味着如果 lambda 以值捕获方式捕获了变量，在函数体内无法修改这些副本。

如果需要修改值捕获的变量副本，必须使用 `mutable` 关键字。

```cpp
int x = 10;

// 未加 mutable：x 是只读副本
// auto f1 = [x]() { x += 5; }; // 编译错误

// 添加 mutable：x 是可修改副本
auto f2 = [x]() mutable {
    x += 5;      // 修改的是 lambda 内部的副本
    std::cout << "Inside: " << x << std::endl; // 输出 15
};

f2();
std::cout << "Outside: " << x << std::endl; // 输出 10 (外部不受影响)
```

## 3.2 返回类型推导

Lambda 表达式支持自动返回类型推导。

**C++11 规则**：
如果 lambda 体中没有 `return` 语句，返回类型为 `void`。如果只有一条 `return` 语句，编译器自动推导返回类型。如果有多条 `return` 语句，它们必须返回相同的类型，否则需要显式指定返回类型。

**C++14 及以后**：
编译器支持更强大的 `auto` 推导规则（类似于函数返回类型推导），即使用户有多条 `return` 语句，只要类型一致或可转换，通常也能处理。

示例：

```cpp
// 单条 return，自动推导为 int
auto add = [](int x, int y) { 
    return x + y; 
};

// 多条 return，类型一致，自动推导为 int
auto check = [](int x) {
    if (x > 0) return 1;
    return -1;
};

// 多条 return，类型不一致（int 和 double），推导可能产生二义性或报错
// 建议显式指定返回类型
auto cast_func = [](int x) -> double {
    if (x > 0) return 1;    // int 隐式转为 double
    return 3.14;            // double
};
```

---

# 4. 进阶特性与用法

## 4.1 Lambda 的底层原理

Lambda 表达式在编译期会被编译器转换为唯一的**匿名类**（闭包类型）。
- 该类重载了 `operator()`。
- 捕获列表中的变量会变成该类的成员变量。
- 值捕获对应类的成员变量初始化，引用捕获对应类的引用成员。

这就解释了为什么每个 lambda 表达式的类型都是唯一的，即使它们的签名看起来一样。

## 4.2 存储 Lambda

由于 lambda 类型是唯一的匿名类，通常使用 `auto` 关键字来存储 lambda 对象。如果需要将 lambda 作为函数参数传递或存储在容器中，可以使用 `std::function`。

```cpp
#include <functional>
#include <iostream>

// 使用 auto 存储（性能最优，类型精确）
auto lambda_auto = [](int x) { return x * 2; };

// 使用 std::function 存储（类型擦除，有一定开销）
std::function<int(int)> lambda_func = [](int x) { return x * 2; };

std::cout << lambda_auto(5) << std::endl;
std::cout << lambda_func(5) << std::endl;
```

## 4.3 递归 Lambda

Lambda 表达式默认无法直接递归调用自身，因为在 lambda 内部，该对象尚未定义完毕，无法通过变量名引用自己。

要实现递归，通常有两种方法：

1. **使用 `std::function`**：虽然性能稍低，但语法直观。
    ```cpp
    #include <functional>
    #include <iostream>

    std::function<int(int)> factorial = [&factorial](int n) -> int {
        if (n <= 1) return 1;
        return n * factorial(n - 1);
    };
    ```

2. **使用通用 lambda (C++14) 传递自身**：性能更好。
    ```cpp
    auto factorial = [](auto&& self, int n) -> int {
        if (n <= 1) return 1;
        return n * self(self, n - 1); // 将 self 作为参数传递
    };
    
    // 调用方式
    std::cout << factorial(factorial, 5) << std::endl;
    ```

## 4.4 泛型 Lambda (C++14)

C++14 允许在形参列表中使用 `auto`，使得 lambda 像模板函数一样工作。

```cpp
auto add = [](auto a, auto b) {
    return a + b;
};

std::cout << add(1, 2) << std::endl;       // int
std::cout << add(1.5, 2.5) << std::endl;   // double
```

C++20 进一步支持了模板语法：

```cpp
auto add = []<typename T>(T a, T b) {
    return a + b;
};
```

---

# 5. 常见陷阱与最佳实践

## 5.1 悬垂引用

这是 lambda 最危险的陷阱。当 lambda 以引用方式捕获局部变量，但 lambda 的生命周期超过了该局部变量的作用域时，引用变成了悬垂引用，导致未定义行为。

**错误示例**：

```cpp
#include <functional>

std::function<int()> create_func() {
    int x = 10;
    // 错误：x 是局部变量，函数结束后销毁
    // 返回的 lambda 持有 x 的悬垂引用
    return [&x]() { return x; }; 
}
```

**正确做法**：
- 如果 lambda 需要传出或长期存储，优先使用值捕获 `[=]` 或 `[x]`。
- 如果必须捕获 `this` 指针且对象可能先于 lambda 销毁，使用 C++17 的 `[*this]` 捕获对象副本。

## 5.2 捕获 this 的陷阱

在类的成员函数中定义 lambda 时，如果使用 `[=]` 或 `[&]`，会隐式捕获 `this` 指针。如果 lambda 在对象销毁后被调用，访问成员变量会导致崩溃。

**推荐做法 (C++17)**：

```cpp
class Processor {
    int value = 42;
public:
    std::function<int()> get_callback() {
        // 使用 [*this] 捕获当前对象的副本
        // 这样即使 Processor 对象销毁，lambda 也是安全的
        return [*this]() {
            return value; 
        };
    }
};
```

## 5.3 最佳实践总结

1.  **优先使用 `auto`**：除非需要类型擦除（如存入容器），否则不要用 `std::function`，因为 `auto` 能生成最高效的代码。
2.  **谨慎使用引用捕获 `[&]`**：仅在确定 lambda 生命周期短于外部变量时使用（如局部 STL 算法）。
3.  **显式捕获优于隐式捕获**：写 `[x, &y]` 比 `[=, &y]` 或 `[&, y]` 更清晰，能减少意外修改的风险。
4.  **注意生命周期**：返回 lambda 或异步调用 lambda 时，务必检查捕获的变量是否依然有效。