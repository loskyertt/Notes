# 1. 拷贝次数的优化

## 1.1 问题引入：意外的拷贝

`std::vector` 是 C++ 中最常用的容器，其内存空间位于**堆**上。如果在不知道其底层机制的情况下使用，很容易产生大量的隐性性能消耗。

观察下面的代码，这是一个经典的示例，展示了 `std::vector` 在动态扩容过程中产生的对象拷贝。我们先定义一个带有追踪打印功能的类 `Vertex`：

```cpp
#include <iostream>
#include <vector>

struct Vertex {
    float m_x, m_y, m_z;

    // 构造函数
    Vertex(float x, float y, float z) : m_x(x), m_y(y), m_z(z) {
        // std::cout << "Constructed\n"; 
    }

    // 显式写出拷贝构造函数，查看调用情况
    Vertex(const Vertex& other) : m_x(other.m_x), m_y(other.m_y), m_z(other.m_z) {
        std::cout << "Copied!\n";
    }
};

int main() {
    std::vector<Vertex> vertices;
    vertices.push_back({ 1, 2, 3 });
    vertices.push_back({ 4, 5, 6 });
    vertices.push_back({ 7, 8, 9 });
}
```

**输出结果**：

```bash
Copied!
Copied!
Copied!
Copied!
Copied!
Copied!
```

仅仅是添加了 3 个元素，却触发了 6 次拷贝构造。这在处理复杂对象时是巨大的性能开销。要解决这个问题，必须深入理解 `vector` 的行为模式。

## 1.2 原因深度剖析

造成上述 6 次拷贝的原因主要有两点：**扩容迁移**和**临时对象构造**。

### 1.2.1 内存动态扩容机制

`std::vector` 的初始容量通常为 0。为了保证随机访问的速度，`vector` 必须在内存中维护一段连续的空间。当现有空间不足以容纳新元素时，它会执行以下步骤：

1.  申请一块更大的内存空间（通常是原来的 1.5 倍或 2 倍）。
2.  将旧内存中的所有元素**拷贝**或**移动**到新内存中。
3.  释放旧内存。

这个过程会导致原本已经存在的元素在内存中发生“搬家”。

### 1.2.2 执行过程推演

结合扩容机制，我们可以推演代码的执行细节。默认情况下，编译器如果没有定义移动构造函数或移动构造不可用，就会调用拷贝构造函数。

1.  **第一次 `push_back({1, 2, 3})`**：
    -   容量 0 -> 扩容至 1。
    -   `{1, 2, 3}` 是一个临时对象（右值），被拷贝到 vector 中。
    -   **拷贝计数：1**。当前 size=1, capacity=1。

2.  **第二次 `push_back({4, 5, 6})`**：
    -   容量不足（当前 1），触发扩容至 2。
    -   申请新内存。
    -   **拷贝计数：1**。旧元素 `{1, 2, 3}` 被拷贝到新内存。
    -   **拷贝计数：1**。新元素 `{4, 5, 6}` 被拷贝到新内存。
    -   **累计计数：3**。当前 size=2, capacity=2。

3.  **第三次 `push_back({7, 8, 9})`**：
    -   容量不足（当前 2），触发扩容至 4（假设按 2 倍增长）。
    -   申请新内存。
    -   **拷贝计数：2**。旧元素 `{1, 2, 3}` 和 `{4, 5, 6}` 被拷贝到新内存。
    -   **拷贝计数：1**。新元素 `{7, 8, 9}` 被拷贝到新内存。
    -   **累计计数：6**。

## 1.3 优化策略

### 1.3.1 方案一：内存预分配

> [!tip] 💡 既然扩容导致了旧数据的迁移，最直接的优化方式就是在添加元素前，提前告知 `vector` 需要多少空间。

`reserve()` 函数用于预留容量，但不改变元素的个数（`size` 保持不变）。这避免了中途的多次扩容。

