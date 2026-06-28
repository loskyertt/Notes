---
title: alignof 与内存对齐
date: 2026-06-26
tags:
  - CPP
  - 内存管理
  - 内存对齐
  - C++11
  - alignof
aliases:
  - alignof
  - alignment requirement
  - 对齐要求
  - 对齐运算符
---

# 1. 背景与核心问题

阅读内存池、分配器或 SIMD 代码时，常见到：

```cpp
alignof(T)
alignof(void*)
align_up(size, alignof(void*));
```

`alignof` 与 `sizeof` 形似但语义完全不同：

| 运算符 | 询问的问题 | 回答 |
|---|---|---|
| `sizeof(T)` | 这个对象**占多少字节**？ | 占用空间大小 |
| `alignof(T)` | 这个对象要求**按多少字节对齐**？ | 地址对齐要求 |

> [!summary]
> `sizeof` 关心“占多大”，`alignof` 关心“放哪里”。二者是两个独立的概念，不能互相替代。

---

# 2. `alignof` 运算符

## 2.1 定义

`alignof` 是 C++11 引入的一元运算符，用于查询**类型 `T` 的对齐要求（alignment requirement）**，返回值为 `std::size_t`。

```cpp
std::size_t a = alignof(T);
```

其含义为：类型 `T` 的对象必须放置在地址为 `alignof(T)` 整数倍的位置上。

> [!info]
> `alignof` 的操作数是一个**类型**（与 `sizeof` 一样），不能直接传变量名，但可以传 `decltype(expr)`。

## 2.2 基本示例

```cpp
#include <iostream>

int main()
{
    std::cout << alignof(char)   << '\n';
    std::cout << alignof(short)  << '\n';
    std::cout << alignof(int)    << '\n';
    std::cout << alignof(double) << '\n';
}
```

64 位平台典型输出：

```text
1
2
4
8
```

> 单位均为字节。

## 2.3 常见类型的对齐值（64 位平台典型值）

| 类型 | `sizeof` | `alignof` |
|---|---:|---:|
| `char` | 1 | 1 |
| `short` | 2 | 2 |
| `int` | 4 | 4 |
| `double` | 8 | 8 |
| `void*` | 8 | 8 |
| `long long` | 8 | 8 |

> [!warning]
> 上表是典型值，并非标准规定。具体数值由**编译器 + 目标 ABI**共同决定，跨平台时必须实测。

---

# 3. “N 字节对齐”的含义

以 `double d;` 为例，假设 `alignof(double) == 8`：

```text
地址 0x1000  →  0x1000 % 8 == 0   ✅ 合法（8 字节对齐）
地址 0x1004  →  0x1004 % 8 == 4   ❌ 非法（未 8 字节对齐）
```

若将 `double` 放置在未对齐地址上，不同平台行为差异显著：

| 平台 | 行为 |
|---|---|
| x86 / x86-64 | 硬件自动处理，但性能下降（跨缓存行/跨总线） |
| 部分 ARM | 触发对齐异常，程序崩溃 |
| 部分 MIPS / 嵌入式 RISC | 总线错误（Bus Error） |

因此编译器会在布局阶段主动插入**填充字节（padding）**，保证每个成员满足其对齐要求。

---

# 4. `alignof` 与 `sizeof` 的区别

## 4.1 概念对比

- `sizeof(T)`：`T` 实例**实际占用**的字节数（含内部 padding）。
- `alignof(T)`：`T` 实例**起始地址**必须满足的对齐约束。

二者没有必然的相等关系。

## 4.2 通过结构体理解 padding

```cpp
struct A { char c; };

struct B { double d; };

struct C { char c; double d; };
```

典型测试结果：

| 类型 | `sizeof` | `alignof` | 说明 |
|---|---:|---:|---|
| `A` | 1 | 1 | 单 `char`，无填充 |
| `B` | 8 | 8 | 单 `double`，自然对齐 |
| `C` | 16 | 8 | `char` 后插入 7 字节 padding，使 `d` 落在 8 字节边界 |

## 4.3 内存布局示意

`struct C` 的内部布局（小端序，地址递增）：

```text
偏移:  0    1    2    3    4    5    6    7    8         15
     +----+----+----+----+----+----+----+----+----+......+----+
     | c  | <---------- padding ----------> |      d (8B)      |
     +----+----+----+----+----+----+----+----+----+......+----+
```

- `c` 占 1 字节，位于偏移 0。
- `d` 要求 8 字节对齐，因此必须从偏移 8 开始，中间 7 字节为 padding。
- `sizeof(C) == 16`：尾部无需再填充（16 已是 8 的倍数）。

> [!tip]
> 记忆技巧：结构体大小必须是**其最大成员 `alignof` 的整数倍**，以便数组中相邻元素都满足对齐。

---

# 5. `alignof(void*)` 的含义

## 5.1 不是 `void` 的对齐

```cpp
alignof(void*)   // ✅ 指针对象的对齐要求
alignof(void)    // ❌ 编译错误：void 是不完整类型
```

`alignof(void*)` 查询的是**指针类型对象**本身（作为变量）存放时所需的对齐，而非“void 的对齐”。

