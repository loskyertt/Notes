---
title: CMake find_package 详解
date: 2026-07-14
tags:
  - CMake
  - Cpp
  - BuildSystem
  - ThirdParty
aliases:
  - find_package
  - CMake Config Package
  - Find Module
---

# 1. 核心结论

`find_package()` 是 CMake 用来接入第三方库的入口。它本身不等于“链接库”，而是负责**找到一个包，并加载这个包提供的 CMake 信息**。

加载成功后，现代 CMake 最推荐的结果是得到一个 **namespaced imported target**，例如：

```cmake
find_package(jsoncpp CONFIG REQUIRED)
target_link_libraries(app PRIVATE JsonCpp::JsonCpp)
```

`find_package()` 其实是“像是高级版 include”。但区别是使用 `include(...)` 时，你需要明确告诉 CMake 加载哪个 `.cmake` 文件；使用 `find_package(...)` 使，你只需要告诉 CMake 你要找哪个包，至于去哪找、找哪个文件、版本是否满足、是否 REQUIRED、找到后暴露哪些 target，由 `find_package` 的规则和包配置决定。

> [!summary]
> **`xxxConfig.cmake` 跟着库走，`Findxxx.cmake` 跟着项目走。**
>
> - 已经打包好的库，优先提供 `xxxConfig.cmake`
> - 不知道库装在哪里，需要到系统里搜索时，才写 `Findxxx.cmake`
> - 使用方优先链接 `Xxx::Xxx` 这种带命名空间的 target

---

# 2. find_package 的两种模式

## 2.1 Config 模式

Config 模式查找的是包自己提供的配置文件：

```text
<PackageName>Config.cmake
<package-name>-config.cmake
```

典型安装结构：

```text
jsoncpp-1.9.8/
├── include/
│   └── json/
├── lib/
│   ├── libjsoncpp.a
│   └── cmake/
│       └── jsoncpp/
│           ├── jsoncppConfig.cmake
│           └── jsoncppConfigVersion.cmake
```

使用方：

```cmake
find_package(jsoncpp CONFIG REQUIRED)
target_link_libraries(app PRIVATE JsonCpp::JsonCpp)
```

配置时告诉 CMake 安装前缀：

```bash
cmake -S . -B build -DCMAKE_PREFIX_PATH=/path/to/jsoncpp-1.9.8
```

或者直接指定配置文件目录：

```bash
cmake -S . -B build -Djsoncpp_DIR=/path/to/jsoncpp-1.9.8/lib/cmake/jsoncpp
```

## 2.2 Module 模式

Module 模式查找的是使用方项目或 CMake 内置模块提供的文件：

```text
Find<PackageName>.cmake
```

典型项目结构：

```text
my-app/
├── CMakeLists.txt
├── main.cpp
└── cmake/
    └── FindJsonCpp.cmake
```

使用方：

```cmake
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

find_package(JsonCpp REQUIRED)
target_link_libraries(app PRIVATE JsonCpp::JsonCpp)
```

`FindJsonCpp.cmake` 的职责是到系统中搜索：

```text
json/json.h
libjsoncpp.a
libjsoncpp.so
jsoncpp.lib
```

然后把搜索结果包装成 `JsonCpp::JsonCpp`。

## 2.3 两种模式的关系

`xxxConfig.cmake` 不是简单替代 `Findxxx.cmake`，它们面向不同场景。

| 文件 | 所属方 | 主要职责 | 典型位置 |
|---|---|---|---|
| `xxxConfig.cmake` | 库作者或打包者 | 描述这个库如何被使用 | `lib/cmake/xxx/` |
| `Findxxx.cmake` | 使用方项目或 CMake | 搜索一个未知位置的库 | 项目的 `cmake/` 目录或 CMake 模块目录 |

> [!tip]
> 可以把 `xxxConfig.cmake` 理解成“包的自我介绍”（放到具体的某一个包的目录中），把 `Findxxx.cmake` 理解成“使用方写的寻包脚本”（放到自己项目的 `cmake` 目录中）。

