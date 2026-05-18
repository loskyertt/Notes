> 在 C++ 中，计时是一项非常基础且重要的技能，主要用于性能调优、算法对比或基准测试（Benchmarking）。
> 虽然 C 语言留下了 `clock()`，但其精度较低。现代 C++（C++11 及以后）推荐使用 **`<chrono>`** 标准库。它不仅精度高（可达纳秒级），而且类型安全。

---

# 1. `std::chrono`

`chrono` 库主要由三个核心部分组成：

- **Clock（时钟）**：提供当前时间的类。最常用的是 `std::chrono::high_resolution_clock`（高精度时钟）。
    
- **Time Point（时间点）**：记录某个特定的瞬间。
    
- **Duration（时长）**：两个时间点之间的差值，可以以秒、毫秒、微秒等表示。

---

# 2. 基本计时模板

计算一段代码执行时间的标准流程是：**记录开始点 -> 执行代码 -> 记录结束点 -> 计算差值**。

```cpp
#include <iostream>
#include <chrono>
#include <thread> // 仅用于模拟耗时

int main() {
    // 1. 记录开始时间点
    auto start = std::chrono::high_resolution_clock::now();

    // --- 要计时的代码块 ---
    std::this_thread::sleep_for(std::chrono::seconds(1)); // 模拟干活 1 秒
    // --------------------

    // 2. 记录结束时间点
    auto end = std::chrono::high_resolution_clock::now();

    // 3. 计算时长
    // duration 的模版参数决定了结果的单位（如秒、毫秒、微秒）
    std::chrono::duration<float> duration = end - start;

    // 4. 输出结果
    std::cout << "耗时: " << duration.count() << "s" << std::endl;

    return 0;
}
```

如果你想得到更具体的单位（比如微秒 `us`），可以使用 `std::chrono::cast` 或者直接利用内置的 `duration` 类型：

```cpp
auto start = std::chrono::high_resolution_clock::now();
// ... 执行代码 ...
auto end = std::chrono::high_resolution_clock::now();

// 转换为毫秒 (ms)
auto ms_duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

// 转换为微秒 (us)
auto us_duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);

std::cout << ms_duration.count() << "ms\n";
std::cout << us_duration.count() << "us\n";
```

---

# 3. 自动计时器

在实际开发中，如果要在每个函数里都写 `start` 和 `end` 会非常麻烦。我们可以利用 C++ 的 **RAII（资源获取即初始化）** 机制：创建一个对象，在构造时计时，在析构（超出作用域）时自动打印时间。

```cpp
#include <chrono>
#include <iostream>

struct Timer {
  std::chrono::time_point<std::chrono::high_resolution_clock> start, end;

  Timer() { start = std::chrono::high_resolution_clock::now(); }

  ~Timer() {
    end = std::chrono::high_resolution_clock::now();
    auto duration =
        std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    std::cout << "Timer 耗时: " << duration.count() << "us ("
              << duration.count() * 0.001f << "ms)\n";
  }
};

void FunctionToTest() {
  Timer timer;  // 只要这一行，函数结束时会自动打印时间

  for (int i = 0; i < 1000; i++) {
    std::cout << "Hello\n";
  }
}  // 代码运行到这儿时，自动调用 Timer 的析构函数

int main() {
  FunctionToTest();
}
```

---

# 4. 注意事项

- **不要在计时循环里打印 `std::cout`**：控制台输出是非常慢的 IO 操作。如果你在计时的循环里不停地打印，你测得的时间实际上是**打印字符串的时间**，而不是你逻辑代码的时间。
    
- **编译器优化**：如果你写了一段完全没副作用的代码（比如空转循环），编译器可能会直接把它删掉，导致测得时间为 0。测试性能时通常需要开启一定的优化级别（如 `-O2`），并确保代码结果被使用了。
    
- **时钟选择**：
    - `system_clock`：代表系统时间（可以被用户手动修改，不适合测时长）。
    - `steady_clock`：单调递增时钟，像秒表一样，不受系统调时影响，最适合测时长。
    - `high_resolution_clock`：通常是上述两者中精度最高的一个别名。