```cpp
#include <iostream>
#include <vector>

struct Vertex {
    float m_x, m_y, m_z;

    // 构造函数
    Vertex(float x, float y, float z) : m_x(x), m_y(y), m_z(z) {
        // std::cout << "Constructed\n"; 
    }

    // 显式写出拷贝构造函数，查看调用情况
    Vertex(const Vertex& other) : m_x(other.m_x), m_y(other.m_y), m_z(other.m_z) {
        std::cout << "Copied!\n";
    }
};

int main() {
    std::vector<Vertex> vertices;
    vertices.reserve(3); // 关键：提前申请容纳 3 个元素的空间

    vertices.push_back({ 1, 2, 3 });
    vertices.push_back({ 4, 5, 6 });
    vertices.push_back({ 7, 8, 9 });
}
```

**优化效果**：
- 扩容迁移的拷贝被完全消除。
- 只剩下 3 次临时对象的拷贝（`push_back` 将临时对象拷贝进 vector）。

**输出**：

```bash
Copied!
Copied!
Copied!
```

### 1.3.2 方案二：原地构造

> [!tip] 💡 在预留空间后，我们仍然面临一个问题：`push_back({1, 2, 3})` 会先创建一个临时对象，再将其拷贝到 vector 中。能否直接在 vector 的内存中构造对象？

`emplace_back()` 是 C++11 引入的方法，它接受构造函数的参数，直接在容器的内存位置调用构造函数，省去了临时对象的创建和拷贝过程。

```cpp
std::vector<Vertex> vertices;
vertices.reserve(3);

// 直接传递构造参数，而不是对象
vertices.emplace_back(1.0f, 2.0f, 3.0f);
vertices.emplace_back(4.0f, 5.0f, 6.0f);
vertices.emplace_back(7.0f, 8.0f, 9.0f);
```

**优化效果**：
- 没有临时对象，没有拷贝构造。
- 结合 `reserve()`，没有任何扩容操作。
- **输出结果：无任何打印**。（除了可能的构造函数打印）。

### 1.3.3 方案三：移动语义与 `noexcept`

> [!tip] 💡 虽然 `emplace_back` 解决了新元素构造的问题，但如果由于客观原因必须扩容（例如**未预知元素数量**），旧元素依然需要迁移。

在扩容迁移时，`vector` 会检查元素的移动构造函数是否存在。如果存在，它会优先使用移动构造（将旧对象的资源“偷”给新对象），而不是深拷贝。移动构造的开销通常极小（对于简单类型可能只是指针拷贝）。

如果在 `Vertex` 中定义了移动构造函数：

```cpp
#include <iostream>
#include <vector>

struct Vertex {
    float m_x, m_y, m_z;

    // 构造函数
    Vertex(float x, float y, float z) : m_x(x), m_y(y), m_z(z) {
        // std::cout << "Constructed\n"; 
    }

    // 显式写出拷贝构造函数，查看调用情况
    Vertex(const Vertex& other) : m_x(other.m_x), m_y(other.m_y), m_z(other.m_z) {
        std::cout << "Copied!\n";
    }
    
    // 移动构造函数
	Vertex(Vertex&& other) : m_x(other.m_x), m_y(other.m_y), m_z(other.m_z) {
	    std::cout << "Moved!\n";
	}
};

int main() {
    std::vector<Vertex> vertices;
    vertices.push_back({ 1, 2, 3 });
    vertices.push_back({ 4, 5, 6 });
    vertices.push_back({ 7, 8, 9 });
}
```

此时扩容的开销将从 "Copied!" 变为 "Moved!"，效率显著提升。

**输出**：

```bash
Moved!
Moved!
Moved!
Moved!
Moved!
Moved!
```

> [!success] `noexcept` 关键字的重要性
> `vector` 对移动构造的安全性有极高要求。当 `std::vector` 需要重新分配存储时，如果元素的移动构造函数**不是 `noexcept`**，且拷贝构造函数可用，则**优先使用拷贝构造**进行元素迁移。

