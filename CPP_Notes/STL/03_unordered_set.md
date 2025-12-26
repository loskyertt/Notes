# 1.unordered_set

`unordered_set` 是 C++ 标准库里基于 **哈希表** 实现的 **集合**（只存“键”，不存“值”），具有 **平均 O(1)** 的插入、删除、查找复杂度。它与 `set` 最大的区别是：

- `set`：底层红黑树，元素有序；  
- `unordered_set`：底层哈希表，元素 **无序** 存储，速度通常更快。

`unordered_set` 与 `set` 对比速览：

| 特性              | `unordered_set` | `set`        |
|-------------------|-----------------|--------------|
| 底层结构          | 哈希表          | 红黑树       |
| 元素顺序          | 无序            | 升序         |
| 插入/查找/删除    | 平均 O(1)       | O(log n)     |
| 额外需求          | 哈希函数        | 只需 `<`     |
| 迭代器稳定性      | 重新哈希时失效  | 稳定         |

---

# 2.基本语法

1. 头文件与声明：

```cpp
#include <unordered_set>
```

2. 基本用法速查表

| 操作                | 代码示例                                | 说明                     |
|---------------------|------------------------------------------|--------------------------|
| 创建                | `std::unordered_set<int> s;`             | 空集合                   |
| 插入                | `s.insert(42);`                          | 已存在则不变             |
| 查找                | `if (s.count(42)) …`                     | 存在返回 1，否则 0       |
| 删除                | `s.erase(42);`                           | 按键删除                 |
| 遍历                | `for (int x : s) std::cout << x;`        | 无序                     |
| 清空                | `s.clear();`                             |                          |
| 大小 / 是否空       | `s.size();` / `s.empty();`               |                          |

## 2.1 用法

### 2.1.1 基本操作

```cpp
std::unordered_set<int> s = {1, 2, 3, 4, 5};

// 插入元素
s.insert(6);
s.insert(3);  // 不会插入，因为 3 已存在

// 查找元素，find 返回迭代器
if (s.find(3) != s.end()) {
	std::cout << "找到元素 3" << std::endl;
}

// 删除元素
s.erase(2);

// 检查元素是否存在，只返回 0/1
if (s.count(4) > 0) {
	std::cout << "元素 4 存在" << std::endl;
}

// 获取大小
std::cout << "集合大小: " << s.size() << std::endl;

// 清空集合
s.clear();
```

### 2.1.2 遍历 unordered_set

```cpp
std::unordered_set<int> s = {10, 20, 30, 40, 50};

// 方法1：使用迭代器
std::cout << "使用迭代器遍历: ";
for (auto it = s.begin(); it != s.end(); ++it) {
	std::cout << *it << " ";
}
std::cout << std::endl;

// 方法2：使用范围 for 循环（推荐）
std::cout << "使用范围for循环: ";
for (int num : s) {
	std::cout << num << " ";
}
std::cout << std::endl;
```

两者生成的代码几乎一样，都是 O(n) 线性扫描，没有差别。

### 2.1.3 把元素放入 `unordered_set` 中。

方法1：使用范围构造函数（最简洁）

```cpp
std::vector<int> A = {1, 2, 3, 2, 4, 5, 1, 6};
    
// 直接将 vector 转换为 unordered_set（自动去重）
std::unordered_set<int> mySet(A.begin(), A.end());
```

方法2：使用 insert 方法（最啰嗦）

```cpp
std::vector<int> A = {1, 2, 3, 2, 4, 5};
std::unordered_set<int> mySet;
    
// 使用 insert 方法逐个插入
for (int num : A) {
	mySet.insert(num);  // 重复元素会被自动忽略
}
```

一般需要在插入时做额外逻辑（例如过滤条件）才用这种方法的显式循环。

方法3：使用 insert 的范围版本（次之）

```cpp
std::vector<int> A = {1, 2, 3, 4, 5};
std::unordered_set<int> mySet;
    
// 一次性插入整个 vector 的范围
mySet.insert(A.begin(), A.end());
```

### 2.1.4 自定义类型做键

待补充......

