---
title: C++ Lambda 函数与 std::bind 转换
date: 2026-07-22
tags:
  - C++
  - STL
  - Lambda
  - std-bind
  - 函数对象
aliases:
  - lambda函数
  - std::bind 转 lambda
---

# 1. std::bind 与 Lambda 的基本认识

C++ 中经常需要把“某个函数调用”包装成一个可以稍后执行的对象。例如：

- STL 算法需要接收一个谓词。
- 回调系统需要保存一个回调函数。
- 事件系统需要把对象成员函数绑定成统一形式。
- 线程、任务队列、定时器需要延迟执行某个调用。

`std::bind` 和 lambda 都可以完成这类事情。现代 C++ 中，lambda 通常更直观、更**推荐**；但理解 `std::bind` 的规则，有助于阅读老代码，也能帮助理解函数对象、成员函数指针、完美转发等知识点。

## 1.1 std::bind 的基本作用

`std::bind` 定义在 `<functional>` 中。它会生成一个函数对象。调用这个函数对象时，内部会再调用原始函数，并按照绑定规则组合参数。

下面是一个可以简单示例：

```cpp
#include <functional>
#include <iostream>

int foo(int x, int y, int z) {
    std::cout << "foo(" << x << ", " << y << ", " << z << ")\n";
    return x + y + z;
}

int main() {
    using namespace std::placeholders;

    // 生成一个函数对象 f。
    // _1 表示 f 被调用时传入的第 1 个参数。
    // _2 表示 f 被调用时传入的第 2 个参数。
    // 42 是提前固定好的参数。
    auto f = std::bind(foo, _1, 42, _2);

    int result = f(10, 20);

    // 实际效果等价于：
    // int result = foo(10, 42, 20);
    std::cout << "result = " << result << '\n';
}
```

输出结果：

```text
foo(10, 42, 20)
result = 72
```

核心含义是：

```cpp
auto f = std::bind(foo, _1, 42, _2);
```

等价于把下面这个调用模式保存起来：

```cpp
foo(调用时的第1个参数, 42, 调用时的第2个参数)
```

## 1.2 Lambda 的基本作用

Lambda 表达式是 C++11 引入的匿名函数对象。它可以直接在代码中定义一个局部的可调用对象。

下面示例与上一节 `std::bind` 的行为相同：

```cpp
#include <iostream>
#include <utility>

int foo(int x, int y, int z) {
    std::cout << "foo(" << x << ", " << y << ", " << z << ")\n";
    return x + y + z;
}

int main() {
    auto f = [](auto&& a, auto&& b) -> decltype(auto) {
        return foo(
            std::forward<decltype(a)>(a),
            42,
            std::forward<decltype(b)>(b)
        );
    };

    int result = f(10, 20);

    std::cout << "result = " << result << '\n';
}
```

输出结果：

```text
foo(10, 42, 20)
result = 72
```

Lambda 写法的结构是：

```cpp
[捕获列表](形参列表) -> 返回类型 {
    函数体
};
```

在上面的例子中：

```cpp
[](auto&& a, auto&& b) -> decltype(auto) {
    return foo(...);
}
```

- `[]`：不捕获外部变量。
- `auto&& a, auto&& b`：用泛型 lambda 接收任意类型的第 1、2 个参数。
- `std::forward<decltype(a)>(a)`：把参数按照传入时的左值或右值状态继续传给 `foo`。
- `-> decltype(auto)`：尽量保留 `foo` 的真实返回类型，包括引用返回。
- `return foo(...)`：lambda 调用结果就是 `foo(...)` 的调用结果。

> [!summary]
> `std::bind` 是“用占位符描述未来调用”；lambda 是“直接写出未来调用时要执行的代码”。

---

# 2. Lambda 的核心基础

## 2.1 Lambda 的基本语法

一个较完整的 lambda 表达式形式如下：

```cpp
[捕获列表](形参列表) 可选说明符 -> 返回类型 {
    函数体
};
```

常见组成部分：

| 部分 | 说明 |
|---|---|
| `[]` | 捕获列表，决定 lambda 是否保存外部变量 |
| `()` | 形参列表，决定调用 lambda 时需要传入什么参数 |
| `mutable` | 允许修改按值捕获的副本 |
| `-> 返回类型` | 显式指定返回类型 |
| `{}` | 函数体 |

最基础示例：

```cpp
#include <iostream>

int main() {
    auto greet = []() {
        std::cout << "Hello, lambda!\n";
    };

    greet();
}
```

## 2.2 捕获列表

Lambda 的捕获列表决定它如何访问定义处的外部变量。

