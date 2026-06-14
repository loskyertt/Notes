# Notes

个人学习笔记仓库，记录学习过程中的知识点与心得。记笔记是个好习惯——哪怕长时间没有复习后会遗忘具体内容，但仍然知道自己学过某个概念，能想到在哪里重新复习，或者遇到问题时知道怎么查找解决方法。

起初想把这些文档整理成博客，之前搭建过 Hexo 博客，但更新起来比较麻烦，而且数学公式渲染有问题，就干脆存到本地、GitHub 和 Gitee 仓库中。本地可以用 Obsidian 进行浏览。

---

## 目录

- [获取方式](#获取方式)
- [阅读方式](#阅读方式)
- [目录导航](#目录导航)
  - [AI](#ai)
  - [C++](#c)
  - [CMake](#cmake)
  - [Docker](#docker)
  - [Git](#git)
  - [Linux](#linux)
  - [Node.js](#nodejs)
  - [Python](#python)
  - [Vue3](#vue3)
  - [Windows](#windows)
  - [操作系统](#操作系统)
- [项目结构](#项目结构)
- [许可证](#许可证)

---

## 获取方式

从 GitHub 拉取：

```bash
git clone https://github.com/loskyertt/Notes
```

或者从 Gitee 拉取：

```bash
git clone https://gitee.com/loskyertt/Notes
```

## 阅读方式

推荐使用 [Obsidian](https://obsidian.md/) 打开本仓库，以获得最佳阅读体验（支持双向链接、数学公式渲染、图片预览等）。

主题推荐：**Things** 或 **Blue Topaz**，配色看起来很舒服。

---

## 目录导航

### AI

AI 相关的学习笔记，涵盖 Agent、MCP、提示词工程等主题。

| 主题 | 说明 |
|------|------|
| [Agent](AI/Agent/) | Claude Code、Open Code 等 AI Agent 笔记 |
| [MCP](AI/MCP/) | MCP 概念、代码图谱 |
| [提示词](AI/提示词/) | Agent 提示词、笔记整理、简历改写、论文写作 |
| [LobeHub 配置](AI/LobeHub_配置.md) | LobeHub 的配置方法 |

### C++

C++ 开发相关的系统性笔记，涵盖语言核心特性、设计模式、算法、游戏开发及图形学等主题。

| 分类 | 内容 |
|------|------|
| [开发环境配置](CPP/开发环境配置/) | Windows C++ 环境配置、GDB 调试、Qt 配置、PCL 编译 |
| [知识点 - 类与 OOP](CPP/知识点/类与OOP/) | 类与结构体、构造函数、继承、虚函数、多态、const 成员函数 |
| [知识点 - 内存管理](CPP/知识点/内存管理/) | 堆与栈、指针与引用、内存操作、函数指针、多维数组 |
| [知识点 - RAII](CPP/知识点/RAII/) | 作用域、RAII 与资源管理、智能指针、RAII 实战 |
| [知识点 - CXX 特性](CPP/知识点/CXX_特性/) | 模板、运算符重载、内联、起别名、值类别、noexcept |
| [知识点 - 数据类型](CPP/知识点/数据类型/) | 数据类型、类型转换、枚举、联合体 |
| [知识点 - 其它](CPP/知识点/其它/) | main 函数传参、宏、预编译头文件、头文件循环依赖 |
| [STL](CPP/STL/) | 迭代器、lambda、unordered_set、priority_queue、vector、array、chrono、optional、variant、any |
| [并发编程](CPP/并发编程/) | 多线程、线程同步、线程异步、线程池、协程 |
| [网络编程](CPP/网络编程/) | 网络编程基础、IO 多路复用（select/poll/epoll）、Reactor 模型 |
| [Linux 系统编程](CPP/Linux系统编程/) | 基础知识、文件描述符 |
| [OpenGL](CPP/OpenGL/) | GLFW/GLEW/GLAD 配置、渲染管线、着色器 |
| [游戏开发](CPP/游戏开发/) | 动画效果与帧率、坐标系、自动挂载与卸载 |
| [设计模式](CPP/设计模式/) | 单例模式、组合模式 |
| [算法](CPP/算法/) | STL 使用方法、刷题总结（力扣/牛客/CodeFun2000）、技巧汇总、知识点 |

### CMake

CMake 使用教程，涵盖从基础到进阶的完整内容，涉及 OpenCV CMake 源码中的语法点及大型项目中的实用技巧。

| 章节 | 内容 |
|------|------|
| [01 安装与基本介绍](CMake/01_安装与基本介绍.md) | CMake 安装与基本概念 |
| [02 CMake 的文件分布](CMake/02_CMake的文件分布.md) | 项目文件组织方式 |
| [03 变量的设置与引用](CMake/03_变量的设置与引用.md) | 变量操作 |
| [04 运算符与条件、循环语句](CMake/04_运算符_条件_循环语句.md) | 流程控制 |
| [05 目标构建](CMake/05_目标构建.md) | 构建目标定义 |
| [06 变量参与 C++ 的编译](CMake/06_变量参与C++的编译.md) | 编译选项配置 |
| [07 宏与函数](CMake/07_宏与函数.md) | CMake 宏与函数 |
| [08 find_package 详解](CMake/08_find_package详解.md) | 依赖查找机制 |
| [09 生成器表达式](CMake/09_生成器表达式.md) | 生成器表达式 |
| [10 项目的导出与安装](CMake/10_项目的导出与安装.md) | 项目导出与安装 |
| [11 项目实战](CMake/11_项目实战.md) | 综合实战 |
| [12 CTest](CMake/12_CTest.md) | 测试框架 |
| [13 CPack](CMake/13_CPack.md) | 打包分发 |

### Docker

Docker 学习笔记，包含基础教程、Dockerfile 编写、开发环境搭建及常见问题。

| 主题 | 说明 |
|------|------|
| [基础教程](Docker/基础教程.md) | Docker 基本概念与操作 |
| [Dockerfile 教程](Docker/Dockerfile_教程.md) | Dockerfile 编写方法 |
| [容器代理](Docker/容器代理.md) | 容器网络代理配置 |
| [问题汇总](Docker/问题汇总.md) | 常见问题与解决方案 |
| [开发环境搭建](Docker/开发环境搭建/) | Hexo、MySQL、PCL、深度学习等环境搭建 |

### Git

Git 版本控制学习笔记。

| 主题 | 说明 |
|------|------|
| [初始配置](Git/01_初始配置.md) | Git 安装与初始配置 |
| [基本使用](Git/02_基本使用.md) | 日常操作命令 |
| [分支管理策略](Git/03_分支管理策略.md) | 分支模型与协作流程 |
| [常用参数说明](Git/04_常用参数说明.md) | 常用命令参数速查 |

### Linux

Linux 使用记录，涵盖多个发行版的安装、配置与日常使用。

| 分类 | 内容 |
|------|------|
| [Arch 系](Linux/Arch系/) | EndeavourOS 安装、KDE 桌面配置、pacman 使用教程 |
| [Debian 系](Linux/Debian系/) | 中文乱码修复、镜像源配置 |
| [NixOS](Linux/NixOS/) | NixOS 安装、WSL2 NixOS、教程、问题汇总、Nix 开发环境配置 |
| [WSL](Linux/WSL/) | WSL2 教程、WSL 容器 GUI |
| [常用指令](Linux/常用指令/) | 用户操作、文件权限、文件操作、grep、打包压缩、systemctl |
| [系统配置](Linux/系统配置/) | Docker、Firewall、Flatpak、Grub、Steam、nix、zsh、字体、挂载、语言、软件包、运维 |

### Node.js

Node.js 相关工具使用教程。

| 主题 | 说明 |
|------|------|
| [fnm 使用教程](nodejs/01_fnm_使用教程.md) | Fast Node Manager 版本管理 |
| [包管理器](nodejs/02_包管理器.md) | npm/pnpm/yarn 包管理器对比与使用 |

### Python

Python 学习笔记，包含爬虫、并发编程和工具使用。

| 分类 | 内容 |
|------|------|
| [Python 爬虫](Python/Python爬虫/) | 从基础到进阶的爬虫教程（9 篇） |
| [并发编程](Python/并发编程/) | 多进程和多线程编程 |
| [其它](Python/其它/) | 包管理器、代理池搭建、matplotlib 乱码解决 |

### Vue3

Vue3 框架学习教程，包含 TypeScript 语法参考。

- [Vue3 教程](Vue3教程/Vue3教程.md)

### Windows

Windows 系统使用技巧。

- [工作区切换设置](Windows/工作区切换设置.md)

### 操作系统

操作系统课程笔记，内容系统完整，配有大量示意图。

| 章节 | 内容 |
|------|------|
| [操作系统基础](操作系统/01操作系统基础/) | 基本概念、发展历程、运行环境、系统结构、引导、虚拟机 |
| [进程与线程](操作系统/02进程与线程/) | 进程、线程、CPU 调度、同步与互斥、死锁 |
| [内存管理](操作系统/03内存管理/) | 内存管理概念、虚拟内存管理 |
| [文件管理](操作系统/04文件管理/) | 文件系统基础、目录、空闲分区管理、文件操作、共享与保护、文件系统 |
| [输入输出管理](操作系统/05输入-输出管理/) | I/O 管理概述、设备独立性软件、磁盘与固态硬盘 |
| [题解](操作系统/题解/) | PV 操作解题思路、存储系统答题结构 |

---

## 项目结构

```
Notes/
├── AI/                    # AI 相关笔记（Agent、MCP、提示词）
├── CMake/                 # CMake 教程（13 章 + 源码示例）
├── CPP/                   # C++ 笔记（语言特性、算法、网络编程、OpenGL...）
├── Docker/                # Docker 笔记（基础、Dockerfile、环境搭建）
├── Git/                   # Git 笔记（配置、使用、分支策略）
├── Linux/                 # Linux 笔记（Arch/Debian/NixOS/WSL/常用指令/系统配置）
├── Python/                # Python 笔记（爬虫、并发编程、工具）
├── Vue3教程/               # Vue3 教程
├── Windows/               # Windows 使用技巧
├── nodejs/                # Node.js 工具教程（fnm、包管理器）
├── 操作系统/               # 操作系统课程笔记（基础、进程、内存、文件、I/O）
├── .gitignore             # Git 忽略规则
├── LICENSE                # MIT 许可证
└── README.md              # 本文件
```

---

## 许可证

本项目基于 [MIT License](LICENSE) 开源。
