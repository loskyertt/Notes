# 1. 题目

给定 $n$ 个元素，第 $i$ 个元素具有特征值 $A_i$。定义 **FST 距离** 如下：

$$
\text{dist}(i, j) = |i^2 - j^2| + |A_i^2 - A_j^2|
$$

请计算 $A_i$ 中所有元素对儿中的最大 FST 距离。

- **输入描述**：
  - 第一行输入一个整数 $n$（$1 \leq n \leq 10^5$）。
  - 第二行输入 $n$ 个整数 $A_1, A_2, \dots, A_n$（$1 \leq A_i \leq 10^9$）。

- **输出描述**：输出一个整数，表示最大距离。

示例1

```
输入：2
4 3

输出：10
```

> **说明**：$|2^2 - 1^2| + |4^2 - 3^2| = 3 + 7 = 10$（下标 $i, j$ 默认从 1 开始）。

---

# 2. 题解

本题的核心在于如何在给定的数据规模下，高效地计算最大距离。我们需要从时间复杂度和数值范围两个维度进行分析。

## 2.1 暴力解法及其局限性分析

最直观的思路是枚举所有可能的下标对 $(i, j)$，计算距离并取最大值。

### 2.1.1 代码实现

```cpp
#include <algorithm>
#include <cmath>
#include <iostream>
#include <vector>

void solve() {
  int n;
  std::cin >> n;

  std::vector<int> nums(n + 1);
  for (int i = 1; i <= n; ++i) {
    std::cin >> nums[i];
  }

  int max_dist = 0;
  for (int i = 1; i <= n; ++i) {
    for (int j = 1; j <= n; ++j) {
      // 注意：这里存在严重的溢出风险
      int dist = std::abs(i * i - j * j) + std::abs(nums[i] * nums[i] - nums[j] * nums[j]);
      max_dist = std::max(max_dist, dist);
    }
  }

  std::cout << max_dist << "\n";
}

int main() {
  std::ios::sync_with_stdio(false);
  std::cin.tie(nullptr);
  solve();
  return 0;
}
```

### 2.1.2 存在的问题

该代码在题目给定的数据范围下存在两个致命问题：

1. **时间复杂度过高 (TLE)**
    - 代码使用了双层嵌套循环，时间复杂度为 $O(n^2)$。
    - 题目中 $n \le 10^5$，这意味着运算次数将达到 $10^{10}$ 级别。
    - 在一般竞赛环境（1秒时限）下，CPU 只能处理约 $10^8$ 次运算，该代码必然超时。

2. **数值溢出**
    - 题目中 $A_i \le 10^9$。
    - 计算 $A_i^2$ 时，结果可达 $10^{18}$。
    - C++ 中 `int` 类型的最大值仅为 $2.1 \times 10^9$。使用 `int` 存储平方结果会发生溢出，导致计算结果错误甚至变为负数。必须使用 `long long` 类型。

## 2.2 优化思路：曼哈顿距离性质

为了将时间复杂度从 $O(n^2)$ 降低，我们需要利用数学性质减少枚举次数。

### 2.2.1 模型转化

观察题目给出的距离公式：
$$ \text{dist}(i, j) = |i^2 - j^2| + |A_i^2 - A_j^2| $$

令 $x_i = i^2$，$y_i = A_i^2$，则公式可转化为二维平面上的**曼哈顿距离**公式：
$$ \text{dist}(i, j) = |x_i - x_j| + |y_i - y_j| $$

### 2.2.2 绝对值展开与最值性质

绝对值 $|A|$ 本质上是 $\max(A, -A)$。那么对于两个绝对值相加 $|A| + |B|$，它其实就是下面四种组合中的**最大值**：
1. $+(A) + (B)$
2. $+(A) - (B)$
3. $-(A) + (B)$
4. $-(A) - (B)$

我们将题目中的 $x_i - x_j$ 代入 $A$，把 $y_i - y_j$ 代入 $B$：

