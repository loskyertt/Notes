# 1.priority_queue

`priority_queue`（优先队列）可以简单理解成“每次取队头时，自动把**最大（或最小）**的元素弹出来”的容器。它底层用**堆（heap）**实现，插入和弹出都是 **O(log n)**。

---

# 2.基本语法

1. 头文件与声明：

```cpp
#include <queue>          // 必备
using std::priority_queue;
```

2. 模板参数说明

```cpp
// 完整模板声明：
// priority_queue<T, Container, Compare>
//
// T: 元素类型 (int)
// Container: 底层容器 (vector<int>)
// Compare: 比较函数 (greater<int>)

// 比如：
priority_queue<int, vector<int>, greater<int>> min_pq;
```

3. 核心成员函数：

| 函数        | 含义                          |
|-------------|-------------------------------|
| `push(x)`   | 把元素压进队列                |
| `pop()`     | 弹出顶部元素（不返回值）      |
| `top()`     | 读取顶部元素（不弹出）        |
| `empty()`   | 是否为空                      |
| `size()`    | 元素个数                      |

| 需求    | 代码示例（最小/最大堆）                                         | 备注              |
| ----- | ---------------------------------------------------- | --------------- |
| 默认最大堆 | `priority_queue<int> pq;`                            | 默认大顶堆，`top()` 返回最大值 |
| 最小堆   | `priority_queue<int, vector<int>, greater<int>> pq;` | 小顶堆，`top()` 返回最小值 |
| 自定义排序 | `priority_queue<Node, vector<Node>, MyCmp> pq;`      | 自己写比较器/仿函数      |

---

# 3.用法

## 3.1 基本操作

```cpp
// 默认：大顶堆（最大值在顶部）
priority_queue<int> pq;

// 插入元素
pq.push(30);
pq.push(10);
pq.push(50);
pq.push(20);
pq.push(40);

// 访问顶部元素（不删除）
cout << "顶部元素: " << pq.top() << endl;  // 输出: 50

// 弹出顶部元素
pq.pop();
cout << "弹出后顶部: " << pq.top() << endl;  // 输出: 40

// 获取大小
cout << "队列大小: " << pq.size() << endl;

// 检查是否为空
if (!pq.empty()) {
	cout << "队列不为空" << endl;
}
```

## 3.2 小顶堆

```cpp
// 小顶堆：最小值在顶部
priority_queue<int, vector<int>, greater<int>> min_pq;

min_pq.push(30);
min_pq.push(10);
min_pq.push(50);

cout << "最小值: " << min_pq.top() << endl;  // 输出: 10
```

> **注意**：`greater<>` 是小顶堆，`less<>` （或默认）是大顶堆。

## 3.3 自定义比较函数

```cpp
// 自定义比较函数：按绝对值大小排序
struct AbsCompare {
    bool operator()(int a, int b) {
        return abs(a) < abs(b);  // 绝对值大的优先
    }
};

int main() {
    priority_queue<int, vector<int>, AbsCompare> abs_pq;
    
    abs_pq.push(-30);
    abs_pq.push(10);
    abs_pq.push(-50);
    
    cout << "绝对值最大的: " << abs_pq.top() << endl;  // 输出: -50
}
```

---

# 4. 建堆方式

1. 方式一：默认构造函数（逐个插入）

```cpp
priority_queue<int> pq;  // 创建空堆，然后逐个插入
pq.push(3);
pq.push(1);
pq.push(4);
// ...
```

2. 方式二：范围构造函数（批量建堆，推荐）

```cpp
vector<int> data = {3, 1, 4, 1, 5, 9, 2, 6};
priority_queue<int> pq(data.begin(), data.end());  // 一次性批量建堆
```

3. 时间复杂度分析

| 建堆方式 | 时间复杂度 | 空间复杂度 |
|---------|-----------|-----------|
| 逐个插入 | O(n log n) | O(n) |
| 批量建堆 | **O(n)** | O(n) |