```cpp
#include <iostream>

int main() {
    int x = 10;
    int y = 20;

    // 值捕获：lambda 内保存 x 的副本。
    auto by_value = [x]() {
        std::cout << "x copy = " << x << '\n';
    };

    // 引用捕获：lambda 内直接访问外部 y。
    auto by_reference = [&y]() {
        y += 1;
    };

    x = 100;
    y = 200;

    by_value();      // 输出 10，因为捕获的是创建 lambda 时的副本
    by_reference();  // 修改外部 y

    std::cout << "y = " << y << '\n'; // 输出 201
}
```

常见捕获方式：

| 写法 | 含义 |
|---|---|
| `[]` | 不捕获任何外部变量 |
| `[x]` | 按值捕获 `x` |
| `[&x]` | 按引用捕获 `x` |
| `[=]` | 默认按值捕获被使用的外部变量 |
| `[&]` | 默认按引用捕获被使用的外部变量 |
| `[this]` | 捕获当前对象指针 |
| `[*this]` | C++17 起，按值捕获当前对象副本 |

## 2.3 返回类型与 return

Lambda 如果不写返回类型，编译器通常可以自动推导。

```cpp
auto add = [](int a, int b) {
    return a + b; // 推导返回 int
};
```

但是在写函数包装器时，经常推荐使用：

```cpp
-> decltype(auto)
```

原因是它能保留引用返回。

```cpp
#include <iostream>

int global_value = 10;

int& get_value() {
    return global_value;
}

int main() {
    auto bad = []() {
        return get_value(); // 返回类型推导为 int，引用被丢失
    };

    auto good = []() -> decltype(auto) {
        return get_value(); // 返回类型为 int&，保留引用
    };

    int copied = bad(); // copied 是一份 int 副本
    copied = 20;        // 只修改 copied，不会修改 global_value

    good() = 30;        // good() 返回 int&，会修改 global_value

    std::cout << "copied = " << copied << '\n';
    std::cout << "global_value = " << global_value << '\n';
}
```

输出结果：

```text
copied = 20
global_value = 30
```

> [!warning]
> 如果原来的 `std::bind` 调用结果会被使用，那么 lambda 中应该写 `return foo(...)`。如果省略 `return`，lambda 的结果通常会变成 `void`，就不再等价。

## 2.4 泛型 Lambda 与 auto&&

C++14 开始，lambda 形参可以使用 `auto`，这叫泛型 lambda。

```cpp
#include <iostream>

int main() {
    auto add = [](auto a, auto b) {
        return a + b;
    };

    std::cout << add(1, 2) << '\n';
    std::cout << add(1.5, 2.5) << '\n';
}
```

当形参写成 `auto&&` 时，它在泛型 lambda 中通常是**转发引用**，可以接收左值，也可以接收右值。

```cpp
#include <iostream>
#include <utility>

void inspect(int&) {
    std::cout << "int&\n";
}

void inspect(const int&) {
    std::cout << "const int&\n";
}

void inspect(int&&) {
    std::cout << "int&&\n";
}

int main() {
    auto wrapper = [](auto&& x) {
        // 注意：在函数体中，变量名 x 本身永远是左值。
        // 因此需要 std::forward 才能保留调用 wrapper 时的值类别。
        inspect(std::forward<decltype(x)>(x));
    };

    int a = 1;
    const int ca = 2;

    wrapper(a);            // 输出 int&
    wrapper(ca);           // 输出 const int&
    wrapper(3);            // 输出 int&&
    wrapper(std::move(a)); // 输出 int&&
}
```

可将这条规则记为：

```text
auto&& 负责接住左值和右值
std::forward 负责按原来的左值或右值身份继续传下去
```

---

# 3. std::bind 的工作规则

## 3.1 占位符规则

`std::placeholders::_1`、`_2`、`_3` 等占位符表示调用绑定对象时传入的第几个参数。

```cpp
#include <functional>
#include <iostream>

void show(int a, int b, int c) {
    std::cout << a << ", " << b << ", " << c << '\n';
}

int main() {
    using namespace std::placeholders;

    auto f = std::bind(show, _2, 100, _1);

    // _2 取调用 f 时的第 2 个参数，即 20。
    // _1 取调用 f 时的第 1 个参数，即 10。
    f(10, 20); // 输出 20, 100, 10
}
```

这里：

```cpp
std::bind(show, _2, 100, _1)
```

表示：

```cpp
show(调用时第2个参数, 100, 调用时第1个参数)
```

## 3.2 普通绑定参数

`std::bind` 会把普通绑定参数复制或移动到返回的函数对象中。之后调用时，这些保存的参数会作为左值传给目标函数。

