---
title: 内存对齐、alignof 与 alignas
date: 2026-07-02
tags:
  - CPP
  - 内存管理
  - 内存对齐
  - C++11
  - alignof
  - alignas
aliases:
  - alignof
  - alignas
  - alignment requirement
  - 对齐要求
  - 对齐运算符
---

# 1. 背景与核心问题

阅读内存池、分配器、网络缓冲区或 SIMD 代码时，经常会看到：

```cpp
alignof(T)
alignof(void*)
alignas(32)
align_up(size, alignof(void*))
```

`alignof` 与 `sizeof` 形似，但回答的问题完全不同。

| 运算符 | 询问的问题 | 回答 |
|---|---|---|
| `sizeof(T)` | 这个对象占多少字节 | 对象大小 |
| `alignof(T)` | 这个对象要求放在什么边界上 | 起始地址对齐要求 |

> [!summary]
> `sizeof` 关心“占多大”，`alignof` 关心“放哪里”。二者是独立概念，不能因为数值偶尔相等就互相替代。

---

# 2. `alignof` 运算符

## 2.1 定义

`alignof` 是 C++11 引入的一元运算符，用于查询类型 `T` 的**对齐要求（alignment requirement）**，返回值类型为 `std::size_t`。

```cpp
std::size_t alignment = alignof(T);
```

若 `alignof(T) == N`，则 `T` 类型对象的起始地址必须是 `N` 的整数倍。

> [!info]
> `alignof` 的操作数是类型。若要查询变量对应类型的对齐要求，可以使用 `alignof(decltype(x))`。

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

单位均为字节。

## 2.3 典型值

| 类型 | `sizeof` | `alignof` |
|---|---:|---:|
| `char` | 1 | 1 |
| `short` | 2 | 2 |
| `int` | 4 | 4 |
| `double` | 8 | 8 |
| `void*` | 8 | 8 |
| `long long` | 8 | 8 |

> [!warning]
> 上表只是 64 位主流 ABI 的典型值，不是标准强制值。跨平台代码应以实际编译目标为准。

---

# 3. N 字节对齐

以 `double d;` 为例，假设 `alignof(double) == 8`：

```text
地址 0x1000 -> 0x1000 % 8 == 0  合法
地址 0x1004 -> 0x1004 % 8 == 4  未对齐
```

未对齐访问在不同平台上后果不同：

| 平台 | 可能行为 |
|---|---|
| x86 / x86-64 | 硬件通常能处理，但可能性能下降 |
| 部分 ARM | 可能触发对齐异常 |
| 部分 MIPS / 嵌入式 RISC | 可能出现 Bus Error |

因此编译器会在对象布局中插入**填充字节（padding）**，保证成员起始地址满足对齐要求。

---

# 4. `alignof` 与 `sizeof`

## 4.1 概念对比

- `sizeof(T)`：一个 `T` 对象实际占用的字节数，包含内部和尾部 padding。
- `alignof(T)`：一个 `T` 对象起始地址必须满足的对齐约束。

二者没有必然相等关系。

## 4.2 结构体 padding

```cpp
struct A { char c; };
struct B { double d; };
struct C { char c; double d; };
```

典型结果：

| 类型 | `sizeof` | `alignof` | 说明 |
|---|---:|---:|---|
| `A` | 1 | 1 | 单个 `char` |
| `B` | 8 | 8 | 单个 `double` |
| `C` | 16 | 8 | `char` 后插入 7 字节 padding |

`struct C` 的典型内存布局：

```text
偏移:  0    1    2    3    4    5    6    7    8         15
     +----+----+----+----+----+----+----+----+----+......+----+
     | c  | <---------- padding ----------> |      d (8B)      |
     +----+----+----+----+----+----+----+----+----+......+----+
```

> [!tip]
> 结构体大小通常会补齐到其最大成员对齐要求的整数倍，这样结构体数组中的每个元素都能满足对齐。

---

# 5. `alignof(void*)`

## 5.1 查询的是指针对象

```cpp
alignof(void*) // 合法：查询 void* 类型对象的对齐要求
alignof(void)  // 非法：void 是不完整类型
```

`alignof(void*)` 查询的是指针变量本身存放时需要的对齐，而不是 `void` 的对齐。