这是因为：如果在移动一半时抛出异常，原来的数据可能已经被破坏，无法恢复；而拷贝构造如果不抛异常，原数据依然完好。

因此，如果类支持移动语义且不抛异常，必须显式声明 `noexcept`：

```cpp
Vertex(Vertex&& other) noexcept 
    : m_x(other.m_x), m_y(other.m_y), m_z(other.m_z) {
    std::cout << "Moved!\n";
}
```

加上 `noexcept` 后，`vector` 在扩容时才会放心地调用移动构造函数。

---

## 1.4 总结

优化 `std::vector` 性能的三步走策略：

1.  **预分配**：只要能预估数量，就使用 `reserve(n)`。
2.  **原地构造**：添加元素时，优先使用 `emplace_back(args...)`。
3.  **安全移动**：定义自定义类型的移动构造函数时，务必加上 `noexcept`。

---

# 2. `reserve` 与 `resize` 的区别

> [!note] 在 C++ 的 `std::vector` 中，`resize` 和 `reserve` 经常被混淆，因为它们都和内存空间有关。但它们的本质区别在于：**一个是改变“元素的个数”，一个是改变“内存的容量”**。

为了彻底理解它们，我们需要区分两个核心概念：
1. **Size（大小）**：容器中当前**实际存在**的元素个数。
2. **Capacity（容量）**：容器在不重新分配内存的情况下，**最多能容纳**的元素个数。

**核心区别对比**：

| **特性**             | **resize(n)**             | **reserve(n)**           |
| ------------------ | ------------------------- | ------------------------ |
| **对 Size 的影响**     | **改变**。变为 $n$。            | **不改变**。依然是原大小。          |
| **对 Capacity 的影响** | 可能改变。若 $n >$ 原容量，则扩容。     | **改变**。确保容量至少为 $n$。      |
| **是否创建对象**         | **是**。若增加大小，会调用构造函数创建新元素。 | **否**。只预留原始内存，不构造对象。     |
| **访问权限**           | 可以通过 `[]` 访问新增的 $n$ 个元素。  | 不可以直接访问（超出 Size 的访问是越界）。 |

> 比如：使用 `resize(3)`，`vector` 会立即调用 `Vertex()` 默认构造函数创建 3 个对象。如果 `Vertex` 没有默认构造函数，代码将报错。即便有默认构造函数，后续的 `emplace_back` 会在第 4 个位置追加元素，逻辑完全错误。


假设我们有一个空的 `vector<int> v;`。

- **使用 `resize` 的情况**：

```cpp
std::vector<int> v;
v.resize(5); // 将大小改为 5

std::cout << v.size();     // 输出 5
std::cout << v.capacity(); // 输出 5 (或更大)

// 此时 v 里面已经有了 5 个 0（默认初始化）
v[0] = 10; // 合法
```

> - **动作**：编译器申请了内存，并在这块内存上**构造了 5 个整数对象**（默认为 0）。
> - **用途**：当你明确需要 $n$ 个元素并打算直接操作它们时使用。

- **使用 `reserve` 的情况**：

```cpp
std::vector<int> v;
v.reserve(5); // 预留 5 个元素的空间

std::cout << v.size();     // 输出 0
std::cout << v.capacity(); // 输出 5

// v[0] = 10; // ❌ 错误！虽然有空间，但还没有元素对象，这是未定义行为
v.push_back(10); // ✅ 合法，此时 size 变为 1
```

> - **动作**：编译器仅仅是**预留了一块足够放 5 个整数的空地**，但地皮上什么也没盖（没有构造对象）。
> - **用途**：当你预知要往里面放很多数据（比如循环 `push_back`），为了**避免频繁的重新分配内存和拷贝**，先打好招呼。

> [!success] 总结建议
> - **用 `reserve`**：当你担心频繁 `push_back` 导致性能下降，想提前优化内存分配时。
> - **用 `resize`**：当你需要一个固定长度的数组来直接赋值（比如做缓冲区，或者通过下标遍历）时。