```cpp
#include <functional>
#include <iostream>

void print_value(int value, int x) {
    std::cout << "value = " << value << ", x = " << x << '\n';
}

int main() {
    using namespace std::placeholders;

    int value = 10;

    auto f = std::bind(print_value, value, _1);

    value = 99;

    // bind 保存的是 value 当时的副本 10。
    f(1); // 输出 value = 10, x = 1
}
```

## 3.3 引用绑定参数

如果希望 `std::bind` 保存引用，需要使用 `std::ref` 或 `std::cref`。

```cpp
#include <functional>
#include <iostream>

void print_value(int value, int x) {
    std::cout << "value = " << value << ", x = " << x << '\n';
}

int main() {
    using namespace std::placeholders;

    int value = 10;

    auto f = std::bind(print_value, std::ref(value), _1);

    value = 99;

    // bind 通过 std::ref 保存了 value 的引用。
    f(1); // 输出 value = 99, x = 1
}
```

## 3.4 多余参数会被忽略

`std::bind` 生成的函数对象可以接收多余参数，只要这些参数没有被占位符使用，就会被忽略。

```cpp
#include <functional>
#include <iostream>

void print_one(int x) {
    std::cout << x << '\n';
}

int main() {
    using namespace std::placeholders;

    auto f = std::bind(print_one, _1);

    f(10, 20, 30); // 合法，20 和 30 被忽略
}
```

普通 lambda 默认不会忽略多余参数。若要模拟这个行为，需要显式添加参数包：

```cpp
#include <iostream>
#include <utility>

void print_one(int x) {
    std::cout << x << '\n';
}

int main() {
    auto f = [](auto&& x, auto&&...) {
        print_one(std::forward<decltype(x)>(x));
    };

    f(10, 20, 30); // 合法，20 和 30 被 auto&&... 接住但未使用
}
```

## 3.5 嵌套 bind 表示函数组合

如果 `std::bind` 的绑定参数本身也是一个 bind 表达式，那么内部 bind 会先被调用，其返回值再传给外层函数。

```cpp
#include <functional>
#include <iostream>

int bar(int x) {
    std::cout << "bar(" << x << ")\n";
    return x * 2;
}

int foo(int x, int y) {
    std::cout << "foo(" << x << ", " << y << ")\n";
    return x + y;
}

int main() {
    using namespace std::placeholders;

    auto f = std::bind(foo, std::bind(bar, _1), _2);

    int result = f(10, 3);

    // 等价于：
    // int result = foo(bar(10), 3);
    std::cout << "result = " << result << '\n';
}
```

---

# 4. std::bind 转 Lambda 的规则

## 4.1 基本翻译模板

如果有：

```cpp
auto f = std::bind(target, arg1, arg2, arg3);
```

可以按下面模板转换：

```cpp
auto f = [需要保存的外部变量](auto&& p1, auto&& p2, auto&& p3) -> decltype(auto) {
    return target(翻译后的 arg1, 翻译后的 arg2, 翻译后的 arg3);
};
```

翻译时逐个处理 `arg1`、`arg2`、`arg3`。

| bind 参数 | Lambda 中的写法 | 说明 |
|---|---|---|
| `_1` | `std::forward<decltype(p1)>(p1)` | 调用 lambda 时的第 1 个参数 |
| `_2` | `std::forward<decltype(p2)>(p2)` | 调用 lambda 时的第 2 个参数 |
| 普通值 `x` | 捕获 `[x]` 后在函数体中使用 `x` | 模拟 bind 按值保存 |
| `std::ref(x)` | 捕获 `[&x]` 后在函数体中使用 `x` | 模拟 bind 按引用保存 |
| 嵌套 bind | 写成嵌套函数调用 | 例如 `foo(bar(x))` |
| 成员函数指针 | 使用 `std::invoke` 或 `obj.member(...)` | 需要提供对象作为 `this` 来源 |

> [!tip]
> 转换时先不要急着写完整 lambda，可以先把 `std::bind` 展开成“实际会调用哪个函数、参数顺序是什么”，再写 lambda。

## 4.2 普通函数：固定参数

`std::bind` 写法：

```cpp
auto f = std::bind(foo, _1, 42, _2);
```

lambda 写法：

```cpp
auto f = [](auto&& a, auto&& b) -> decltype(auto) {
    return foo(
        std::forward<decltype(a)>(a),
        42,
        std::forward<decltype(b)>(b)
    );
};
```

完整可运行示例：

```cpp
#include <functional>
#include <iostream>
#include <utility>

int foo(int x, int y, int z) {
    return x + y + z;
}

int main() {
    using namespace std::placeholders;

    auto bind_f = std::bind(foo, _1, 42, _2);

    auto lambda_f = [](auto&& a, auto&& b) -> decltype(auto) {
        return foo(
            std::forward<decltype(a)>(a),
            42,
            std::forward<decltype(b)>(b)
        );
    };

    std::cout << bind_f(1, 2) << '\n';   // 输出 45
    std::cout << lambda_f(1, 2) << '\n'; // 输出 45
}
```

