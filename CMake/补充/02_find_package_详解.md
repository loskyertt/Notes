---
title: 谈谈 CMake 中的 find_package
date: 2024-12-16 13:13:17
excerpt: "本文讲述 CMake 中的 find_package 的原理和使用方式。"
categories: "CMake教程"
---


# 1.find_package 说明

`find_package` 是 CMake 中用于 **查找外部库或包** 的命令。它的主要作用是帮助 CMake 找到第三方库、工具或模块，并导入它们所需的信息（如库路径、头文件路径、链接依赖关系等），以便项目可以顺利进行编译和链接。

## 1.1 工作原理

`find_package` 会在系统指定的路径中查找某个包，并加载与该包相关的信息。可以通过下面这种方式查看系统路径：

```cmake
foreach(path ${CMAKE_SYSTEM_PREFIX_PATH})
  message(STATUS "${path}")
endforeach()
```

会输出：

```
-- /usr/local
-- /usr
-- /
-- /usr
-- /usr/local
-- /usr/X11R6
-- /usr/pkg
-- /opt
```

查找包时，它主要依赖以下机制：

1. **查找路径**
CMake 会按顺序查找以下路径来寻找包：
   1. 用户指定的路径：通过 `CMAKE_PREFIX_PATH` 或 `PATHS` 参数指定的路径。
   2. 默认路径：
     - 系统路径。
     - 环境变量，如 `CMAKE_MODULE_PATH` 和 `CMAKE_FIND_ROOT_PATH`。
   3. CMake 模块路径：CMake 内置的模块搜索路径。
   4. 自定义的查找脚本：
     - 自定义的 `<package>Config.cmake` 文件。
     - `Find<Package>.cmake` 模块文件。

CMake 按照这些路径的顺序进行搜索，找到第一个匹配的结果即停止。

2. **工作模式**
`find_package` 有两种主要工作模式：
   1. Config 模式（配置文件模式）
      查找 `<Package>Config.cmake` 或 `<lowercase-package>-config.cmake` 文件。该模式通常用于现代 CMake 兼容的库或第三方项目，它们会生成配置文件供 CMake 使用。

   2. Module 模式（模块文件模式）  
      查找 `Find<Package>.cmake` 文件。此模式通常用于 CMake 自带的模块文件或用户定义的模块文件。

3. **工作流程**
当你调用 `find_package(<Package> REQUIRED)` 时：
   1. 首先查找 `<Package>Config.cmake` 配置文件。
   2. 如果找不到，再查找 `Find<Package>.cmake` 模块文件。
   3. 成功找到后，CMake 会设置一系列变量（如 `<Package>_INCLUDE_DIRS`、`<Package>_LIBRARIES` 等），这些变量可以在后续的 `target_include_directories` 和 `target_link_libraries` 中使用。
   4. 如果添加 `REQUIRED` 选项但未找到包，CMake 将报错。

## 1.2 基本语法

```cmake
find_package(<PackageName> [version] [EXACT] [QUIET] [REQUIRED] [MODULE])
```

参数说明：
1. `<PackageName>`：要查找的包名。
2. `[version]`：指定包的版本号，例如 `1.2.3`。
3. `EXACT`：要求找到的版本必须完全匹配指定的版本号。
4. `QUIET`：找到包时不输出查找过程的日志信息。
5. `REQUIRED`：如果未找到包，CMake 将终止配置并报错。
6. `MODULE`：强制 CMake 只查找 `Find<Package>.cmake` 模块文件，而不查找 `<Package>Config.cmake`。

## 1.3 使用 find_package 的结果

`find_package` 成功找到包后，通常会设置一些变量，用户可以使用这些变量来链接库、包含头文件等。

常见的变量包括：
- `<Package>_FOUND`：是否找到包，值为 `TRUE` 或 `FALSE`。
- `<Package>_INCLUDE_DIRS`：包的头文件目录。
- `<Package>_LIBRARIES`：包的库文件路径。
- `<Package>_VERSION`：找到的包的版本号。

---

# 2.注意点

在 CMake 中，`find_package` 对包名是 **大小写敏感** 的，包名必须与库提供的 `<Package>Config.cmake` 文件名、模块文件名（如 `Find<Package>.cmake`）中的定义 **完全匹配** 。

在不确定大小写时，可以通过查找库的 **CMake 配置文件** 方式来确定，通常位于库的安装路径下：

```bash
find /usr /opt -name "*Config.cmake"
```

例如，找到 `/usr/lib/cmake/Qt5/Qt5Config.cmake`，说明包名是 `Qt5`。
