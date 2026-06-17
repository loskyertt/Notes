---
title: 解决 VSCode 下使用 gdb 调试 C++ 出现的问题
date: 2024-11-13 20:43:01
excerpt: "注意：编程环境是 Linux 或者 WSL 子系统。发行版是 Arch，在 Ubuntu 下没有这个问题。"
categories: "C++教程"
---


# 1.问题复现

如下图所示，这是调试时出现的问题：
[![step1](https://s21.ax1x.com/2024/11/13/pAgMhxf.md.png)](https://imgse.com/i/pAgMhxf)

[![step2](https://s21.ax1x.com/2024/11/13/pAgMHaj.md.png)](https://imgse.com/i/pAgMHaj)

从图中可以发现，在进行单步调试时，先进入`ostream`，但是失败了，因为它找的是错误的路径，这个路径`/usr/src/debug/gcc/gcc-build/x86_64-pc-linux-gnu/libstdc++-v3/include`根本不存在。

---

# 2.解决办法

配置`~/.gdbinit`文件即可。

```bash
nano ~/.gdbinit
```

加入下面的内容：
```bash
set directories /usr/include/c++/14.2.1
set substitute-path /usr/src/debug/gcc/gcc-build/x86_64-pc-linux-gnu/libstdc++-v3/include /usr/include/c++/14.2.1
```

然后就解决了，效果如下：
[![step3](https://s21.ax1x.com/2024/11/13/pAgMXR0.md.png)](https://imgse.com/i/pAgMXR0)

---

# 3.说明

1. **`set directories /usr/include/c++/14.2.1`** ：
   这个命令告诉 GDB 在 `/usr/include/c++/14.2.1` 目录下搜索头文件。这个目录通常是系统上安装的 C++ 标准库头文件的位置。当你请求 GDB 显示源代码或者 GDB 需要查找源代码以进行调试时，它会首先查看这个目录。

3. **`set substitute-path /usr/src/debug/gcc/gcc-build/x86_64-pc-linux-gnu/libstdc++-v3/include /usr/include/c++/14.2.1`** ：
   这个命令是一个路径替换指令，它将 GDB 查找源代码时遇到的一个路径映射到另一个路径。具体来说，当 GDB 尝试在 `/usr/src/debug/gcc/gcc-build/x86_64-pc-linux-gnu/libstdc++-v3/include` 目录下查找文件时，它会将其重定向到 `/usr/include/c++/14.2.1` 目录。这通常用于解决源代码和编译后的二进制文件不在同一位置的问题，或者当你想要 GDB 从不同的位置加载源代码时。