## 4.3 普通函数：调整参数顺序

`std::bind` 写法：

```cpp
auto f = std::bind(foo, _2, _1);
```

lambda 写法：

```cpp
auto f = [](auto&& a, auto&& b) -> decltype(auto) {
    return foo(
        std::forward<decltype(b)>(b),
        std::forward<decltype(a)>(a)
    );
};
```

完整可运行示例：

```cpp
#include <functional>
#include <iostream>
#include <string>
#include <utility>

void show_pair(const std::string& first, const std::string& second) {
    std::cout << first << " -> " << second << '\n';
}

int main() {
    using namespace std::placeholders;

    auto bind_f = std::bind(show_pair, _2, _1);

    auto lambda_f = [](auto&& a, auto&& b) -> decltype(auto) {
        return show_pair(
            std::forward<decltype(b)>(b),
            std::forward<decltype(a)>(a)
        );
    };

    bind_f("left", "right");   // 输出 right -> left
    lambda_f("left", "right"); // 输出 right -> left
}
```

> [!warning]
> 这种写法过于抽象，项目开发中这样写会被打死。

## 4.4 普通值绑定：值捕获

`std::bind` 默认保存普通参数的副本。

```cpp
auto f = std::bind(foo, value, _1);
```

对应 lambda：

```cpp
auto f = [value](auto&& x) mutable -> decltype(auto) {
    return foo(value, std::forward<decltype(x)>(x));
};
```

完整示例：

```cpp
#include <functional>
#include <iostream>
#include <utility>

int add(int base, int x) {
    return base + x;
}

int main() {
    using namespace std::placeholders;

    int base = 10;

    auto bind_f = std::bind(add, base, _1);

    auto lambda_f = [base](auto&& x) mutable -> decltype(auto) {
        return add(base, std::forward<decltype(x)>(x));
    };

    base = 100;

    std::cout << bind_f(1) << '\n';   // 输出 11
    std::cout << lambda_f(1) << '\n'; // 输出 11
}
```

这里的 `mutable` 是为了更接近 `std::bind` 保存内部副本后的调用语义。很多场景不需要修改这个副本时，可以省略 `mutable`。

## 4.5 引用绑定：引用捕获

`std::bind` 中如果使用 `std::ref(x)`，说明希望保存引用。

```cpp
auto f = std::bind(foo, std::ref(value), _1);
```

对应 lambda：

```cpp
auto f = [&value](auto&& x) -> decltype(auto) {
    return foo(value, std::forward<decltype(x)>(x));
};
```

完整示例：

```cpp
#include <functional>
#include <iostream>
#include <utility>

int add(int base, int x) {
    return base + x;
}

int main() {
    using namespace std::placeholders;

    int base = 10;

    auto bind_f = std::bind(add, std::ref(base), _1);

    auto lambda_f = [&base](auto&& x) -> decltype(auto) {
        return add(base, std::forward<decltype(x)>(x));
    };

    base = 100;

    std::cout << bind_f(1) << '\n';   // 输出 101
    std::cout << lambda_f(1) << '\n'; // 输出 101
}
```

## 4.6 嵌套 bind：函数组合

`std::bind` 写法：

```cpp
auto f = std::bind(foo, std::bind(bar, _1), _2);
```

lambda 写法：

```cpp
auto f = [](auto&& x, auto&& y) -> decltype(auto) {
    return foo(
        bar(std::forward<decltype(x)>(x)),
        std::forward<decltype(y)>(y)
    );
};
```

完整示例：

```cpp
#include <functional>
#include <iostream>
#include <utility>

int bar(int x) {
    return x * 2;
}

int foo(int x, int y) {
    return x + y;
}

int main() {
    using namespace std::placeholders;

    auto bind_f = std::bind(foo, std::bind(bar, _1), _2);

    auto lambda_f = [](auto&& x, auto&& y) -> decltype(auto) {
        return foo(
            bar(std::forward<decltype(x)>(x)),
            std::forward<decltype(y)>(y)
        );
    };

    std::cout << bind_f(10, 3) << '\n';   // 输出 23
    std::cout << lambda_f(10, 3) << '\n'; // 输出 23
}
```

---

# 5. 成员函数与 this 的绑定

## 5.1 已经有对象实例时的绑定

`std::bind` 写法：

```cpp
auto f = std::bind(&Player::addScore, &p, _1);
```

lambda 写法：

