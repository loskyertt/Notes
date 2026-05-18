# 1. 核心容器

## 1.1 `std::vector`（动态数组）

动态数组是竞赛中最常用的容器，支持随机访问，在尾部插入元素均摊时间为 $O(1)$。

### 1.1.1 常用技巧

- **预分配空间**：使用 `v.reserve(n)` 提前分配内存，避免频繁扩容导致的性能损耗。
- **快速清空**：`v.clear()` 将 `size` 置 0，但通常不释放内存。若需强制释放内存，可使用“交换技法”：
    ```cpp
    vector<int>().swap(v); // 此时 v 的 capacity 也变为 0
    ```
- **去重三部曲**：先排序，再利用 `unique` 将重复元素移至末尾，最后删除。
    ```cpp
    sort(v.begin(), v.end());
    v.erase(unique(v.begin(), v.end()), v.end());
    ```

### 1.1.2 结构体排序

当容器中存储的是自定义结构体时，需要指定排序规则。

假设有如下结构体：

```cpp
struct Info {
  int id;
  int score;
};

std::vector<Info> nums = {{1, 90}, {2, 85}, {3, 90}, {4, 70}};
```

**需求**：优先按 `score` 降序排列；若 `score` 相同，则按 `id` 升序排列。

#### 方式一：Lambda 表达式（推荐）

使用 Lambda 表达式作为 `std::sort` 的第三个参数，逻辑清晰且无需修改结构体定义。

> [!tip] 核心口诀
> **Lambda 返回 `true` 表示第一个参数应该排在第二个参数前面。**
> - 升序（小在前）：使用 `<` 号。
> - 降序（大在前）：使用 `>` 号。

**实现代码**：

```cpp
// 降序：分数高的在前；分数相同，id 小的在前
std::sort(nums.begin(), nums.end(), [](const Info& a, const Info& b) {
    if (a.score != b.score)
        return a.score > b.score; // 分数不同：大于号表示大的在前（降序）
    return a.id < b.id;           // 分数相同：小于号表示小的在前（升序）
});
```

**关于参数传递方式的选择**：

| 传参方式 | 发生了什么 | 适用场景 |
| :--- | :--- | :--- |
| `Info a` | 拷贝构造，产生临时对象 | 对象极小（如 `int`），拷贝开销可忽略 |
| `const Info& a` | 传递引用，无拷贝，禁止修改 | **结构体或大对象，推荐使用** |
| `Info& a` | 传递引用，无拷贝，允许修改 | 需要在比较函数中修改元素（极罕见） |

对于基本类型（`int`, `double`），直接传值效率更高：

```cpp
std::sort(v.begin(), v.end(), [](int a, int b) {
    return a > b; // 纯降序
});
```

#### 方式二：重载运算符

若该结构体在全局范围内都有固定的排序需求，可重载 `operator<`。

```cpp
struct Info {
    int id;
    int score;

    // 重载 < 运算符，定义结构体的“自然顺序”
    bool operator<(const Info& other) const {
        if (score != other.score)
            return score > other.score; // 此处定义为降序
        return id < other.id;
    }
};

// 排序时直接调用，无需传参
std::sort(nums.begin(), nums.end());
```

> [!warning] 注意事项
> 使用 `std::greater<T>()` 配合重载运算符时，需要同时重载 `operator>`。若只需降序输出，使用反向迭代器 `std::sort(nums.rbegin(), nums.rend())` 更为便捷，但需注意这会反转所有比较逻辑。

### 1.1.3 排序逻辑深度解析

`std::sort` 内部基于快速排序、堆排序和插入排序的混合实现。理解其比较逻辑对于编写复杂排序至关重要。

**比较函数的本质**：

比较函数回答的问题始终是：**“a 是否应该排在 b 的前面？”**
- 返回 `true`：`a` 排在 `b` 前面。
- 返回 `false`：`b` 排在 `a` 前面（或两者等价）。

**多级排序的构建方法**：

利用 `if` 语句的短路特性，由高优先级到低优先级依次判断。

**构造模板**：
1. **第 1 步**：用自然语言描述排序规则（先按 A，再按 B）。
2. **第 2 步**：将“A 不同”作为第一层 `if`，返回 A 的比较结果。
3. **第 3 步**：在 `else` 或后续语句中处理 B 的比较。

**实例扩展**：假设按 **班级升序 $\to$ 分数降序 $\to$ ID 升序**。

```cpp
[](const Info& a, const Info& b) {
    if (a.classId != b.classId)
        return a.classId < b.classId; // 班级升序

    if (a.score != b.score)
        return a.score > b.score;     // 分数降序

    return a.id < b.id;               // ID 升序
}
```

## 1.2 `std::set` / `std::map`（关联容器）

基于红黑树实现，元素自动有序，插入、删除、查找的时间复杂度均为 $O(\log n)$。

