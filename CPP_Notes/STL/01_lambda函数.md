在 C++ 中，lambda 函数（也叫 lambda 表达式）是一种轻量级、匿名的函数对象，主要用于简化一次性小函数的编写，常见于 STL 算法、回调函数、并发任务等场景。

---

# 1.基本语法

```
[ 捕获列表 ] ( 形参列表 ) 可选项 -> 返回类型 {
    // 函数体
};
```

- **捕获列表**：指定 lambda 表达式可以访问的外部变量。
- **形参列表**：与普通函数一样，可省略（无参时）。
- **可选项**：如 `mutable`、`constexpr`、`noexcept` 等。
- **返回类型**：可省略，编译器会自动推导。

示例：

```cpp
#include <iostream>

int main() {
    auto greet = []() {
        std::cout << "Hello, lambda!" << std::endl;
    };
    greet();  // 输出：Hello, lambda!
}
```

---

# 2.捕获列表（Capture Clause）

捕获列表指定了 lambda 如何访问其定义作用域中的变量。

| 语法           | 含义                                           |
|----------------|------------------------------------------------|
| `[]`           | 不捕获任何外部变量                             |
| `[x]`          | 以值方式捕获变量 `x`                           |
| `[&x]`         | 以引用方式捕获变量 `x`                         |
| `[=]`          | 以值方式捕获所有外部变量                       |
| `[&]`          | 以引用方式捕获所有外部变量                     |
| `[=, &x]`      | 值捕获所有变量，但 `x` 以引用方式捕获          |
| `[this]`       | 捕获当前对象的 `this` 指针                     |
| `[*this]`      | C++17 起，以值方式捕获当前对象的拷贝（防悬垂） |

示例：

```cpp
int a = 10, b = 20;

auto f1 = [a, &b]() {
    // a 是值捕获，b 是引用捕获
    b += a;
};

f1();  // b 变为 30
```

---

# 3.返回类型与类型推导

~~如果 lambda 体中只有一条 `return` 语句，编译器可自动推导返回类型。否则需显式指定返回类型。~~

```cpp
auto add = [](int x, int y) -> int {
    return x + y;
};
```

这是 `C++14 ` 之前的一条老规则，现在已经过时了。

现代写法：
```cpp
auto add = [](int x, int y) {   // 可以省略 -> int
    return x + y;
};
```

有**多条 `return` 语句**也可以使用 `auto`，但 **所有 `return` 表达式必须能推导出同一个类型**，否则编译器无法决定 lambda 的返回类型，就会报错。

示例 1：合法（两条 `return` 都是 `int`）

```cpp
auto f = [](int x) {
    if (x > 0) return 1;   // int
    return 0;              // int   —— 类型一致
};
```

示例 2：非法（返回类型不一致）

```cpp
auto f = [](int x) {
    if (x > 0) return 1;   // int
    return 3.14;           // double —— 编译错误
};
```

示例三：显式指定返回类型（正确）

```cpp
auto f = [](int x) -> double {
    if (x > 0) return 1;   // 1 会隐式转成 double
    return 3.14;
};
```

---

# 4.lambda 的本质与类型

- lambda 表达式在编译期会生成一个**匿名类**（闭包类型），并重载了 `operator()`。
- 每个 lambda 表达式的类型是**唯一的**，即使签名相同。
- 可以用 `auto` 或 `std::function` 来保存 lambda。

```cpp
auto lambda = [](int x) { return x * 2; };
```

或者：

```cpp
// std::function< 返回类型(参数1类型, 参数2类型, ...) >
std::function<int(int)> lambda =  [](int x) { return x * 2; };
```

> 如果要 **递归** 调用 lambda 函数，就不能用 `auto` 了，得用 `std::function` 显示声明。因为**当用 `auto` 声明变量时，在整条声明语句结束之前，这个变量名还不能被当成一个完整类型来使用**。
> C++ 的 lambda 默认不能捕获自身，在递归调用 lambda 函数时，还需要通过捕获列表 `[&]` 显示捕获自身变量，

---

# 5.常见使用场景

1. 与 STL 算法结合

```cpp
#include <algorithm>
#include <vector>

std::vector<int> v = {1, 2, 3, 4, 5};
std::for_each(v.begin(), v.end(), [](int n) {
    std::cout << n << " ";
});
```

2. 作为回调函数

```cpp
auto callback = [](int result) {
    std::cout << "Result: " << result << std::endl;
};
```

3. 并发任务（如 `std::thread`）

```cpp
#include <thread>

int x = 42;
std::thread t([&x]() {
    x += 10;
});
t.join();
```

4. C++20 起支持模板 lambda

```cpp
auto generic = []<typename T>(T a, T b) {
    return a + b;
};
std::cout << generic(1, 2) << std::endl;     // int
std::cout << generic(1.1, 2.2) << std::endl; // double
```

---

# 6.注意事项与陷阱

| 问题类型     | 说明与建议 |
|--------------|------------|
| **悬垂引用** | 引用捕获局部变量时，确保 lambda 的生命周期不超过变量作用域。 |
| **捕获 this 的陷阱** | 如果对象已销毁，lambda 中使用 `this` 会悬垂。C++17 可用 `[*this]` 捕获对象副本。 |
| **mutable 关键字** | 默认 lambda 的 `operator()` 是 `const` 的，值捕获的变量不可修改。使用 `mutable` 可去掉 const 限制。 |

```cpp
int x = 10;
auto f = [x]() mutable {
    x += 5;  // 可以修改副本
};
```