---

# 3. 具体项目结构对比

## 3.1 场景一：自己重打包 jsoncpp

假设项目结构如下：

```text
my-app/
├── CMakeLists.txt
├── main.cpp
└── 3rdparty/
    └── jsoncpp-1.9.8/
        ├── include/
        │   └── json/
        │       └── json.h
        └── lib/
            ├── libjsoncpp.a
            └── cmake/
                └── jsoncpp/
                    └── jsoncppConfig.cmake
```

这里 `jsoncpp` 的位置和目录结构都是确定的，因此应该提供 `jsoncppConfig.cmake`，而不是写 `Findjsoncpp.cmake`。

`jsoncppConfig.cmake`：

```cmake
get_filename_component(_jsoncpp_root
    "${CMAKE_CURRENT_LIST_DIR}/../../.."
    ABSOLUTE
)

if(NOT TARGET JsonCpp::JsonCpp)
    add_library(JsonCpp::JsonCpp STATIC IMPORTED)

    set_target_properties(JsonCpp::JsonCpp PROPERTIES
        IMPORTED_LOCATION "${_jsoncpp_root}/lib/libjsoncpp.a"
        INTERFACE_INCLUDE_DIRECTORIES "${_jsoncpp_root}/include"
    )
endif()

unset(_jsoncpp_root)
```

`unset(_jsoncpp_root)` 的作用是：**把前面临时创建的 CMake 变量 `_jsoncpp_root` 删除掉，避免它污染后续作用域**。

> [!question] 为什么要清掉？
>
> 因为 `.cmake` 文件被 `include()` 或 `find_package()` 加载时，里面设置的普通变量通常会留在调用方当前作用域里。也就是说，如果不 `unset`，外层 `CMakeLists.txt` 后面仍然可能看到，比如在外层 `CMakeLists.txt` 中写：
> 
> ```cmake
> message("${_jsoncpp_root}")
> ```
>
> 这不是严重错误，但会让临时变量泄露出去。尤其配置文件、工具脚本、`FindXXX.cmake` 里经常会定义很多辅助变量，所以习惯上用完就清理。
> **补充**：`_jsoncpp_root` 前面的下划线也表达了“这是内部临时变量”的意思，但下划线只是命名约定，不会自动隐藏变量。 

使用方 `CMakeLists.txt`：

```cmake
cmake_minimum_required(VERSION 3.20)
project(MyApp)

add_executable(app main.cpp)

find_package(jsoncpp CONFIG REQUIRED)

target_link_libraries(app PRIVATE JsonCpp::JsonCpp)
```

配置命令：

```bash
cmake -S . -B build -DCMAKE_PREFIX_PATH=./3rdparty/jsoncpp-1.9.8
```

也可以在 `find_package` 前添加：

```cmake
list(APPEND CMAKE_PREFIX_PATH "${CMAKE_SOURCE_DIR}/3rdparty/jsoncpp-1.9.8")
```

这样在适用 cmake 构建时，就不需要指定参数 `-DCMAKE_PREFIX_PATH=./3rdparty/jsoncpp-1.9.8` 了。

---

## 3.2 场景二：不知道 jsoncpp 安装在哪里

假设项目结构如下：

```text
my-app/
├── CMakeLists.txt
├── main.cpp
└── cmake/
    └── FindJsonCpp.cmake
```

而 jsoncpp 可能安装在任意位置：

```text
/usr/include/json/json.h
/usr/lib/libjsoncpp.so

/usr/local/include/json/json.h
/usr/local/lib/libjsoncpp.a

D:/libs/jsoncpp/include/json/json.h
D:/libs/jsoncpp/lib/jsoncpp.lib
```

这时 `FindJsonCpp.cmake` 才有意义：