```cpp
void* p; // p 这个指针对象也要被放在满足 alignof(void*) 的地址上
```

## 5.2 与 `sizeof(void*)`

| 平台 | `sizeof(void*)` | `alignof(void*)` |
|---|---:|---:|
| 64 位 x86-64 / ARM64 | 8 | 8 |
| 32 位 x86 / ARM32 | 4 | 4 |

主流平台上二者经常相等，但标准不要求它们永远相等。

---

# 6. 内存池中的对齐

## 6.1 FreeList 节点要求

内存池常把空闲块解释为链表节点：

```cpp
struct FreeNode
{
    FreeNode* next;
};
```

当某块内存处于空闲状态时，它的起始地址会被当作 `FreeNode*` 访问。若该地址不满足 `alignof(void*)`，访问 `next` 可能性能下降，甚至在部分平台触发硬件异常。

因此，内存池分配出的块至少要满足指针类型对齐要求：

```cpp
block_size = align_up(block_size, alignof(void*));
```

## 6.2 为什么不用 `sizeof(void*)`

```cpp
block_size = align_up(block_size, sizeof(void*)); // 常见但语义不准确
```

`sizeof(void*)` 表示指针占多少字节；`alignof(void*)` 表示指针对象需要按多少字节对齐。对齐场景应使用后者表达真实意图。

---

# 7. 自定义对齐 `alignas`

## 7.1 显式指定类型对齐

`alignas` 用于提高类型或对象的对齐要求。

```cpp
struct alignas(32) BigObject
{
    char data[32];
};

static_assert(alignof(BigObject) == 32);
```

若声明为：

```cpp
struct alignas(64) CacheLineData
{
    int value;
};
```

则对象起始地址至少按 64 字节对齐，`sizeof(CacheLineData)` 通常也会补齐到 64 的倍数，以保证数组元素也对齐。

## 7.2 常见用途

| 用途 | 说明 |
|---|---|
| SIMD | 满足特定向量指令加载要求 |
| Cache line 对齐 | 减少 false sharing |
| 分配器实现 | 保证返回内存满足类型需求 |
| 硬件/协议结构 | 匹配底层布局约束 |

---

# 8. 对齐计算

常见向上对齐公式：

```cpp
std::uintptr_t align_up(std::uintptr_t value, std::size_t alignment)
{
    return (value + alignment - 1) & ~(alignment - 1);
}
```

该公式要求 `alignment` 是 2 的幂。以 32 字节对齐为例：

```text
alignment - 1 = 31 = 0x1F
~0x1F 会清掉低 5 位

0x1008 + 0x1F = 0x1027
0x1027 & ~0x1F = 0x1020
```

因此 `0x1008` 向上对齐到 32 字节边界后得到 `0x1020`。

---

# 9. 易错点

> [!warning]
> 对齐问题表面是数字计算，实质是对象起始地址是否满足类型约束。

1. **把 `alignof` 当成设置工具**：`alignof` 只查询；设置对齐用 `alignas`。
2. **误写 `alignof(x)`**：`x` 是变量时应写 `alignof(decltype(x))`。
3. **混淆 `void` 与 `void*`**：`alignof(void)` 非法，`alignof(void*)` 合法。
4. **假设 `sizeof(T) == alignof(T)`**：数值相等不代表语义相同。
5. **忽略尾部 padding**：结构体大小会影响数组中下一个元素的对齐。
6. **硬编码对齐值**：优先使用 `alignof(T)` 或 `alignof(std::max_align_t)`。
7. **对非 2 的幂使用位运算对齐公式**：位运算公式只适用于 2 的幂对齐。

---

# 10. 总结

| 概念 | 含义 |
|---|---|
| `sizeof(T)` | 对象占用多少字节 |
| `alignof(T)` | 对象起始地址要求按多少字节对齐 |
| `alignas(N)` | 显式提高对象或类型的对齐要求 |
| padding | 编译器为满足对齐插入的填充字节 |
| `alignof(void*)` | 指针对象本身的对齐要求 |

> [!summary]
> 内存对齐是连接对象布局、CPU 访问规则和分配器实现的基础概念。写内存池、对象池或底层缓冲区时，必须用 `alignof` 表达对齐要求，用 `sizeof` 表达空间大小。
