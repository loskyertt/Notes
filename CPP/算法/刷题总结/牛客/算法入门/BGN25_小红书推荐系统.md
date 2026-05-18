# 1. 题目

## 1.1 题目描述

小红书有一个推荐系统，可以根据用户搜索的关键词推荐用户希望获取的内容。

现在给定小红的搜索记录（记录为分词后的结果），我们认为当一个单词出现的次数**不少于 $3$ 次**时，该单词为"用户期望搜索的单词"，即称为关键词。请你根据小红的记录，输出小红的用户画像对应的所有关键词。

## 1.2 输入输出格式

- **输入描述**：一行字符串，仅由小写字母和空格组成，代表小红的搜索记录。字符串长度不超过 $100000$。
- **输出描述**：小红所有的关键词，每行输出一个。按搜索频次从高到低排序；频次相同的，按字典序升序排序。

## 1.3 示例

**输入**：

```
kou red game red ok who game red karaoke yukari kou red red nani kou can koukou ongaku game game
```

**输出**：

```
red
game
kou
```

**手动验证**：

| 单词 | 出现次数 | 是否关键词（≥3 次） |
| :--- | :---: | :---: |
| red | 5 | ✅ |
| game | 4 | ✅ |
| kou | 3 | ✅ |
| koukou | 1 | ❌ |
| ok / who / karaoke 等 | 1 | ❌ |

输出顺序：`red`（5）> `game`（4）> `kou`（3），符合频次降序要求 ✅

---

# 2. 题解

## 2.1 解题思路

本题是经典的**词频统计 + 自定义排序**问题，分三步：

1. **读取并统计词频**：将输入按空格分割为单词，用哈希表（`map`）统计每个单词的出现次数。
2. **过滤关键词**：只保留出现次数 $\geq 3$ 的单词。
3. **排序并输出**：按频次降序排列；频次相同时按字典序升序排列。

## 2.2 原始代码与问题分析

原始代码如下：

```cpp
#include <algorithm>
#include <ios>
#include <iostream>
#include <string>
#include <map>
#include <utility>
#include <vector>

void solve() {
  std::string str;
  std::cin >> str;                    // ← Bug 1

  std::map<std::string, int> res;

  std::string record = "";
  for (auto ch : str) {
    if (ch == ' ') {
      res[record] += 1;
      record = "";
      continue;
    }
    record += ch;
  }
  // ← Bug 2：循环结束后 record 中的最后一个单词未被处理

  std::vector<std::pair<std::string, int>> counts(res.begin(), res.end());
  std::sort(
      counts.begin(), counts.end(),
      [](const std::pair<std::string, int> &a,
         const std::pair<std::string, int> &b) {
        if (a.second != b.second) return a.second > b.second;
        return a.first < b.first;
      });

  for (const auto &val : counts) {
    std::cout << val.first << "\n"; // ← Bug 3：未过滤低频词
  }
}

int main() {
  std::ios::sync_with_stdio(false);
  std::cin.tie(nullptr);
  solve();
}
```

该代码存在 **3 个问题**，层层叠加导致输出完全错误。

### 2.2.1 Bug 1：`cin >> str` 不能读取整行

```cpp
std::string str;
std::cin >> str;   // 只读取第一个空格之前的内容
```

`std::cin >> str` 以**空白字符**（空格、换行、Tab）为分隔符，每次只读入一个 token。对于示例输入：

```
kou red game red ok ...
```

执行后 `str` 的值为 `"kou"`，后续所有单词均未被读入。

因此，代码中试图手动遍历 `str` 并在 `' '` 处切割单词的逻辑**永远不会触发**——`str` 中根本不含空格，`ch == ' '` 的分支永远为假，`res` 最终只存储了 `"kou"` 一个单词（计数为 1）。

**修正方向**：应读取整行输入，再进行分割，或直接循环调用 `cin >> word` 逐词读取。