---

# 3. `push_back` 与 `emplace_back` 的区别

- **`push_back(T obj)`**：
    - 接收一个已存在的对象。
    - 流程：外部构造临时对象 -> 拷贝/移动到 vector 内部 -> 销毁临时对象。
- **`emplace_back(Args...)`**：
    - 接收构造函数的参数列表。
    - 流程：它直接在 `vector` 管理的内存空间里调用构造函数的底层指令（`placement new`），在目标位置直接把对象生成出来。

假设我们有一个复杂的类 `Item`，在构造和拷贝时会打印信息：

```cpp
#include <iostream>
#include <vector>
#include <string>

struct Item {
  std::string name;

  Item(std::string s) : name(s) { std::cout << "构造\n"; }

  Item(const Item &other) { std::cout << "拷贝\n"; }

  Item(Item &&other) noexcept { std::cout << "移动\n"; }
};

int main() {
  std::vector<Item> vec;
  vec.reserve(10);  // 预留空间，避免扩容干扰观察

  std::cout << "--- 执行 push_back ---\n";
  vec.push_back(Item("Apple"));

  std::cout << "\n--- 执行 emplace_back ---\n";
  vec.emplace_back("Orange");
}
```

输出：

```bash
--- 执行 push_back ---
构造
移动

--- 执行 emplace_back ---
构造
```

- **执行 `push_back` 时**：
	1. **构造**：`Item("Apple")` 创建了一个临时对象。
	2. **移动**（或拷贝）：`push_back` 把临时对象搬进 `vector` 的内存。
	3. **析构**（图中未标出）：函数行结束，临时对象被销毁。

> 注：现代编译器可能会通过优化减少这一次移动，但在语义上，这确实是两个步骤。

- **执行 `emplace_back` 时**：
	1. **构造**：编译器直接在 `vector` 的内存地址上运行 `Item("Orange")`。

> 没有任何中间商，没有临时对象，也没有搬运过程。

- **对比**：

| 特性 | `push_back` | `emplace_back` |
| :--- | :--- | :--- |
| **参数要求** | 必须是对象类型 | 构造函数的参数列表 |
| **性能** | 可能会有额外的移动/拷贝开销 | 性能最高，直接就地构造 |
| **隐式转换** | **不支持** `explicit` 构造函数 | **支持** `explicit` 构造函数 |
| **可读性** | 明确添加一个对象 | 看起来更像是在传递参数 |

> [!warning] 关于 `explicit` 的一个小陷阱
> 如果构造函数被标记为 `explicit`，`push_back` 会报错，因为它的参数需要一次隐式转换：
> 
> ```cpp
> struct Foo {
>     explicit Foo(int x) {}
> };
> 
> std::vector<Foo> v;
> // v.push_back(10);     // ❌ 编译报错，不能隐式转为 Foo
> v.push_back(Foo(10));   // ✅ 显式创建对象可以
> v.emplace_back(10);     // ✅ 正常运行，它是直接调用构造函数
> ```

---

# 4. `std::vector::at()` 与 `std::vector::operator[]` 的区别

> 在 C++ 中，`at()` 和 `operator[]` 都能让你访问 `vector` 中的元素，但它们在**安全策略**和**运行性能**上有着本质的区别。

|**特性**|**at(index)**|**operator[index]**|
|---|---|---|
|**边界检查**|**强制检查**。如果索引越界，抛出异常。|**不检查**（通常）。如果越界，结果不可预知。|
|**越界后果**|抛出 `std::out_of_range` 异常。|**未定义行为 (Undefined Behavior)**。|
|**运行性能**|略慢。每次访问都要进行额外的范围判断。|**极快**。直接计算内存偏移量，无开销。|
|**推荐场景**|处理来自用户输入或不确定来源的索引。|在循环中访问已知合法的索引，追求极致性能。|