* **组合 1：** $(x_i - x_j) + (y_i - y_j) = \mathbf{(x_i + y_i) - (x_j + y_j)}$
* **组合 2：** $(x_i - x_j) - (y_i - y_j) = \mathbf{(x_i - y_i) - (x_j - y_j)}$
* **组合 3：** $-(x_i - x_j) + (y_i - y_j) = \mathbf{(y_i - x_i) - (y_j - x_j)}$ （这等价于组合 2 的相反数）
* **组合 4：** $-(x_i - x_j) - (y_i - y_j) = \mathbf{-(x_i + y_i) + (x_j + y_j)}$ （这等价于组合 1 的相反数）

> 这样做的目的是把 $x_i$ 和 $y_i$ 凑在一起。

注意看上面的**组合 1**：我们要让 $(x_i + y_i) - (x_j + y_j)$ 尽量大。
那最理想的情况不就是让前面的 $(x_i + y_i)$ 取得**整个数组里的最大值**，让后面的 $(x_j + y_j)$ 取得**整个数组里的最小值**吗？

所以：
* 组合 1 和 4 的最大可能值就是：$\max(x + y) - \min(x + y)$
* 组合 2 和 3 的最大可能值就是：$\max(x - y) - \min(x - y)$

最后，我们在这两个“候选最大值”里再取一个 $\max$，就得到了全场最高分。

因此，最大曼哈顿距离公式可简化为：
$$ \text{MaxDist} = \max\Big( \max(x+y) - \min(x+y), \max(x-y) - \min(x-y) \Big) $$

### 2.2.3 算法流程

基于上述推导，我们只需遍历数组一次，维护四个极值变量即可。

1.  定义四个变量：`max_sum`, `min_sum`, `max_diff`, `min_diff`。
2.  遍历每个元素 $i$，计算 $x = i^2$ 和 $y = A_i^2$。
3.  更新 `max_sum = max(max_sum, x + y)` 和 `min_sum = min(min_sum, x + y)`。
4.  更新 `max_diff = max(max_diff, x - y)` 和 `min_diff = min(min_diff, x - y)`。
5.  最终结果为 `max(max_sum - min_sum, max_diff - min_diff)`。

该算法的时间复杂度为 $O(n)$，空间复杂度为 $O(1)$，完全符合要求。

## 2.3 优化后的代码实现

```cpp
#include <algorithm>
#include <climits>
#include <ios>
#include <iostream>

void solve() {
  int n;
  std::cin >> n;

  // 初始化极值变量
  // 由于数值最大可达 10^18，必须使用 long long
  // 初始化时，max 初始化为极小值，min 初始化为极大值
  long long max_sum = LLONG_MIN;
  long long min_sum = LLONG_MAX;
  long long max_diff = LLONG_MIN;
  long long min_diff = LLONG_MAX;

  for (long long i = 1; i <= n; ++i) {
    long long val;
    std::cin >> val;

    // 计算 x=i^2, y=A_i^2
    long long x = i * i;
    long long y = val * val;

    // 维护 x + y 的极值
    max_sum = std::max(max_sum, x + y);
    min_sum = std::min(min_sum, x + y);

    // 维护 x - y 的极值
    max_diff = std::max(max_diff, x - y);
    min_diff = std::min(min_diff, x - y);
  }

  long long ans = std::max(max_sum - min_sum, max_diff - min_diff);
  std::cout << ans << "\n";
}

int main() {
  std::ios::sync_with_stdio(false);
  std::cin.tie(nullptr);

  solve();
}
```

## 2.4 关键点总结

1.  **数据类型选择**：处理 $10^9$ 级别数据的平方运算时，必须使用 `long long`，否则会产生溢出。
2.  **算法降维**：利用曼哈顿距离的性质，将 $O(n^2)$ 的枚举转化为 $O(n)$ 的极值统计。
3.  **数学转换**：核心公式 $|x_i - x_j| + |y_i - y_j| = \max(|(x_i+y_i) - (x_j+y_j)|, |(x_i-y_i) - (x_j-y_j)|)$ 是解决此类问题的通用技巧。这一技巧同样适用于更高维度的切比雪夫距离转换等问题。