---
title: C++ const 成员函数
date: 2026-06-30
tags:
  - Cpp
  - const
  - OOP
  - const-correctness
aliases:
  - const 成员函数
  - const correctness
  - 常量成员函数
---

# 1. 核心概念

`const` 成员函数是在成员函数参数列表后添加 `const` 限定符的函数。它承诺不修改调用对象的逻辑状态。

```cpp
class Player {
public:
    int health() const {
        return m_health;
    }

    void setHealth(int value) {
        m_health = value;
    }

private:
    int m_health = 100;
};
```

> [!summary]
> `const` 成员函数的意义不是“这个函数只能被 const 对象调用”，而是“这个函数也能被 const 对象调用，并承诺不修改对象状态”。

---

# 2. this 指针的变化

成员函数中的 `const` 本质上改变了 `this` 指针指向对象的类型。

| 成员函数 | `this` 类型 | 能否修改普通成员 |
|---|---|---|
| 非 `const` 成员函数 | `T* const` | 可以 |
| `const` 成员函数 | `const T* const` | 不可以 |

```cpp
class Counter {
public:
    int value() const {
        // m_value = 1;  // 错误：const 成员函数不能修改普通成员
        return m_value;
    }

private:
    int m_value = 0;
};
```

`this` 指针本身总是不能改指向；`const` 成员函数额外限制了 `this` 指向的对象不能被当作可修改对象。

---

# 3. 调用权限

## 3.1 对象类型决定可调用函数

| 对象形式 | 可调用 `const` 函数 | 可调用非 `const` 函数 |
|---|---:|---:|
| `T obj;` | 是 | 是 |
| `const T obj;` | 是 | 否 |
| `T& ref` | 是 | 是 |
| `const T& ref` | 是 | 否 |
| `T* ptr` | 是 | 是 |
| `const T* ptr` | 是 | 否 |

```cpp
void print(const Player& player) {
    std::cout << player.health() << "\n";  // 要求 health() 是 const 成员函数
}
```

如果 `health()` 漏掉 `const`，`print` 就无法通过 `const Player&` 调用它。

## 3.2 const 成员函数的内部限制

`const` 成员函数内部只能调用其他 `const` 成员函数，除非调用的是静态成员函数或外部函数。

```cpp
class Player {
public:
    bool isAlive() const {
        return health() > 0;  // 正确：调用 const 成员函数
    }

    int health() const { return m_health; }

    void damage(int value) { m_health -= value; }

private:
    int m_health = 100;
};
```

---

# 4. 逻辑常量性与 mutable

## 4.1 逻辑常量性

有些操作物理上会修改内部缓存或统计字段，但从对象外部观察，它们仍然是只读操作。这称为**逻辑常量性**。

```cpp
class QueryCache {
public:
    int get(int key) const {
        ++m_queryCount;
        return lookup(key);
    }

private:
    int lookup(int key) const;

    mutable int m_queryCount = 0;
};
```

`mutable` 允许成员在 `const` 成员函数中被修改，适用于缓存、统计、调试标记和互斥锁等不改变对象逻辑状态的内部实现细节。

## 4.2 mutable 的常见用途

| 场景 | 示例 |
|---|---|
| 懒加载缓存 | `mutable std::optional<Value> cache;` |
| 访问统计 | `mutable int accessCount;` |
| 线程同步 | `mutable std::mutex mutex;` |
| 调试信息 | `mutable bool logged;` |

```cpp
class Profile {
public:
    std::string name() const {
        std::lock_guard<std::mutex> lock(m_mutex);
        if (!m_cachedName.has_value()) {
            m_cachedName = loadName();
        }
        return *m_cachedName;
    }

private:
    std::string loadName() const;

    mutable std::mutex m_mutex;
    mutable std::optional<std::string> m_cachedName;
};
```

> [!warning]
> `mutable` 不是绕过设计约束的通行证。它只应修饰不改变对象逻辑状态的内部字段。

---

# 5. const 重载

同名成员函数可以同时提供 `const` 和非 `const` 版本。编译器会根据调用对象的 const 属性选择重载。

```cpp
class Buffer {
public:
    char& at(std::size_t index) {
        return m_data[index];
    }

    const char& at(std::size_t index) const {
        return m_data[index];
    }

private:
    std::vector<char> m_data;
};
```

```cpp
Buffer buffer;
const Buffer constBuffer;

buffer.at(0) = 'A';       // 调用非 const 版本，返回 char&
// constBuffer.at(0) = 'B'; // 错误：调用 const 版本，返回 const char&
```

这种设计常见于容器、字符串和访问器接口。

---

# 6. const_cast 的风险

`const_cast` 可以移除 `const` 限定，但只有在原始对象本身不是 `const` 时，修改行为才可能是安全的。

```cpp
void unsafe(const Player& player) {
    auto& mutablePlayer = const_cast<Player&>(player);
    mutablePlayer.setHealth(0);  // 如果 player 原本是 const 对象，则行为未定义
}
```

较合理的使用场景通常是适配遗留 API，并且能够证明被调用函数不会破坏逻辑常量性。

```cpp
class LegacyAdapter {
public:
    int query() const {
        return const_cast<LegacyAdapter*>(this)->legacyQuery();
    }

private:
    int legacyQuery();  // 遗留接口：实际只读，但签名缺少 const
};
```

> [!warning]
> 如果能通过补全 `const` 接口、拆分只读函数或使用 `mutable` 解决问题，就不要使用 `const_cast`。

---

# 7. 实践准则

1. 所有不修改对象逻辑状态的成员函数都应声明为 `const`。
2. getter、`size()`、`empty()`、`contains()`、`find()` 等查询函数通常都应是 `const`。
3. 返回引用或指针时，`const` 成员函数应返回 `const T&` 或 `const T*`，避免通过返回值修改对象内部状态。
4. `mutable` 只用于缓存、统计、锁等逻辑只读字段。
5. `const_cast` 只作为遗留接口适配手段，不作为常规设计工具。

> [!summary]
> `const` 正确性会直接影响接口通用性。一个只读函数漏写 `const`，会让它无法用于 `const` 引用、`const` 指针和大量泛型场景。