假设我们有一个大小为 3 的 `vector`：`std::vector<int> v{10, 20, 30};`

- **使用 `v.at(5)`**：

```cpp
try {
    int value = v.at(5); // 试图访问不存在的第 6 个元素
} catch (const std::out_of_range& e) {
    std::cerr << "捕获到异常: " << e.what() << "\n";
    // 程序可以优雅地处理错误，不会崩溃
}
```

> **结果**：程序安全运行，你可以通过 `try-catch` 捕获这个错误。

- **使用 `v[5]`**：

```cpp
int value = v[5]; // 危险！
```

> **结果**：编译器不会报错。在运行时，它会直接去访问那块不属于你的内存。运气好的话，程序崩溃（Segmentation Fault）；运气差的话，程序**不崩溃**，但读到了一个垃圾值，或者修改了其他变量的数据，产生极其难排查的 Bug。

---

# 5. `clear()` 详解

> `std::vector::clear()` 的作用非常明确：**移除容器中的所有元素**。

当你调用 `v.clear()` 时，发生了两件事：

1. **析构元素**：销毁 `vector` 中的每一个对象。如果 `vector` 存储的是类对象，会逐个调用它们的析构函数。
2. **重置大小**：将 `vector` 的 `size` 设置为 **0**。

> [!note] 关键点：它不会释放内存 (Capacity)
> **`clear()` 只清空内容，不归还内存**。即 Size 变为 0，Capacity（容量）保持不变。

这样设计的目的是为了性能。如果你清空了一个 `vector` 紧接着又要往里存新数据，保留之前的内存空间可以避免重新申请内存的昂贵开销。

代码示例：

```cpp
#include <iostream>
#include <vector>

int main() {
    std::vector<int> v = {1, 2, 3, 4, 5};
    
    std::cout << "清空前: size = " << v.size() << ", capacity = " << v.capacity() << "\n";
    
    v.clear();
    
    std::cout << "清空后: size = " << v.size() << ", capacity = " << v.capacity() << "\n";
    
    // 证明内存还在：我们可以直接 push_back 而不需要重新分配内存
    v.push_back(10);
    std::cout << "再添加后: size = " << v.size() << ", capacity = " << v.capacity() << "\n";
    
    return 0;
}
```

**输出结果通常是**：
- 清空前: size = 5, capacity = 5
- 清空后: size = 0, capacity = 5 <-- **看，容量没变！**
- 再添加后: size = 1, capacity = 5

> [!tip] 💡 什么时候该用 `clear()`？
> - **复用容器**：比如在循环中，每一轮都要处理一批新的数据，先 `clear()` 再填充，可以极大提高性能。
> - **重置状态**：当你不再需要当前的数据，但预计稍后还会用到这个容器时。 

如果你确定这个 `vector` 之后再也不会用到大容量，或者它占用了太多内存，可以使用 `shrink_to_fit()`：

```cpp
v.clear();
v.shrink_to_fit(); // 请求释放没用的 capacity（更推荐）
```

或者**彻底销毁法（Swap 技巧）**：

```cpp
std::vector<int>().swap(v); // 用一个全新的空容器交换，彻底清空 v 的内存（了解即可）
```

---

# 6. `erase()` 详解

`std::vector::erase()` 允许你删除容器中指定位置的一个元素，或者一个范围内的多个元素。

## 6.1 基本用法

- **用法一：删除单个元素**

```cpp
// 语法：v.erase(迭代器位置)
auto it = v.begin() + 2; 
v.erase(it); // 删除索引为 2 的元素（第 3 个）
```

- **用法二：删除一个范围**

```cpp
// 语法：v.erase(起始迭代器, 结束迭代器)
// 注意：区间是 [first, last)，即包含 first，不包含 last
v.erase(v.begin() + 1, v.begin() + 4); // 删除索引 1, 2, 3 的元素
```

- **用法三：Erase-remove 惯用法**

