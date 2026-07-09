---
title: constexpr 关键字
date: 2026-06-21
tags:
  - C++
  - constexpr
  - 编译期计算
  - 模板元编程
  - C++11
  - C++14
  - C++17
  - C++20
aliases:
  - constexpr
  - 编译期常量
  - constexpr函数
  - constexpr if
---

# 1. 背景与动机

## 1.1 传统 C++ 的编译期计算困境

在 C++11 之前，编译期计算主要依赖模板元编程：

```cpp
// 传统模板元编程 —— 写法晦涩
template<int N>
struct Factorial
{
    static const int value = N * Factorial<N - 1>::value;
};

template<>
struct Factorial<0>
{
    static const int value = 1;
};

int arr[Factorial<5>::value];  // 间接获得编译期常量
```

**痛点：**
- 语法晦涩，可读性差
- 错误信息难以理解
- 不支持浮点数、用户自定义类型
- 本质上是“利用模板实例化的副作用”计算

## 1.2 constexpr 的设计目标

> [!summary]
> **constexpr 的核心目标：将编译期计算从“模板技巧”变成“普通函数调用”，让编译期编程像运行期编程一样自然。**

`constexpr` 是 C++11 引入的关键字，并在 C++14/17/20 中不断增强。它的核心含义是：

> **告诉编译器：这个值（或函数）在满足条件时，可以在编译期求值。**

---

# 2. 核心概念

## 2.1 constexpr 变量

### 2.1.1 基本用法

```cpp
constexpr int N = 1024;
int arr[N];  // 合法：N 是编译期常量
```

`N` 在编译阶段就已经确定为 `1024`，因此可以作为数组长度、模板参数等要求编译期常量的场景。

### 2.1.2 const 不一定是编译期常量

```cpp
int getNum() { return 100; }

const int x = getNum();       // 只读，但不是编译期常量
constexpr int y = 100;        // 编译期常量，且不可修改
constexpr int z = getNum();   // 错误：getNum() 不是 constexpr 函数
```

> [!warning]
> **`const` 仅保证不可修改，不保证编译期求值。`constexpr` 强制编译期求值。**

## 2.2 constexpr 函数

### 2.2.1 基本用法

```cpp
constexpr int square(int x)
{
    return x * x;
}

constexpr int a = square(5);  // 编译期计算：a = 25
```

编译器在编译期直接计算 `square(5)`，相当于：

```cpp
constexpr int a = 25;
```

### 2.2.2 编译器优化视角

```cpp
constexpr int square(int x) { return x * x; }

int main()
{
    constexpr int n = square(10);
    // 编译器直接替换为：constexpr int n = 100;
    // 不生成任何函数调用指令
}
```

### 2.2.3 constexpr 函数也能运行时调用

```cpp
constexpr int square(int x) { return x * x; }

int n;
std::cin >> n;
int result = square(n);  // 合法：运行时调用
```

> [!tip]
> **constexpr 函数 = 既能编译期执行，也能运行期执行。** 取决于调用时的参数是否是编译期已知的。

## 2.3 constexpr 与 const 的区别

| 特性 | `const` | `constexpr` |
|---|---|---|
| 语义 | 只读（不可修改） | 编译期常量 + 不可修改 |
| 编译期求值保证 | 不保证 | 强制要求 |
| 可运行时初始化 | 是 | 否 |
| 可用于数组长度 | 不一定 | 一定可以 |
| 可用于模板参数 | 不一定 | 一定可以 |

```cpp
const int x = rand();       // 合法：运行时初始化
constexpr int y = rand();   // 错误：rand() 不是 constexpr
```

---

# 3. 进阶用法

## 3.1 constexpr 类与构造函数

### 3.1.1 constexpr 构造函数（C++11）

```cpp
class Point
{
public:
    constexpr Point(int x, int y)
        : x_(x), y_(y) {}

    constexpr int x() const { return x_; }
    constexpr int y() const { return y_; }

private:
    int x_;
    int y_;
};

// 编译期构造对象
constexpr Point p(1, 2);
constexpr int px = p.x();  // 编译期求值
```

> [!info]
> C++11 要求 constexpr 构造函数体为空，C++14 放宽了此限制。

### 3.1.2 constexpr 静态成员