```cpp
auto f = [&p](auto&& delta) -> decltype(auto) {
    return p.addScore(std::forward<decltype(delta)>(delta));
};
```

完整示例：

```cpp
#include <functional>
#include <iostream>
#include <utility>

struct Player {
    int addScore(int delta) {
        score += delta;
        return score;
    }

    int score = 0;
};

int main() {
    using namespace std::placeholders;

    Player p;

    auto bind_f = std::bind(&Player::addScore, &p, _1);

    auto lambda_f = [&p](auto&& delta) -> decltype(auto) {
        return p.addScore(std::forward<decltype(delta)>(delta));
    };

    std::cout << bind_f(10) << '\n';   // 输出 10
    std::cout << lambda_f(20) << '\n'; // 输出 30
}
```

更通用的写法是使用 `std::invoke`：

```cpp
auto lambda_f = [&p](auto&& delta) -> decltype(auto) {
    return std::invoke(
        &Player::addScore,
        p,
        std::forward<decltype(delta)>(delta)
    );
};
```

`std::invoke` 可以统一处理普通函数、函数对象、成员函数指针和成员变量指针。

> [!question] 在用 `std::bind` 时为什么写 `&Player::addScore`？

非静态成员函数不是普通函数。它需要依附某个对象才能调用，因为调用时需要一个隐藏的 `this` 指针。

成员函数指针写法是：

```cpp
&Player::addScore
```

它的类型大致是：

```cpp
int (Player::*)(int)
```

含义是：

```text
指向 Player 的一个成员函数：
参数是 int
返回值是 int
调用时还需要一个 Player 对象作为 this 来源
```

不能写：

```cpp
Player.addScore
```

因为 `Player` 是类型，不是对象。

也不能把：

```cpp
p.addScore
```

当作普通函数对象传给 `std::bind`。C++ 中表达“某个类的成员函数本身”时，要使用成员函数指针语法：

```cpp
&Player::addScore
```

## 5.2 在类内部绑定当前对象 this

如果在类的非静态成员函数中创建回调，虽然代码写在类定义内部，但运行到这个成员函数时，当前对象已经存在。因此可以绑定 `this`。

```cpp
#include <functional>
#include <iostream>
#include <utility>

struct Player {
    int score = 0;

    int addScore(int delta) {
        score += delta;
        return score;
    }

    auto makeBindCallback() {
        using namespace std::placeholders;

        // this 是当前对象的地址。
        // 调用返回的函数对象时，会对当前对象调用 addScore。
        return std::bind(&Player::addScore, this, _1);
    }

    auto makeLambdaCallback() {
        // 捕获 this，表示 lambda 内保存当前对象指针。
        return [this](auto&& delta) -> decltype(auto) {
            return addScore(std::forward<decltype(delta)>(delta));
        };
    }
};

int main() {
    Player p;

    auto bind_cb = p.makeBindCallback();
    auto lambda_cb = p.makeLambdaCallback();

    std::cout << bind_cb(10) << '\n';   // 输出 10
    std::cout << lambda_cb(20) << '\n'; // 输出 30
}
```

> [!warning]
> `[this]` 捕获的是当前对象指针，不会延长对象生命周期。如果 lambda 被保存到对象生命周期之外，再调用就会产生悬垂指针风险。

## 5.4 静态成员函数的绑定

静态成员函数属于类本身，不依赖某个具体对象，因此它和普通函数更接近。

```cpp
struct Player {
    static int makeScore(int base, int bonus) {
        return base + bonus;
    }
};
```

静态成员函数可以通过类名调用：

```cpp
Player::makeScore(10, 5);
```

也可以取地址后作为可调用目标：

```cpp
&Player::makeScore
```

它的类型是普通函数指针，而不是成员函数指针：

```cpp
int (*)(int, int)
```

这和非静态成员函数不同：

```cpp
int (Player::*)(int)
```

核心区别是：

| 函数类型 | 是否需要对象 | bind 时是否需要绑定 `this` |
|---|---|---|
| 非静态成员函数 | 需要 | 需要对象、对象指针或占位符 |
| 静态成员函数 | 不需要 | 不需要 |

### 5.4.1 在对象外部绑定静态成员函数

`std::bind` 写法：

```cpp
auto f = std::bind(&Player::makeScore, _1, 100);
```

lambda 写法：

```cpp
auto f = [](auto&& base) -> decltype(auto) {
    return Player::makeScore(
        std::forward<decltype(base)>(base),
        100
    );
};
```

完整示例：