如果你想删除 `vector` 中所有等于某个值的元素，循环调用 `erase` 效率极低。在 C++20 之前，大家用 `std::remove` 配合 `erase`：

```cpp
// 传统写法：将要删的移到末尾，然后一次性切掉
v.erase(std::remove(v.begin(), v.end(), 5), v.end());

// C++20 中更简单的写法
std::erase(v, 5);
```

> 还可以把 `std::remove(...)` 换成 `std::remove_if(...)` 并结合 lambda 表达式来删除满足特定条件的元素。

## 6.2 原理

`erase()` 的特性：**为了保持 `vector` 内存的连续性，删除元素后，其后的所有元素都必须向前移动**。

1. **析构**：销毁被删除位置的对象。
2. **移动**：将被删除元素之后的所有元素，一个接一个地向前移动一个位置（调用移动赋值或拷贝赋值）。
3. **更新 Size**：`size` 减小，但 **`capacity`（容量）依然不变**。

时间复杂度为 $O(n)$
- **删除末尾元素**：非常快（类似于 `pop_back()`），因为不需要移动任何元素。
- **删除中间或头部元素**：非常慢。如果你删除第一个元素，后面所有的元素都要往前挪。如果 `vector` 有 100 万个元素，删除第一个就会触发 999,999 次移动操作。

> [!debug] 一个非常危险的 Bug：迭代器失效

当你 `erase` 一个元素后，指向该位置及之后位置的所有迭代器都会**失效**。错误写法如下：

```cpp
for (auto it = v.begin(); it != v.end(); ++it) {
    if (*it == 5) {
        v.erase(it); // ❌ 错误！执行后 it 失效，下一次循环 ++it 会崩溃
    }
}
```

**正确的写法**：`erase()` 会返回一个**指向被删除元素下一个位置**的有效迭代器。

```cpp
for (auto it = v.begin(); it != v.end(); ) {
    if (*it == 5) {
        it = v.erase(it); // ✅ 更新迭代器，指向下一个元素
    } else {
        ++it;
    }
}
```

---

# 7. `swap()` 详解

`std::vector` 的 `swap` 是一个**常数时间（$O(1)$）的容器交换操作**，它会**直接交换两个 vector 的内部资源**，而不是逐个移动或拷贝元素。

对两个 `vector`：

```cpp
std::vector<int> a{1,2,3};
std::vector<int> b{4,5};
```

执行：

```cpp
a.swap(b);
// 或 std::swap(a, b);
```

> [!note] swap 有下面两种形式
> 
> 1. 成员函数
> 
> ```cpp
> v1.swap(v2);
> ```
> 
> 2. 泛型版本（推荐）
> 
> ```cpp
> std::swap(v1, v2);
> ```
> 
> 对于 `vector`，标准库对 `std::swap` 做了特化，最终还是调用 `vector::swap`。

结果：

```cpp
a = {4,5}
b = {1,2,3}
```

关键点：**没有发生元素级别的拷贝或移动**。

而是交换了内部三元组（典型实现）：
- `data` 指针（指向堆内存）
- `size`
- `capacity`

因此时间复杂度为 $O(1)$，既不依赖元素数量，也不调用元素的拷贝 / 移动构造函数。

这点和下面操作完全不同：

```cpp
a = b;              // O(n) 拷贝
a = std::move(b);   // O(n)（通常是逐元素移动）
```

`swap()` 还可以用用来**释放内存**：

```cpp
std::vector<int> v;
// ... v 很大

std::vector<int>().swap(v);
```

执行后会清空 `v`，并且**释放 capacity（真正归还内存）**。而 `v.clear(); ` 是只清空元素，不释放 capacity。

> [!question] 迭代器 / 引用会怎样？
>这是很多人忽略的点：**迭代器不会失效，但“归属的容器变了”**
> 
> ```cpp
> auto it = a.begin();
> a.swap(b);
> ```
> 
> 执行 swap() 后，`it` 仍然有效，但它指向的是**原来 a 的数据，现在属于 b**。