```cpp
class Config
{
public:
    static constexpr int BufferSize = 4096;
    static constexpr double Pi = 3.1415926535;
};

char buf[Config::BufferSize];  // 直接使用，无需额外定义
```

C++17 起，`static constexpr` 成员变量默认是 `inline` 的，无需在类外定义。

## 3.2 constexpr if（C++17）

### 3.2.1 基本语法

```cpp
template<typename T>
void print(T value)
{
    if constexpr (std::is_integral_v<T>)
    {
        std::cout << "integer: " << value << '\n';
    }
    else if constexpr (std::is_floating_point_v<T>)
    {
        std::cout << "float: " << value << '\n';
    }
    else
    {
        std::cout << "other type\n";
    }
}
```

### 3.2.2 编译期行为

```cpp
print(10);    // 实例化后只保留 "integer" 分支
print(3.14);  // 实例化后只保留 "float" 分支
```

> [!summary]
> **`if constexpr` 是编译期分支选择，不满足条件的分支不会被实例化。** 这与运行时 `if` 有本质区别——运行时 `if` 要求所有分支都合法。

### 3.2.3 典型应用场景

```cpp
// 场景1：消除 enable_if 的繁琐
template<typename T>
auto getValue(T t)
{
    if constexpr (std::is_pointer_v<T>)
        return *t;           // 指针：解引用
    else
        return t;            // 非指针：直接返回
}

// 场景2：递归终止
template<typename T, typename... Args>
void printAll(T first, Args... rest)
{
    std::cout << first;
    if constexpr (sizeof...(rest) > 0)
        printAll(rest...);   // 有参数时递归
    // 否则此分支直接被丢弃，不会生成无效的递归调用
}
```

## 3.3 constexpr Lambda（C++17）

```cpp
auto square = [](int x) constexpr { return x * x; };

constexpr int result = square(5);  // 编译期计算
```

C++17 起，lambda 如果满足 constexpr 条件则隐式为 constexpr；也可显式声明。

## 3.4 consteval 与 constinit（C++20）

### 3.4.1 consteval —— 强制编译期求值

```cpp
consteval int square(int x)  // 必须编译期求值
{
    return x * x;
}

constexpr int a = square(5);  // OK

int n;
std::cin >> n;
// int b = square(n);          // 错误：consteval 禁止运行时调用
```

> [!tip]
> **consteval = 立即函数（immediate function）**，每次调用都必须在编译期产生结果。相当于 constexpr 的“强制版本”。

### 3.4.2 constinit —— 强制编译期初始化

```cpp
constinit int g_value = 42;  // 必须用编译期常量初始化

int getValue() { return 100; }
// constinit int g_bad = getValue();  // 错误：getValue() 不是 constexpr
```

> [!info]
> `constinit` 保证变量在编译期初始化（静态初始化），避免静态初始化顺序问题（Static Initialization Order Fiasco），但变量本身仍可修改。

### 3.4.3 三者对比

| 关键字 | 语义 | 可修改 | 编译期要求 |
|---|---|---|---|
| `constexpr` | 编译期可求值 | 否 | 可能（运行时也可） |
| `consteval` | 必须编译期求值 | — | 强制（函数） |
| `constinit` | 编译期初始化 | 是 | 强制（初始化） |

---

# 4. 编译期执行的条件与限制

## 4.1 编译期执行的条件

constexpr 函数在编译期执行，必须同时满足：

1. **函数本身是 constexpr**
2. **所有参数都是编译期常量**
3. **函数体内不包含非 constexpr 操作**

```cpp
constexpr int square(int x) { return x * x; }

constexpr int a = square(5);     // 编译期：条件全部满足
int b = square(rand());           // 运行时：参数非编译期常量
```

## 4.2 C++ 各版本限制演变

| 版本 | constexpr 函数限制 |
|---|---|
| **C++11** | 只能包含一条 return 语句；构造函数体必须为空 |
| **C++14** | 允许多条语句、局部变量、循环、if/switch |
| **C++17** | 支持 `if constexpr`、constexpr lambda |
| **C++20** | 支持 `consteval`/`constinit`、`constexpr` 虚函数、`constexpr` 动态内存分配、`constexpr` try-catch |

```cpp
// C++11 写法
constexpr int factorial_11(int n)
{
    return n <= 1 ? 1 : n * factorial_11(n - 1);
}

// C++14 写法（更自然）
constexpr int factorial_14(int n)
{
    int result = 1;
    for (int i = 2; i <= n; ++i)
        result *= i;
    return result;
}
```