```cpp
#include <functional>
#include <iostream>
#include <utility>

struct Player {
    static int makeScore(int base, int bonus) {
        return base + bonus;
    }
};

int main() {
    using namespace std::placeholders;

    auto bind_f = std::bind(&Player::makeScore, _1, 100);

    auto lambda_f = [](auto&& base) -> decltype(auto) {
        return Player::makeScore(
            std::forward<decltype(base)>(base),
            100
        );
    };

    std::cout << bind_f(20) << '\n';   // 输出 120
    std::cout << lambda_f(20) << '\n'; // 输出 120
}
```

这里不需要写：

```cpp
std::bind(&Player::makeScore, &p, _1)
```

因为 `makeScore` 是静态成员函数，没有隐藏的 `this` 参数。绑定一个对象反而会让参数数量和函数签名不匹配。

### 5.4.2 在类内部绑定静态成员函数

在类的成员函数内部创建回调时，静态成员函数仍然不需要捕获 `this`。

`std::bind` 写法：

```cpp
return std::bind(&Player::makeScore, _1, default_bonus);
```

lambda 写法：

```cpp
return [default_bonus](auto&& base) -> decltype(auto) {
    return Player::makeScore(
        std::forward<decltype(base)>(base),
        default_bonus
    );
};
```

完整示例：

```cpp
#include <functional>
#include <iostream>
#include <utility>

struct Player {
    static int makeScore(int base, int bonus) {
        return base + bonus;
    }

    auto makeBindCallback() const {
        using namespace std::placeholders;

        int default_bonus = 100;

        // 静态成员函数不需要 this。
        // 这里只绑定第 2 个参数 bonus，第 1 个参数 base 留给调用者传入。
        return std::bind(&Player::makeScore, _1, default_bonus);
    }

    auto makeLambdaCallback() const {
        int default_bonus = 100;

        // 只捕获 default_bonus，不捕获 this。
        return [default_bonus](auto&& base) -> decltype(auto) {
            return Player::makeScore(
                std::forward<decltype(base)>(base),
                default_bonus
            );
        };
    }
};

int main() {
    Player p;

    auto bind_cb = p.makeBindCallback();
    auto lambda_cb = p.makeLambdaCallback();

    std::cout << bind_cb(20) << '\n';   // 输出 120
    std::cout << lambda_cb(20) << '\n'; // 输出 120
}
```

这里虽然是通过对象 `p` 调用了 `makeBindCallback()` 和 `makeLambdaCallback()`，但返回的回调本身并不依赖 `p`。只要 `default_bonus` 已经按值保存进回调，即使 `p` 之后销毁，这两个回调也不会因为缺少 `this` 而失效。

> [!summary]
> 静态成员函数转 lambda 时，把它当作“带类名作用域的普通函数”即可：调用时写 `Player::func(...)`，绑定时不需要 `this`，也不需要对象地址。

## 5.5 不绑定对象，把对象留给调用者传入

也可以只绑定成员函数，不提前绑定具体对象。

`std::bind` 写法：

```cpp
auto f = std::bind(&Player::addScore, _1, _2);
```

这表示调用 `f(player, delta)` 时，把第一个参数当作对象，把第二个参数当作 `delta`。

lambda 写法：

```cpp
auto f = [](auto&& player, auto&& delta) -> decltype(auto) {
    return std::invoke(
        &Player::addScore,
        std::forward<decltype(player)>(player),
        std::forward<decltype(delta)>(delta)
    );
};
```

完整示例：

```cpp
#include <functional>
#include <iostream>
#include <utility>

struct Player {
    int score = 0;

    int addScore(int delta) {
        score += delta;
        return score;
    }
};

int main() {
    using namespace std::placeholders;

    Player p1;
    Player p2;

    auto bind_f = std::bind(&Player::addScore, _1, _2);

    auto lambda_f = [](auto&& player, auto&& delta) -> decltype(auto) {
        return std::invoke(
            &Player::addScore,
            std::forward<decltype(player)>(player),
            std::forward<decltype(delta)>(delta)
        );
    };

    std::cout << bind_f(p1, 10) << '\n';    // 输出 10
    std::cout << lambda_f(&p2, 20) << '\n'; // 输出 20，std::invoke 支持对象指针
}
```

这里 lambda 的捕获列表是空的：

```cpp
[]
```

因为它没有保存某个固定对象。对象是未来调用时通过参数传进来的：

```cpp
lambda_f(p, 10);
lambda_f(&p, 10);
```

---

# 6. std::forward 在转换中的作用

## 6.1 为什么不能只写 foo(a)

在函数体中，具名变量表达式本身是左值。即使 `a` 的类型是右值引用，表达式 `a` 仍然是左值。

```cpp
auto f = [](auto&& a) {
    foo(a); // a 在这里一定作为左值传递
};
```

如果希望调用 lambda 时传进来的右值继续作为右值传给 `foo`，就需要：

```cpp
std::forward<decltype(a)>(a)
```