### 2.2.2 Bug 2：最后一个单词被遗漏

即使用 `getline` 正确读取了整行，手动遍历字符的逻辑仍有缺陷：

```cpp
for (auto ch : str) {
    if (ch == ' ') {
        res[record] += 1;   // 遇到空格才入 map
        record = "";
        continue;
    }
    record += ch;
}
// 循环结束时，record 中保存着最后一个单词，但已无空格触发入 map
```

该逻辑依赖**空格作为哨兵**来触发单词入 map。而输入的最后一个单词后面没有空格，因此循环结束后 `record` 中存有最后一个单词，但它**从未被写入 `res`**。

对于示例，这会导致最后一个单词 `"game"` 少计一次（实际 4 次，统计结果为 3 次），进而可能影响排序与输出。

**修正方向**：循环结束后，若 `record` 非空，需额外执行一次 `res[record] += 1`。

### 2.2.3 Bug 3：未过滤低频词

题目要求只输出出现次数**不少于 3 次**的单词，但原始代码将 `res` 中所有单词（不论频次高低）全部输出：

```cpp
// 未做任何过滤，直接输出全部
for (const auto &val : counts) {
    std::cout << val.first << "\n";
}
```

**修正方向**：输出前加入频次判断 `if (val.second >= 3)`。

## 2.3 代码修正

综合以上三处修正，最简洁的写法是利用 `while (std::cin >> word)` 逐词读取，它会自动跳过所有空白字符直至输入流结束（EOF），从根本上规避 Bug 1 与 Bug 2：

```cpp
#include <algorithm>
#include <iostream>
#include <map>
#include <string>
#include <vector>

void solve() {
    std::string word;
    std::map<std::string, int> counts_map;

    // 1. 逐词读取并统计词频
    //    cin >> word 自动跳过空格/换行，读到 EOF 时返回 false，循环结束
    while (std::cin >> word) {
        counts_map[word]++;
    }

    // 2. 过滤：仅保留出现次数 >= 3 的关键词
    std::vector<std::pair<std::string, int>> filtered;
    for (const auto &kv : counts_map) {
        if (kv.second >= 3) {
            filtered.push_back(kv);
        }
    }

    // 3. 排序：频次降序，频次相同时字典序升序
    std::sort(
        filtered.begin(), filtered.end(),
        [](const std::pair<std::string, int> &a,
           const std::pair<std::string, int> &b) {
            if (a.second != b.second) return a.second > b.second;
            return a.first < b.first;
        });

    // 4. 输出
    for (const auto &kv : filtered) {
        std::cout << kv.first << "\n";
    }
}

int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    solve();
    return 0;
}
```

## 2.4 复杂度分析

设输入字符串长度为 $n$，去重后单词数量为 $m$。

| 阶段 | 时间复杂度 | 说明 |
| :--- | :--- | :--- |
| 读取 + 词频统计 | $O(n \log m)$ | 每次 `map` 插入/查找为 $O(\log m)$，共最多 $O(n)$ 次操作 |
| 过滤 | $O(m)$ | 遍历 `map` 一次 |
| 排序 | $O(m \log m)$ | `std::sort` 对 `filtered` 排序 |
| **总计** | $O(n \log m)$ | 瓶颈在统计阶段 |

在 $n = 100000$ 的约束下，复杂度完全满足要求。

若希望将统计阶段优化至均摊 $O(n)$，可将 `std::map` 替换为 `std::unordered_map`，但在竞赛中通常无必要。

## 2.5 本地测试说明

在本地 IDE 或终端测试时，输入完毕后需手动触发 EOF 信号，`while (cin >> word)` 才会退出循环并执行后续逻辑：

- **Windows**：输入完成后按 `Enter`，再按 `Ctrl + Z`，再按 `Enter`
- **Linux / macOS**：输入完成后按 `Ctrl + D`

在线评测系统（OJ）会在测试数据末尾自动附加 EOF，无需手动操作。
