有时候我们会见到这样的 `main` 函数：

```cpp
int main(int argc, char **argv) {
  ......
}

# 或者
int main(int argc, char *argv[]) {
  ......
}
```

在 `main` 中有两个参数，简单来说，`argc` 和 `argv` 这两个参数是程序与 **操作系统/用户** 沟通的接口。

---

# 1. 参数的具体含义

在 C++ 中，`main` 函数的完整签名通常是：`int main(int argc, char** argv)`

- **`argc` (Argument Count)**：
    - **含义**：命令行参数的 **个数**。
    - **注意**：程序名本身也算一个参数，所以 `argc` 至少是 1。
        
- **`argv` (Argument Vector)**：
    - **含义**：指向参数字符串的 **指针数组**（可以理解为字符串列表）。
    - **内容**：`argv[0]` 是程序路径，`argv[1]` 是第一个参数，以此类推。

> 这两个参数的名字只是约定俗成的命名习惯（Convention），**不是必须取为这两个名字**。

---

# 2. 传参的意义

## 2.1 指定处理的对象

> 如文件操作。

假设你写了一个图片压缩程序。如果没有参数，你只能在代码里写死文件名；有了参数，你可以这样用：

`./compress my_photo.jpg out.jpg`

`compress` 是生成的可执行文件名称，程序通过 `argv[1]` 就能知道你要处理哪张图。

## 2.2 控制程序的行为（开关/选项）

很多工具通过参数改变功能。例如：

`./my_tool --version` （查看版本）

`./my_tool --fast-mode` （开启快速模式）

## 2.3 自动化与脚本集成

程序员经常写脚本（Shell 或 Python）来批量调用 C++ 程序。通过传递不同的参数，脚本可以指挥 C++ 程序完成各种不同的任务。

# 3. 代码示例

我们可以写一个简单的程序来打印这些参数，看看它们到底是什么：

```cpp
#include <iostream>

int main(int argc, char** argv) {
    std::cout << "收到参数个数: " << argc << std::endl;

    for (int i = 0; i < argc; ++i) {
        std::cout << "第 " << i << " 个参数是: " << argv[i] << std::endl;
    }

    // 实际逻辑判断示例
    if (argc > 1 && std::string(argv[1]) == "--help") {
        std::cout << "这是一个示例程序，用法：./app [文件名]" << std::endl;
    }

    return 0;
}
```

> 编译该代码，生成的程序名是 `test_app`。

如果你在命令行输入：

```bash
./test_app hello 123
```

输出将会是：

```bash
收到参数个数: 3
第 0 个参数是: ./test_app
第 1 个参数是: hello
第 2 个参数是: 123
```

如果在命令行输入：

```bash
./test_app --help
```

输出将会是：

```bash
收到参数个数: 2
第 0 个参数是: ./test_app
第 1 个参数是: --help
这是一个示例程序，用法：./app [文件名]
```

---

# 4. 其他问题

## 4.1 Q.1

> Q：为什么 `argv` 是指向 `char` 指针的指针 (`char**`)？

这其实是 C/C++ 处理 **字符串数组** 的一种底层表现形式。我们分三层来理解：

>> 第一层：字符串在 C++ 中是什么？

在底层，字符串是一串连续的 `char`。我们通常用 `char*`（指向字符序列首地址的指针）来表示一个字符串。

>> 第二层：多个字符串（数组）怎么表示？

由于命令行参数通常不止一个（比如 `./app input.txt output.txt`），我们需要把多个 `char*` 存放在一起。

那么，“存放 `char*` 的数组”在作为函数参数传递时，会退化为“指向 `char*` 的指针”。

>> 第三层：内存布局

当你输入命令 `./test hello world` 时，内存里发生了如下布局：

1. **`argv[0]`** 是一个指针，指向字符串 `"./test"` 的首地址。
2. **`argv[1]`** 是一个指针，指向字符串 `"hello"` 的首地址。
3. **`argv[2]`** 是一个指针，指向字符串 `"world"` 的首地址。
4. **`argv[3]`** 通常是一个 `NULL` 指针，标记结束。

因为 `argv` 指向的是这一串“指针的首地址”，所以它的类型自然就是 **指向指针的指针**（`char**`）。

## 4.2 Q.2

> Q：为什么不用 `std::string` 或 `std::vector`？

你可能会想：既然 C++ 有更高级的容器，为什么 `main` 还是这么原始？

- **历史包袱**：`main` 函数的设计源自 70 年代的 C 语言，那时候还没有 `std::string`。
- **ABI 兼容性**：操作系统（Windows/Linux）在启动程序时，是用最原始的方式把参数压入栈的。为了保证 C++ 程序能运行在任何操作系统上，`main` 必须使用最基础、最通用的底层类型。
- **零开销**：`char**` 不需要任何内存分配或复杂的构造函数，启动速度最快。

# 5. 补充

> 一个有趣的小技巧。

如果你觉得处理 `char**` 太麻烦，现代 C++ 程序员通常会第一步就把它转成 `std::vector<std::string>`：

```cpp
#include <vector>
#include <string>

int main(int argc, char** argv) {
    // 将原始的 char** 包装成好用的 vector
    std::vector<std::string> args(argv, argv + argc);
    
    if (args.size() > 1 && args[1] == "run") {
        // 现在可以用熟悉的 string 操作了
    }
}
```

`std::vector<std::string> args(argv, argv + argc);` 使用的是 **“迭代器区间构造函数”**。

> Q：为什么是 `argv` 和 `argv + argc`？

这里涉及到一个核心概念：**指针就是迭代器**。`argv` 是指向指针数组的**第一个元素**（即 `argv[0]` 的地址）。`argv + argc` 是利用了指针算术，它指向该数组 **最后一个元素之后** 的那个位置（即“结束哨兵”）。

虽然 `argv` 是 `char**`，`argc` 是 `int`，但在 C++ 中，`指针 + 整数` 是合法操作。它意味着“将指针向后移动 `n` 个元素的单位距离”。

>> 比如：`argc` 是 3，`argv + argc` 就代表从起点往后数 3 个 `char*` 的位置。

> Q：这里的构造函数是怎么工作的？

`std::vector` 有一个模板构造函数，长得大概像这样：

```cpp
template <class InputIterator> vector(InputIterator first, InputIterator last);
```

它的工作逻辑是：**“从 `first` 开始，一直复制到 `last` 之前的位置。”**

1. 它读取 `argv[0]`（一个 `char*`），发现 `vector` 需要的是 `std::string`。
2. 它会自动调用 `std::string` 的构造函数，把 `char*` 转换成 `std::string` 并存入 `vector`。
3. 它移动到下一个指针 `argv[1]`，重复此过程。
4. 直到到达 `argv + argc`，停止工作。

当然，你也可以选择手动写，不过代码会变得很冗长：

```cpp
std::vector<std::string> args;
for (int i = 0; i < argc; ++i) {
    args.push_back(argv[i]); // 每次都要手动 push
}
```

而 `args(argv, argv + argc)` 只有一行，且编译器通常能对其进行高度优化（比如预先计算好空间，避免多次内存分配）。