### 1.2.1 有序性利用

利用其有序性，可高效查询前驱和后继：

```cpp
auto it = s.lower_bound(x); // 第一个 >= x 的位置
if (it != s.begin()) {
    int prev = *--it;       // x 的前驱（小于 x 的最大值）
}
```

### 1.2.2 二分查找的性能陷阱

> [!warning] 关联容器的二分查找
> 在 `std::set` 或 `std::map` 中查找元素时：
> - **推荐**：使用成员函数 `s.lower_bound(x)`，时间复杂度为 $O(\log n)$。
> - **禁止**：使用全局函数 `std::lower_bound(s.begin(), s.end(), x)`，因为关联容器不支持随机访问，该操作会退化为 $O(n)$。

## 1.3 `std::priority_queue`（优先队列）

基于堆实现，默认为大根堆，适用于动态维护极值。

### 1.3.1 基本用法

- **大根堆**（默认）：`priority_queue<int> pq;`，堆顶元素最大。
- **小根堆**：`priority_queue<int, vector<int>, greater<int>> pq;`，堆顶元素最小。

### 1.3.2 Dijkstra 算法应用

在图论最短路算法中，常存储 `pair<int, int>`（距离与节点编号）。`pair` 的默认比较规则是**先比较第一维，第一维相同时比较第二维**，正好符合 Dijkstra 贪心选择距离最小节点的需求。

```cpp
using PII = pair<int, int>;
priority_queue<PII, vector<PII>, greater<PII>> pq; // 小根堆
pq.push({0, start}); // {distance, node_id}
```

## 1.4 `std::pair` / `std::tuple`

用于捆绑多个数据。

### 1.4.1 比较规则

`pair` 和 `tuple` 默认按字典序比较，先比第一元素，再比第二元素，依此类推。这使得它们可以直接作为 `map` 的键或用于排序。

### 1.4.2 便捷赋值

- C++11 `tie` 解包：
    ```cpp
    pair<int, int> p = {1, 2};
    int x, y;
    tie(x, y) = p; // x=1, y=2
    ```
- C++17 结构化绑定（推荐）：
    ```cpp
    auto [x, y] = p; // x=1, y=2
    ```

---

# 2. 高效算法

头文件 `<algorithm>` 提供了大量常用算法。

## 2.1 二分搜索系列

在**有序**序列中进行查找，时间复杂度 $O(\log n)$。

- `lower_bound(begin, end, x)`：返回第一个 **大于等于** $x$ 的元素的迭代器。
- `upper_bound(begin, end, x)`：返回第一个 **大于** $x$ 的元素的迭代器。
- `binary_search(begin, end, x)`：检查序列中是否存在 $x$，返回 `bool`。

> [!tip] 实际应用
> 若要在有序数组中查找 $x$ 并获取其下标：
> ```cpp
> int pos = lower_bound(a, a + n, x) - a;
> ```

## 2.2 排列与选择

- `next_permutation(begin, end)`：将序列变为字典序的下一个排列，常用于全排列暴力搜索。
    ```cpp
    sort(a, a + n);
    do {
        // 处理当前排列
    } while (next_permutation(a, a + n));
    ```
- `nth_element(begin, begin + k, end)`：在 $O(n)$ 时间内将第 $k$ 小的元素放到下标 $k$ 的位置，左边都比它小，右边都比它大（不保证有序）。

---

# 3. 特殊工具类

## 3.1 `pb_ds` (Policy-Based Data Structures)

GNU C++ 扩展库，提供了标准库未包含的高级数据结构。

### 3.1.1 主要用途

- **平衡树**：支持 $O(\log n)$ 查询第 $k$ 大元素、排名等，常作为 `std::set` 的增强版。
- **哈希表**：`gp_hash_table` 通常比 `std::unordered_map` 效率更高且不容易被卡常数。

> [!info] 头文件
> 使用时需包含 `<ext/pb_ds/assoc_container.hpp>` 和 `<ext/pb_ds/tree_policy.hpp>`。

## 3.2 `std::bitset`

用于处理二进制位集合，空间占用极小（通常为 $1/8$ 字节每 bit），且支持位运算。

### 3.2.1 状态压缩与集合操作

- **初始化**：`bitset<1000> bs;` 定义大小为 1000 的位集。
- **运算**：支持 `&`、`|`、`^`、`~` 等位运算，可用于模拟集合的交并补操作。

### 3.2.2 高效遍历

利用内置函数快速找到为 1 的位，常用于图论中遍历邻接点。

- `_Find_first()`：返回第一个置位（为 1）的下标。
- `_Find_next(pos)`：返回 `pos` 之后的下一个置位下标。

```cpp
bitset<100> bs;
// ... 设置某些位为 1
for (int i = bs._Find_first(); i < bs.size(); i = bs._Find_next(i)) {
    // i 为当前为 1 的位置
}
```