---

# 5. 应用场景

## 5.1 替代宏

```cpp
// 宏方式：无类型、无作用域、无 namespace
#define BUFFER_SIZE 1024

// constexpr 方式：类型安全、有作用域
namespace Config {
    constexpr size_t kBufferSize = 1024;
    constexpr double kPi = 3.141592653589793;
}
```

| 对比维度 | 宏 `#define` | `constexpr` |
|---|---|---|
| 类型安全 | 无 | 有 |
| 作用域 | 全局污染 | 命名空间/类作用域 |
| 调试可见 | 符号被替换 | 符号保留 |
| 浮点数支持 | 语法上可行，但无类型 | 原生支持 |

## 5.2 简化模板元编程

**传统模板元编程：**

```cpp
template<int N>
struct Factorial
{
    static constexpr int value = N * Factorial<N - 1>::value;
};

template<>
struct Factorial<0>
{
    static constexpr int value = 1;
};
```

**constexpr 函数：**

```cpp
constexpr int factorial(int n)
{
    return n <= 1 ? 1 : n * factorial(n - 1);
}

constexpr int result = factorial(10);  // 编译期得到 3628800
```

代码量减少，可读性大幅提升。

## 5.3 网络编程示例

```cpp
class Buffer
{
public:
    static constexpr size_t kInitialSize = 1024;
    static constexpr size_t kMaxSize = 65536;
    static constexpr size_t kCheapPrepend = 8;
};

std::vector<char> buffer_(Buffer::kInitialSize);
```

**优势：**
- 类型安全（`size_t`）
- 有类作用域，不会污染全局
- 无宏的副作用
- IDE 可跳转、可补全

---

# 6. 易错点

> [!warning] 易错点 1：constexpr 并不保证编译期执行
> `constexpr` 函数在参数是运行期值时，会在运行期执行。只有 `consteval` 才**强制**编译期执行。

```cpp
constexpr int f(int x) { return x * 2; }
int n;
std::cin >> n;
int r = f(n);  // 运行期执行，完全合法
```

> [!warning] 易错点 2：constexpr 变量 ≠ 编译期常量表达式
> 声明为 `constexpr` 的变量一定是编译期常量，但 `const` 变量不一定是。不要混淆。

> [!warning] 易错点 3：if constexpr 的条件必须是编译期表达式
> 普通 `if` 的条件可以是运行时值，但 `if constexpr` 的条件必须是编译期可确定的。

```cpp
int n;
std::cin >> n;
// if constexpr (n > 0) { ... }  // 错误：n 不是编译期常量
```

> [!warning] 易错点 4：C++11 constexpr 函数限制严格
> 如果项目使用 C++11，constexpr 函数体只能包含一条 return 语句，不能使用循环、局部变量等。

---

# 7. 最佳实践

1. **能用 `constexpr` 就不用 `const`** —— 当值可以在编译期确定时，用 `constexpr` 明确表达意图
2. **用 `constexpr` 替代无类型宏** —— 获得类型安全和作用域控制
3. **模板元编程中优先使用 constexpr 函数** —— 比模板特化更易读、更易调试
4. **C++17+ 项目中善用 `if constexpr`** —— 替代 SFINAE 和 `std::enable_if`，代码更清晰
5. **需要强制编译期求值时用 `consteval`（C++20）** —— 比 `constexpr` 更安全，编译器会强制检查
6. **全局变量用 `constinit`（C++20）** —— 避免静态初始化顺序问题

---

# 8. 总结

> [!summary] 核心理解
> ```text
> const      = 只读（不保证编译期）
> constexpr  = 编译期可确定的常量 + 不可修改 + 编译期计算能力
> consteval  = 强制编译期求值（函数）
> constinit  = 强制编译期初始化（变量，可修改）
> ```

**最常见的三个用途：**

```cpp
constexpr int N = 1024;                         // ① 编译期常量
constexpr int square(int x) { return x * x; }   // ② 编译期函数
if constexpr (condition) { ... }                // ③ 编译期分支
```

在现代 C++（C++17/20）中，`constexpr` 已经是模板编程、库设计和高性能代码中非常核心的关键字，它让编译期计算从“模板魔法”走向了“普通代码”。