## 6.2 示例：保留值类别

```cpp
#include <iostream>
#include <utility>

void consume(int&) {
    std::cout << "consume(int&)\n";
}

void consume(const int&) {
    std::cout << "consume(const int&)\n";
}

void consume(int&&) {
    std::cout << "consume(int&&)\n";
}

int main() {
    auto no_forward = [](auto&& x) {
        consume(x);
    };

    auto with_forward = [](auto&& x) {
        consume(std::forward<decltype(x)>(x));
    };

    int a = 1;

    std::cout << "no_forward(a): ";
    no_forward(a); // consume(int&)

    std::cout << "no_forward(std::move(a)): ";
    no_forward(std::move(a)); // 仍然 consume(int&)，因为 x 是具名变量

    std::cout << "with_forward(a): ";
    with_forward(a); // consume(int&)

    std::cout << "with_forward(std::move(a)): ";
    with_forward(std::move(a)); // consume(int&&)
}
```

> [!summary]
> 在 `std::bind` 转 lambda 时，`auto&&` 负责以泛型方式接收参数；`std::forward<decltype(x)>(x)` 负责把参数按照原来的左值或右值身份继续传给目标函数。

---

# 7. 常见转换场景汇总

## 7.1 转换对照表

| std::bind 写法 | Lambda 写法 |
|---|---|
| `std::bind(foo, _1)` | `[](auto&& x) -> decltype(auto) { return foo(std::forward<decltype(x)>(x)); }` |
| `std::bind(foo, _2, _1)` | `[](auto&& x, auto&& y) -> decltype(auto) { return foo(std::forward<decltype(y)>(y), std::forward<decltype(x)>(x)); }` |
| `std::bind(foo, value, _1)` | `[value](auto&& x) mutable -> decltype(auto) { return foo(value, std::forward<decltype(x)>(x)); }` |
| `std::bind(foo, std::ref(value), _1)` | `[&value](auto&& x) -> decltype(auto) { return foo(value, std::forward<decltype(x)>(x)); }` |
| `std::bind(&T::func, &obj, _1)` | `[&obj](auto&& x) -> decltype(auto) { return obj.func(std::forward<decltype(x)>(x)); }` |
| `std::bind(&T::func, _1, _2)` | `[](auto&& obj, auto&& x) -> decltype(auto) { return std::invoke(&T::func, std::forward<decltype(obj)>(obj), std::forward<decltype(x)>(x)); }` |
| `std::bind(foo, std::bind(bar, _1), _2)` | `[](auto&& x, auto&& y) -> decltype(auto) { return foo(bar(std::forward<decltype(x)>(x)), std::forward<decltype(y)>(y)); }` |

## 7.2 完整综合示例

下面例子把普通函数、参数重排、固定值、引用绑定、成员函数和 lambda 转换放在一起演示。

```cpp
#include <functional>
#include <iostream>
#include <string>
#include <utility>

struct Player {
    explicit Player(std::string name) : name(std::move(name)) {}

    int addScore(int delta) {
        score += delta;
        return score;
    }

    std::string name;
    int score = 0;
};

int makeScore(int base, int bonus, int multiplier) {
    return (base + bonus) * multiplier;
}

void printPlayer(const Player& player, const std::string& label) {
    std::cout << label << ": " << player.name
              << ", score = " << player.score << '\n';
}

int main() {
    using namespace std::placeholders;

    int base = 10;

    // 1. bind：固定 bonus = 5，调用参数 _1 作为 multiplier。
    auto bind_score = std::bind(makeScore, std::ref(base), 5, _1);

    // 2. lambda：等价写法。
    auto lambda_score = [&base](auto&& multiplier) -> decltype(auto) {
        return makeScore(
            base,
            5,
            std::forward<decltype(multiplier)>(multiplier)
        );
    };

    std::cout << bind_score(2) << '\n';   // (10 + 5) * 2 = 30
    std::cout << lambda_score(2) << '\n'; // (10 + 5) * 2 = 30

    base = 20;

    std::cout << bind_score(2) << '\n';   // (20 + 5) * 2 = 50
    std::cout << lambda_score(2) << '\n'; // (20 + 5) * 2 = 50

    Player alice("Alice");

    // 3. bind：绑定成员函数到具体对象 alice。
    auto bind_add = std::bind(&Player::addScore, &alice, _1);

    // 4. lambda：等价写法。
    auto lambda_add = [&alice](auto&& delta) -> decltype(auto) {
        return alice.addScore(std::forward<decltype(delta)>(delta));
    };

    bind_add(10);
    lambda_add(5);

    // 5. bind：参数重排。
    auto bind_print = std::bind(printPlayer, _2, _1);

    // 6. lambda：等价写法。
    auto lambda_print = [](auto&& label, auto&& player) -> decltype(auto) {
        return printPlayer(
            std::forward<decltype(player)>(player),
            std::forward<decltype(label)>(label)
        );
    };

    bind_print("bind", alice);
    lambda_print("lambda", alice);
}
```

