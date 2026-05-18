---
title: VSCode 配置 C++ Qt 开发环境
date: 2024-12-11 19:04:07
excerpt: "注意：这里的系统环境是 Arch-WSL2 子系统。再其它 WSL2 下或者 Linux 发行版下该教程都有用。"
categories: "C++教程"
---


# 1.安装开发包

```bash
sudo pacman -S qt6-base
```

如果要安装其它版本的，比如 `qt5`，则是 `qt5-base`。

对于 WSL 下，是只能连接到 x11 服务的，毕竟 Windows 也没有 Wayland 会话。所以还需要安装 `xcb` 支持库：

```bash
sudo pacman -S qt6-xcb-private-headers
```

需要与安装的 `qt` 版本对应，因为我安装的是 `qt6-base`，我这里自然安装的就是 `qt6` 对应的 `xcb` 支持库。

安装好后需要配置环境变量，把下面这行内容添加到 `.bashrc` 或者 `.zshrc` 中（取决于自己用的 shell）：

```bash
export QT_QPA_PLATFORM=xcb
```

进行这步配置主要是为了避免运行 `qt` 程序时，出现下面的问题：

```txt
qt.qpa.plugin: Could not find the Qt platform plugin "wayland" in ""
```

如果是在 Linux 发行版上的话（能支持`wayland`会话的），可能会出现这样的问题：

```txt
QApplication: invalid style override 'kvantum' passed, ignoring it.
        Available styles: Breeze, Windows, Fusion
```

这是 Qt 程序的 `ui` 样式，提示 `kvantum` 样式不可用，可用的有 `Breeze`，`Windows`，`Fusion` 这三种。所以还需要设置环境变量，把下面这行内容添加到 `.bashrc` 或者 `.zshrc` 中（取决于自己用的 shell）：

```bash
export QT_STYLE_OVERRIDE=Fusion
```

---

# 2.安装字体

默认情况下，中文字体会安装在以下路径之一：

1. `/usr/share/fonts/noto-cjk/`：用于 **Noto CJK** 字体（推荐）。
2. `/usr/share/fonts/wenquanyi/`：用于 **文泉驿** 字体。
3. `/usr/share/fonts/TTF/` 或 `/usr/share/fonts/truetype/`：用于 TTF/OTF 格式的字体文件。

先查看下系统是否有中文字体，使用下面的指令：

```bash
fc-list :lang=zh
```

如果有输出的话，后面操作就不用管，否则就需要额外安装字体。

在我的系统中（Arch-WSL2），`/usr/share/fonts/` 目录下只显示了以下内容，说明没有中文字体：

- **100dpi 和 75dpi**：  
  这些是 X11 系统中的点阵字体（bitmap fonts），通常用于较低分辨率的显示器或作为一些旧式程序的备用字体。  
  - `100dpi` 表示每英寸 100 点的字体。
  - `75dpi` 表示每英寸 75 点的字体。  
  这些字体一般用于非常老旧的 X11 应用程序，现代应用通常不需要这些字体。

- **encodings**：  
  包含字体编码文件，用于定义如何将字符代码映射到字体中的字形。它主要与点阵字体或老式字体相关。

- **util**：  
  一些字体工具或辅助文件的目录，通常与字体管理或配置有关。

1. 安装 Noto CJK 字体（推荐）

这是 Google 提供的现代中文字体，适用于大多数应用场景：

```bash
sudo pacman -S noto-fonts-cjk
```

2. 安装其他中文字体

- **文泉驿字体**：
```bash
sudo pacman -S wqy-zenhei wqy-microhei
```

- **思源宋体**（Noto Serif CJK）：
```bash
sudo pacman -S noto-fonts-emoji noto-fonts-cjk noto-fonts
```

安装一种即可，安装好后，需要重启系统（我这里只需要重启 Arch-WSL2 子系统）。

---

# 3.项目构建

一个简单的 `qt` 开发模板：

```bash
cmake_minimum_required(VERSI0N 3.12)

set(CMAKE_C_COMPILER clang)
set(CMAKE_CXX_COMPILER clang++)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

project(qtguide)

find_package(Qt6 REQUIRED COMPONENTS Core Widgets REQUIRED)

file(GLOB sources *.cpp)
add_executable(qtguide ${sources})
target_link_libraries(qtguide PRIVATE Qt6::Core Qt6::Widgets)
```

建议设置 `clang` 和 `clang++` 作为编译器。我在 Arch-WSL2 下使用 `gcc` 和 `g++` 会出现问题。还有就是设置编译器这部分，一定要放在 `cmake_minimum_required` 下面，其它的上面。因为编译器的选择会影响后续变量、库、目标等配置。CMake 初始化后再更改编译器可能会导致配置问题。

对于 `qt` 模板，可以参考我的仓库中的模板配置：[cpp_template](https://github.com/loskyertt/cpp_template)。