```cpp
void* p;   // 变量 p 本身需要放在 alignof(void*) 对齐的地址上
```

## 5.2 平台差异

| 平台 | `sizeof(void*)` | `alignof(void*)` |
|---|---:|---:|
| 64 位（x86-64 / ARM64） | 8 | 8 |
| 32 位（x86 / ARM32） | 4 | 4 |

> [!warning]
> 标准并未规定 `sizeof(void*) == alignof(void*)`。绝大多数主流 ABI 恰好相等，但这只是约定，不应作为通用假设。

---

# 6. 内存池为何使用 `alignof(void*)`

## 6.1 FreeNode 的对齐要求

内存池通常将空闲块解释为链表节点：

```cpp
struct FreeNode
{
    FreeNode* next;
};
```

空闲块的**起始地址**会被当作 `FreeNode*` 访问，其中第一个成员就是 `next` 指针。若该地址未满足 `alignof(void*)`：

- **x86**：可运行但访问性能下降（跨缓存行/总线拆分）。
- **部分 RISC / 嵌入式**：直接硬件异常。

> [!info]
> 因此，内存池分配出的每一块，至少需要满足**指针类型的对齐要求**，即 `alignof(void*)`。

## 6.2 推荐写法

```cpp
// 将块大小向上对齐到指针类型的对齐要求
block_size = align_up(block_size, alignof(void*));
```

而不是：

```cpp
block_size = align_up(block_size, sizeof(void*));   // 语义不准确
```

---

# 7. 为什么不直接用 `sizeof(void*)`

## 7.1 在主流平台恰好相等

| 平台 | `sizeof(void*)` | `alignof(void*)` | 相等？ |
|---|---:|---:|:---:|
| x86-64 | 8 | 8 | ✅ |
| ARM64 | 8 | 8 | ✅ |
| x86 (32 位) | 4 | 4 | ✅ |

因此在主流平台，老代码写 `sizeof(void*)` 通常也能正常工作。

## 7.2 语义差异

二者表达的不是同一件事：

| 表达式 | 语义 |
|---|---|
| `sizeof(void*)` | 指针**占多少字节** |
| `alignof(void*)` | 指针**应放在什么对齐的地址上** |

> [!tip]
> 现代 C++ 推荐使用 `alignof(void*)`：
> - 它直接表达了"按指针类型的对齐要求对齐"这一**真实意图**；
> - 在 `sizeof` 与 `alignof` 不一致的平台上仍然正确；
> - 可读性更强，便于审阅者理解设计目的。

---

# 8. 自定义对齐：`alignas`

## 8.1 显式指定对齐

`alignas`（C++11）允许显式指定类型或对象的对齐要求：

```cpp
struct alignas(32) BigObject
{
    char data[32];
};

std::cout << sizeof(BigObject)   << '\n';  // 32
std::cout << alignof(BigObject)  << '\n';  // 32
```

## 8.2 `sizeof` 与 `alignof` 始终独立

将声明改为 `alignas(64)`：

```cpp
struct alignas(64) BigObject { char data[32]; };

sizeof(BigObject)   // 64  （尾部填充以使大小为 64 的倍数）
alignof(BigObject)  // 64
```

即使二者数值相同，仍代表两个独立维度：

- `sizeof`：**占多少空间**（含 padding）。
- `alignof`：**必须放在哪里**（地址约束）。

> [!warning]
> 不要因 `sizeof(T) == alignof(T)` 而认为二者可以互换——这只是在特定情况下数值相等，语义完全不同。

---

# 9. 易错点

> [!warning]
> **常见误区清单**
>
> 1. **误用变量作参数**：`alignof(x)`（`x` 是变量）是编译错误，必须传类型，如 `alignof(decltype(x))`。
> 2. **混淆 `void` 与 `void*`**：`alignof(void)` 非法；`alignof(void*)` 才合法。
> 3. **假设 `sizeof == alignof`**：仅在主流平台部分类型上成立，跨平台不可依赖。
> 4. **以为 `alignof` 可设置对齐**：`alignof` 只读查询；设置对齐应使用 `alignas`。
> 5. **忽略结构体尾部 padding**：若最大成员 `alignof` 为 `N`，则结构体 `sizeof` 会被向上取整到 `N` 的倍数。
> 6. **硬编码对齐值**：写死 `8` / `16` 会丧失可移植性，应使用 `alignof(T)` 或 `alignof(std::max_align_t)`。

---

# 10. 总结

## 10.1 两个运算符一句话记忆

| 运算符 | 含义 |
|---|---|
| `sizeof(T)` | 对象占用多少字节 |
| `alignof(T)` | 对象要求按多少字节对齐 |

## 10.2 关于 `alignof(void*)`

- 表示 **`void*` 类型对象**的对齐要求（而非 `void` 本身）。
- 主流平台上 `sizeof(void*) == alignof(void*)`，故老代码用 `sizeof(void*)` 通常也能工作。
- **从语义、可移植性与表达意图角度，内存对齐场景应使用 `alignof(void*)`**。