```cmake
find_path(JsonCpp_INCLUDE_DIR
    NAMES json/json.h
)

find_library(JsonCpp_LIBRARY
    NAMES jsoncpp libjsoncpp
)

include(FindPackageHandleStandardArgs)

find_package_handle_standard_args(JsonCpp
    REQUIRED_VARS
        JsonCpp_INCLUDE_DIR
        JsonCpp_LIBRARY
)

if(JsonCpp_FOUND AND NOT TARGET JsonCpp::JsonCpp)
    add_library(JsonCpp::JsonCpp UNKNOWN IMPORTED)

    set_target_properties(JsonCpp::JsonCpp PROPERTIES
        IMPORTED_LOCATION "${JsonCpp_LIBRARY}"
        INTERFACE_INCLUDE_DIRECTORIES "${JsonCpp_INCLUDE_DIR}"
    )
endif()
```

使用方 `CMakeLists.txt`：

```cmake
cmake_minimum_required(VERSION 3.20)
project(MyApp)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

find_package(JsonCpp REQUIRED)

add_executable(app main.cpp)
target_link_libraries(app PRIVATE JsonCpp::JsonCpp)
```

此时 `FindJsonCpp.cmake` 的核心价值是：

- 搜索头文件
- 搜索库文件
- 检查是否找到
- 包装成 imported target