---

# 8. 易错点与最佳实践

## 8.1 不要忘记 return

`std::bind` 的调用结果是内部目标函数调用的结果。

```cpp
auto f = std::bind(foo, _1, 42, _2);
```

调用：

```cpp
auto result = f(a, b);
```

语义接近：

```cpp
auto result = foo(a, 42, b);
```

因此转换成 lambda 时通常要写：

```cpp
return foo(...);
```

如果写成：

```cpp
foo(...);
```

那么 lambda 通常返回 `void`，不再等价。

## 8.2 注意值捕获与引用捕获

`std::bind(foo, x, _1)` 对应值捕获：

```cpp
[x](auto&& p) mutable -> decltype(auto) {
    return foo(x, std::forward<decltype(p)>(p));
}
```

`std::bind(foo, std::ref(x), _1)` 对应引用捕获：

```cpp
[&x](auto&& p) -> decltype(auto) {
    return foo(x, std::forward<decltype(p)>(p));
}
```

两者的生命周期风险不同：

- 值捕获保存副本，通常更安全。
- 引用捕获不拥有对象，必须保证被引用对象仍然存活。

## 8.3 注意 this 的生命周期

类内部写：

```cpp
return [this](auto&& delta) {
    return addScore(std::forward<decltype(delta)>(delta));
};
```

表示 lambda 保存当前对象指针。如果对象销毁后 lambda 仍被调用，会访问悬垂 `this`。

如果需要让 lambda 在对象销毁后仍安全，C++17 起可以考虑按值捕获当前对象副本：

```cpp
return [*this](auto&& delta) mutable {
    return addScore(std::forward<decltype(delta)>(delta));
};
```

但这会操作副本，不会再修改原对象。

## 8.4 注意 std::bind 会忽略多余参数

`std::bind(foo, _1)` 可以被这样调用：

```cpp
f(1, 2, 3);
```

lambda 如果想保留这种兼容性，需要写：

```cpp
[](auto&& x, auto&&...) -> decltype(auto) {
    return foo(std::forward<decltype(x)>(x));
}
```

## 8.5 注意重载函数

如果目标函数有重载，`std::bind` 和 lambda 都可能需要明确指定目标函数类型。

```cpp
#include <functional>
#include <iostream>

void print(int x) {
    std::cout << "int: " << x << '\n';
}

void print(double x) {
    std::cout << "double: " << x << '\n';
}

int main() {
    using namespace std::placeholders;

    // print 有重载，直接 std::bind(print, _1) 可能无法确定要绑定哪一个。
    auto bind_f = std::bind(static_cast<void(*)(int)>(&print), _1);

    // lambda 可以在函数体中通过参数类型帮助重载决议。
    auto lambda_f = [](int x) {
        print(x);
    };

    bind_f(10);
    lambda_f(10);
}
```

## 8.6 现代 C++ 中优先使用 Lambda

多数情况下，lambda 比 `std::bind` 更推荐：

| 对比点 | std::bind | Lambda |
|---|---|---|
| 可读性 | 参数关系隐藏在占位符中 | 直接写出调用逻辑 |
| 调试体验 | 类型复杂，不易读 | 更接近普通函数 |
| 返回类型控制 | 由 bind 规则决定 | 可以显式 `-> decltype(auto)` |
| 捕获语义 | 普通参数默认衰减保存，引用要用 `std::ref` | `[x]`、`[&x]` 直接表达 |
| 成员函数 | 需要成员函数指针和对象参数 | 可直接 `obj.func(...)` |
| 多余参数 | 默认忽略 | 需要显式 `auto&&...` |

> [!summary]
> 新代码优先写 lambda。维护老代码时，可以先把 `std::bind` 展开为“实际调用形式”，再按规则转换成 lambda。

---

# 9. 参考资料

- [cppreference: std::bind](https://en.cppreference.com/cpp/utility/functional/bind)
- [cppreference: std::placeholders](https://en.cppreference.com/cpp/utility/functional/placeholders)
- [cppreference: Lambda expressions](https://en.cppreference.com/cpp/language/lambda)
- [cppreference: std::forward](https://en.cppreference.com/cpp/utility/forward)
- [cppreference: Reference declaration and forwarding references](https://en.cppreference.com/cpp/language/reference)
- [cppreference: std::invoke](https://en.cppreference.com/w/cpp/utility/functional/invoke)