`find_path` 并**不是从系统根目录开始递归全盘搜索**，而是在 CMake 预设的一组“候选前缀/候选目录”里找。官方文档说 `find_path()` 用来找“包含指定文件的目录”，并且会使用默认搜索路径、`<PackageName>_ROOT`、`CMAKE_PREFIX_PATH`、系统 include 路径等。见 [`find_path`](https://cmake.org/cmake/help/latest/command/find_path.html)。

```cmake
include(FindPackageHandleStandardArgs)

find_package_handle_standard_args(JsonCpp
    REQUIRED_VARS
        JsonCpp_INCLUDE_DIR
        JsonCpp_LIBRARY
)
```

这部分是 CMake 提供的标准辅助模块，专门给 `FindXXX.cmake` 用。它会帮你做几件事：

1. 检查 JsonCpp_INCLUDE_DIR 是否有效
2. 检查 JsonCpp_LIBRARY 是否有效
3. 设置 JsonCpp_FOUND
4. 正确处理 find_package(JsonCpp REQUIRED)
5. 正确处理 find_package(JsonCpp QUIET)
6. 输出标准化的成功/失败信息

官方文档说这个模块就是给 Find Modules 实现 `find_package()` 调用时用的，并处理 `REQUIRED`、`QUIET`、版本等逻辑。见 [`FindPackageHandleStandardArgs`](https://cmake.org/cmake/help/latest/module/FindPackageHandleStandardArgs.html)。

---

# 4. find_package 的选择规则

## 4.1 明确使用 Config 模式

如果只想使用包自带的配置文件：

```cmake
find_package(jsoncpp CONFIG REQUIRED)
```

这会查找：

```text
jsoncppConfig.cmake
jsoncpp-config.cmake
```

这种写法适合现代 CMake 和自己打包的第三方库。

## 4.2 明确使用 Module 模式

如果只想使用 `Findxxx.cmake`：

```cmake
find_package(JsonCpp MODULE REQUIRED)
```

这会查找：

```text
FindJsonCpp.cmake
```

## 4.3 不指定模式时

常见写法：

```cmake
find_package(JsonCpp REQUIRED)
```

它使用 `find_package` 的基本签名。通常会先尝试 Module 模式，再尝试 Config 模式；如果设置了：

```cmake
set(CMAKE_FIND_PACKAGE_PREFER_CONFIG TRUE)
```

则会优先尝试 Config 模式。

> [!warning]
> 如果你已经明确要使用自己打包的 `xxxConfig.cmake`，建议直接写 `CONFIG`，减少搜索路径和大小写带来的歧义。

---

# 5. imported target 的类型

## 5.1 INTERFACE IMPORTED

`INTERFACE IMPORTED` 表示：

```text
这个 target 没有自己的库文件，只传播使用要求。
```

适合 header-only 库。

目录结构：

```text
fmt-header-only/
└── include/
    └── fmt/
        └── format.h
```

配置文件：

```cmake
get_filename_component(_fmt_root
    "${CMAKE_CURRENT_LIST_DIR}/../../.."
    ABSOLUTE
)

if(NOT TARGET fmt::fmt)
    add_library(fmt::fmt INTERFACE IMPORTED)

    set_target_properties(fmt::fmt PROPERTIES
        INTERFACE_INCLUDE_DIRECTORIES "${_fmt_root}/include"
    )
endif()

unset(_fmt_root)
```

使用方：

```cmake
target_link_libraries(app PRIVATE fmt::fmt)
```

这里虽然使用了 `target_link_libraries()`，但不会链接真实库文件，只会继承 `fmt::fmt` 的 include 目录、编译宏、编译选项等使用要求。

## 5.2 STATIC IMPORTED

`STATIC IMPORTED` 表示：

```text
这个 target 对应一个外部已经存在的静态库文件。
```

适合：

```text
libjsoncpp.a
jsoncpp.lib
```

示例：

```cmake
add_library(JsonCpp::JsonCpp STATIC IMPORTED)

set_target_properties(JsonCpp::JsonCpp PROPERTIES
    IMPORTED_LOCATION "${JSONCPP_ROOT}/lib/libjsoncpp.a"
    INTERFACE_INCLUDE_DIRECTORIES "${JSONCPP_ROOT}/include"
)
```

使用方链接后，CMake 会做两件事：

```text
编译阶段：加入 jsoncpp 的 include 目录
链接阶段：链接 libjsoncpp.a
```

## 5.3 SHARED IMPORTED

`SHARED IMPORTED` 表示：

```text
这个 target 对应一个外部已经存在的动态库文件。
```

Linux 示例：

```text
jsoncpp-1.9.8/
├── include/
└── lib/
    └── libjsoncpp.so
```

配置文件：

```cmake
add_library(JsonCpp::JsonCpp SHARED IMPORTED)

set_target_properties(JsonCpp::JsonCpp PROPERTIES
    IMPORTED_LOCATION "${JSONCPP_ROOT}/lib/libjsoncpp.so"
    INTERFACE_INCLUDE_DIRECTORIES "${JSONCPP_ROOT}/include"
)
```

Windows 示例：

```text
jsoncpp-1.9.8/
├── include/
├── bin/
│   └── jsoncpp.dll
└── lib/
    └── jsoncpp.lib
```

配置文件：

```cmake
add_library(JsonCpp::JsonCpp SHARED IMPORTED)

set_target_properties(JsonCpp::JsonCpp PROPERTIES
    IMPORTED_LOCATION "${JSONCPP_ROOT}/bin/jsoncpp.dll"
    IMPORTED_IMPLIB "${JSONCPP_ROOT}/lib/jsoncpp.lib"
    INTERFACE_INCLUDE_DIRECTORIES "${JSONCPP_ROOT}/include"
)
```

## 5.4 UNKNOWN IMPORTED

`UNKNOWN IMPORTED` 表示这个 target 有真实库文件，但当前 Find 模块不确定它是静态库还是动态库，你先把它当作可链接的 imported library 使用。

> 官方文档也明确说：`UNKNOWN` 通常用于 Find Modules，尤其适合 `find_library()` 找到一个库文件，但不想或不能判断其类型的情况。见 [`add_library` imported libraries](https://cmake.org/cmake/help/latest/command/add_library.html)。

这在 `Findxxx.cmake` 中比较常见，因为 `find_library()` 搜到的可能是：

```text
libxxx.a
libxxx.so
xxx.lib
```

示例：

```cmake
add_library(JsonCpp::JsonCpp UNKNOWN IMPORTED)

set_target_properties(JsonCpp::JsonCpp PROPERTIES
    IMPORTED_LOCATION "${JsonCpp_LIBRARY}"
    INTERFACE_INCLUDE_DIRECTORIES "${JsonCpp_INCLUDE_DIR}"
)
```

> [!tip]
> 自己写 `xxxConfig.cmake` 时，通常知道库的真实类型，因此优先写 `STATIC IMPORTED` 或 `SHARED IMPORTED`。只有不确定类型时才用 `UNKNOWN IMPORTED`。

## 5.5 区别

STATIC、SHARED 与 INTERFACE 的区别：

| 类型 | 是否有真实库文件 | 是否需要 `IMPORTED_LOCATION` | 典型场景 |
|---|---:|---:|---|
| `INTERFACE IMPORTED` | 否 | 否 | header-only 库、只传播编译选项的库 |
| `STATIC IMPORTED` | 是 | 是 | 已编译好的 `.a` 或静态 `.lib` |
| `SHARED IMPORTED` | 是 | 是 | 已编译好的 `.so`、`.dylib`、`.dll` |
| `UNKNOWN IMPORTED` | 是 | 是 | Find 模块中不确定库类型 |

关系可以理解为：

```text
INTERFACE = 说明书 target
STATIC    = 静态库文件 + 说明书 target
SHARED    = 动态库文件 + 说明书 target
UNKNOWN   = 未确认类型的库文件 + 说明书 target
```

---

# 6. GLOBAL

## 6.1 默认作用域

普通 imported target 默认有目录作用域。

例如：

```text
my-app/
├── CMakeLists.txt
├── deps/
│   └── CMakeLists.txt
├── app/
│   └── CMakeLists.txt
└── tests/
    └── CMakeLists.txt
```

如果在 `deps/CMakeLists.txt` 中写：

```cmake
add_library(JsonCpp::JsonCpp STATIC IMPORTED)
```

这个 target 默认只在 `deps/` 目录及其子目录可见，兄弟目录 `app/` 和 `tests/` 不一定能直接看到。

## 6.2 GLOBAL 作用域

如果写成：

```cmake
add_library(JsonCpp::JsonCpp STATIC IMPORTED GLOBAL)
```

则表示：

```text
JsonCpp::JsonCpp 在整个 CMake 工程中全局可见。
```

示例：

```cmake
add_library(JsonCpp::JsonCpp STATIC IMPORTED GLOBAL)

set_target_properties(JsonCpp::JsonCpp PROPERTIES
    IMPORTED_LOCATION "${CMAKE_SOURCE_DIR}/3rdparty/jsoncpp-1.9.8/lib/libjsoncpp.a"
    INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_SOURCE_DIR}/3rdparty/jsoncpp-1.9.8/include"
)
```

之后兄弟目录都可以使用：

```cmake
target_link_libraries(app PRIVATE JsonCpp::JsonCpp)
target_link_libraries(test_jsoncpp PRIVATE JsonCpp::JsonCpp)
```

> [!warning]
> 普通 `xxxConfig.cmake` 里通常不必主动加 `GLOBAL`。只有当你在某个子目录集中创建 imported target，并希望整个工程的兄弟目录都能看到它时，才考虑使用 `GLOBAL`。

---

# 7. 静态库与动态库

## 7.1 静态库

静态库常见文件：

```text
Linux/macOS: libxxx.a
Windows:     xxx.lib
```

特点：**链接器从 `.a` 中只抽取本服务实际引用的 `.o` 目标文件，未引用的部分不会进入最终可执行文件。因此多个服务链接同一个 `.a` 不会互相干扰，也不会造成可执行文件膨胀**。

优点：

- 部署简单
- 不依赖额外 `.so` 或 `.dll`
- 不容易运行时找错库

缺点：

- 可执行文件可能更大
- 多个程序使用同一库时，每个程序可能各带一份
- 库升级后通常需要重新链接程序

## 7.2 动态库

动态库常见文件：

```text
Linux:   libxxx.so
macOS:   libxxx.dylib
Windows: xxx.dll
```

特点：

- 链接阶段，可执行文件记录对动态库的依赖。
- 运行阶段，操作系统加载器把动态库映射到进程地址空间。

优点：

- 多个程序可以共享同一个动态库
- 主程序体积较小
- 库可以单独升级
- 适合插件系统和大型软件

缺点：

- 部署更复杂
- 运行时必须能找到正确版本的动态库
- 存在 ABI 兼容和版本冲突问题

## 7.3 常见误解

> [!warning]
> 静态库不是一定把整个库都塞进可执行文件；通常是按目标文件粒度链接需要的部分。
>
> 动态库也不是严格只加载“此刻正在调用的函数模块”；操作系统会把动态库映射到进程地址空间，物理内存页通常按需加载，并且只读代码页可以被多个进程共享。

---

# 8. 推荐实践

## 8.1 个人重打包第三方库

优先选择：

```text
xxxConfig.cmake
```

推荐提供 `Xxx::Xxx` 这种 namespaced imported target，主要是套层命名空间，避免 target 冲突。

示例：

```cmake
find_package(jsoncpp CONFIG REQUIRED)
target_link_libraries(app PRIVATE JsonCpp::JsonCpp)
```

## 8.2 不推荐直接暴露普通 target 名

不推荐：

```cmake
add_library(jsoncpp INTERFACE)
add_library(JsonCpp::JsonCpp ALIAS jsoncpp)
```

因为这样使用方既可以写：

```cmake
target_link_libraries(app PRIVATE JsonCpp::JsonCpp)
```

也可以写：

```cmake
target_link_libraries(app PRIVATE jsoncpp)
```

如果只想支持 `JsonCpp::JsonCpp`，应该直接创建 imported target：

```cmake
add_library(JsonCpp::JsonCpp STATIC IMPORTED)
```

不要额外创建真实的 `jsoncpp` target。

## 8.3 使用方式选择表

| 场景 | 推荐文件 | 推荐 target 类型 |
|---|---|---|
| 自己打包一个固定安装目录的库 | `xxxConfig.cmake` | `STATIC` 或 `SHARED IMPORTED` |
| 上游库已经提供 CMake 包 | 上游的 `xxxConfig.cmake` | 使用上游 target |
| 库只有头文件 | `xxxConfig.cmake` | `INTERFACE IMPORTED` |
| 需要在未知系统路径中搜索库 | `Findxxx.cmake` | `UNKNOWN IMPORTED` 或探测后指定类型 |
| 某子目录创建 target，兄弟目录也要用 | 视情况而定 | `IMPORTED GLOBAL` |

---

# 9. 模板

## 9.1 静态库 Config 模板

```cmake
get_filename_component(_pkg_root
    "${CMAKE_CURRENT_LIST_DIR}/../../.."
    ABSOLUTE
)

if(NOT TARGET Xxx::Xxx)
    add_library(Xxx::Xxx STATIC IMPORTED)

    set_target_properties(Xxx::Xxx PROPERTIES
        IMPORTED_LOCATION "${_pkg_root}/lib/libxxx.a"
        INTERFACE_INCLUDE_DIRECTORIES "${_pkg_root}/include"
    )
endif()

unset(_pkg_root)
```

## 9.2 动态库 Config 模板

```cmake
get_filename_component(_pkg_root
    "${CMAKE_CURRENT_LIST_DIR}/../../.."
    ABSOLUTE
)

if(NOT TARGET Xxx::Xxx)
    add_library(Xxx::Xxx SHARED IMPORTED)

    set_target_properties(Xxx::Xxx PROPERTIES
        IMPORTED_LOCATION "${_pkg_root}/lib/libxxx.so"
        INTERFACE_INCLUDE_DIRECTORIES "${_pkg_root}/include"
    )
endif()

unset(_pkg_root)
```

## 9.3 Header-only Config 模板

```cmake
get_filename_component(_pkg_root
    "${CMAKE_CURRENT_LIST_DIR}/../../.."
    ABSOLUTE
)

if(NOT TARGET Xxx::Xxx)
    add_library(Xxx::Xxx INTERFACE IMPORTED)

    set_target_properties(Xxx::Xxx PROPERTIES
        INTERFACE_INCLUDE_DIRECTORIES "${_pkg_root}/include"
    )
endif()

unset(_pkg_root)
```

