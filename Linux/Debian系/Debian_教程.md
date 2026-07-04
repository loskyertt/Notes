---
title: Debian Server 安装与初始化配置
date: 2026-07-04
tags:
  - Linux
  - Debian
  - Server
  - VMware
  - APT
aliases:
  - Debian 教程
  - Debian Server 初始化
---

# 1. Debian Server 安装目标

## 1.1 适用场景

本文记录的是在 **VMware 虚拟机中安装 Debian Server** 的完整思路，目标是得到一个无桌面环境、资源占用低、适合长期学习和开发实验的 Linux 系统。

适合的学习与开发场景包括：

- Linux 基础命令与系统管理
- Shell 脚本与服务管理
- C/C++ 编译、调试与网络编程
- Git、SSH、远程开发环境
- Docker、服务部署与服务器运维实验

## 1.2 最终系统形态

推荐安装完成后的系统形态如下：

| 项目 | 建议状态 | 说明 |
|---|---|---|
| 桌面环境 | 不安装 | 保持服务器环境简洁，减少资源占用 |
| SSH Server | 安装 | 方便远程登录、VS Code Remote SSH、文件传输 |
| 标准系统工具 | 安装 | 保留 Debian 基础命令和常用管理工具 |
| 开发工具链 | 安装后手动配置 | 避免安装阶段引入过多无关软件 |
| 软件源 | 使用官方源或可靠镜像源 | 保证安全更新和依赖稳定 |

> [!summary]
> Debian Server 的核心思路是：**系统安装阶段尽量小，初始化阶段按需补齐工具链**。这样既接近真实服务器环境，也便于学习 Linux 的软件包、权限、网络和服务管理机制。

---

# 2. Debian 安装镜像选择

## 2.1 常见镜像类型

Debian 官方常见安装介质主要有以下几类：

| 镜像类型 | 特点 | 适合场景 | 服务器安装推荐度 |
|---|---|---|---|
| `netinst` | 体积小，只包含安装器和少量基础组件，安装时联网下载软件包 | 网络稳定、希望安装最新软件包 | 高 |
| `DVD-1` | 体积较大，包含较多基础软件包，可降低安装阶段对网络的依赖 | 网络不稳定、希望离线或半离线安装 | 高 |
| `Live ISO` | 可直接启动图形桌面环境，通常包含 GNOME、KDE、XFCE 等桌面 | 体验桌面系统、救援、临时使用 | 低 |

## 2.2 Live ISO 的定位

Live ISO 文件名通常类似：

```text
debian-live-13.x.x-amd64-gnome.iso
debian-live-13.x.x-amd64-kde.iso
debian-live-13.x.x-amd64-xfce.iso
```

这类镜像可以直接进入一个可用的图形桌面系统，适合体验 Debian 桌面环境或临时救援系统。它并不是服务器最优先的安装介质，因为它的默认内容更偏向桌面使用，容易让初学者在安装时混入不需要的桌面组件。

## 2.3 服务器推荐选择

安装 Debian Server 时，优先选择以下两类之一：

```text
debian-13.x.x-amd64-netinst.iso
debian-13.x.x-amd64-DVD-1.iso
```

不建议优先使用：

```text
debian-live-13.x.x-*.iso
```

选择原则：

- 网络稳定：优先使用 `netinst`，镜像小，安装过程更灵活。
- 网络不稳定：优先使用 `DVD-1`，基础软件包更完整。
- 目标是服务器：不要选择以桌面体验为主的 Live ISO。

---

# 3. VMware 虚拟机配置

## 3.1 推荐硬件配置

用于 Linux 学习、C/C++ 开发、Docker 实验的虚拟机可以按下表配置：

| 配置项 | 推荐值 | 说明 |
|---|---:|---|
| CPU | 2 到 4 核 | 编译、Docker 和多服务实验建议 4 核 |
| 内存 | 4 GB | 最低可用 2 GB，但开发环境会偏紧 |
| 磁盘 | 40 到 60 GB | 建议使用单文件或自动扩展虚拟磁盘 |
| 网络 | NAT | 便于共享宿主机网络，也能被宿主机 SSH 访问 |
| 固件 | UEFI | 更贴近现代系统默认启动方式 |
| ISO | `netinst` 或 `DVD-1` | 根据网络条件选择 |

## 3.2 网络模式建议

VMware 常见网络模式对比如下：

| 网络模式 | 特点 | 建议 |
|---|---|---|
| NAT | 虚拟机通过宿主机访问外网，配置简单 | 初学和开发实验优先选择 |
| Bridged | 虚拟机直接接入局域网，像一台独立主机 | 需要局域网其他设备访问时使用 |
| Host-only | 仅宿主机与虚拟机互通，默认不能访问外网 | 隔离实验环境时使用 |

> [!tip]
> 如果目标是通过宿主机连接 Debian 虚拟机，NAT 模式通常已经足够。后续只需要在 Debian 中安装 SSH Server，并确认虚拟机 IP 地址即可。

---

# 4. Debian 安装过程关键选项

## 4.1 安装器选择

启动安装镜像后，常见入口包括：

```text
Install
Graphical install
Advanced options
```

服务器安装推荐选择：

```text
Install
```

原因是文本安装器更轻量，流程清晰，也更接近服务器环境的传统安装方式。`Graphical install` 只是图形化安装界面，并不等于一定会安装桌面环境；真正决定是否安装桌面的是后续的 **Software Selection**。

## 4.2 Software Selection

安装过程中的 **Software Selection** 是决定系统形态的关键步骤。

如果目标是无桌面 Debian Server，应取消选择：

```text
Debian desktop environment
GNOME
KDE Plasma
XFCE
LXQt
Cinnamon
MATE
```

建议仅保留：

```text
SSH server
standard system utilities
```

这样安装完成后得到的是：

- 无 GUI
- 无桌面环境
- 有基础系统工具
- 可通过 SSH 远程管理
- 更接近生产服务器和云服务器环境

> [!warning]
> 不要只看镜像文件名判断最终系统是否带桌面。即使使用 DVD 镜像，只要在 Software Selection 中取消桌面环境，也可以安装出纯服务器系统。

---

# 5. SSH Server 配置

## 5.1 SSH Server 的作用

`SSH server` 提供远程登录能力，使宿主机或其他机器可以通过 SSH 安全连接到 Debian。

典型用途包括：

- 使用终端远程登录 Debian
- 使用 VS Code Remote SSH 连接开发
- 使用 CLion Remote Toolchain 配置远程工具链
- 使用 `scp`、`rsync` 传输文件
- 远程管理 Git、Docker 和系统服务

## 5.2 基本连接方式

在 Debian 中查看 IP 地址：

```bash
ip addr
```

在宿主机中连接：

```bash
ssh 用户名@IP地址
```

示例：

```bash
ssh sky@192.168.88.100
```

首次连接时，SSH 会提示是否信任该主机指纹。确认 IP 无误后输入 `yes`，再输入用户密码即可登录。

## 5.3 常用排查命令

检查 SSH 服务状态：

```bash
systemctl status ssh
```

启动 SSH 服务：

```bash
sudo systemctl start ssh
```

设置开机自启：

```bash
sudo systemctl enable ssh
```

查看虚拟机 IP：

```bash
hostname -I
```

---

# 6. sudo 权限配置

## 6.1 问题现象

最小化安装后，普通用户可能无法执行管理员命令。例如：

```bash
sudo apt install build-essential
```

提示类似：

```text
sky is not in the sudoers file.
```

或中文环境下显示：

```text
sky 未出现在 sudoers 文件中。
```

## 6.2 根因

`sudo` 能否使用，不只取决于系统是否安装了 `sudo` 软件包，还取决于当前用户是否具有 sudo 权限。

在 Debian 中，普通用户通常需要加入 `sudo` 用户组，才可以通过 `sudo` 临时获得管理员权限。

## 6.3 解决步骤

切换到 root 用户：

```bash
su -
```

将普通用户加入 `sudo` 组：

```bash
usermod -aG sudo sky
```

确认用户组：

```bash
groups sky
```

退出当前登录会话后重新登录，再检查当前用户组：

```bash
groups
```

应包含：

```text
sudo
```

测试 sudo 是否生效：

```bash
sudo whoami
```

预期输出：

```text
root
```

> [!warning]
> `usermod -aG sudo sky` 中的 `-aG` 不要写错。`-a` 表示追加用户组，`-G` 表示设置附加组。如果漏掉 `-a`，可能覆盖用户原有附加组。

---

# 7. APT 软件源配置

## 7.1 软件源文件位置

Debian 新版本安装后，APT 源配置可能使用 Deb822 格式，常见路径为：

```text
/etc/apt/sources.list.d/debian.sources
```

传统格式仍可能存在于：

```text
/etc/apt/sources.list
```

排查软件源时，应同时查看：

```bash
ls /etc/apt/
ls /etc/apt/sources.list.d/
```

## 7.2 Deb822 格式示例

以 Debian 13 Trixie 和清华大学镜像源为例：

```text
Types: deb
URIs: https://mirrors.tuna.tsinghua.edu.cn/debian
Suites: trixie trixie-updates
Components: main contrib non-free non-free-firmware
Signed-By: /usr/share/keyrings/debian-archive-keyring.gpg

Types: deb
URIs: https://mirrors.tuna.tsinghua.edu.cn/debian-security
Suites: trixie-security
Components: main contrib non-free non-free-firmware
Signed-By: /usr/share/keyrings/debian-archive-keyring.gpg
```

修改后更新软件包索引：

```bash
sudo apt update
```

## 7.3 Components 含义

Debian 软件源中的 `Components` 决定启用哪些软件仓库组件：

| 组件 | 含义 | 说明 |
|---|---|---|
| `main` | 完全符合 Debian 自由软件准则的软件 | 默认核心仓库，应始终启用 |
| `contrib` | 软件本身自由，但依赖非自由组件 | 按需启用 |
| `non-free` | 不完全符合 Debian 自由软件准则的软件 | 固件、驱动或部分工具可能需要 |
| `non-free-firmware` | 非自由固件 | 新版本 Debian 将固件单独拆分，常用于硬件支持 |

学习和虚拟机开发环境中，通常可以启用：

```text
main contrib non-free non-free-firmware
```

这样可以减少安装驱动、固件或常用工具时的包缺失问题。

# 8. DVD 光盘源问题

## 8.1 问题现象

使用 DVD 镜像安装 Debian 后，APT 可能自动保留 DVD 光盘源。后续执行安装或更新命令时，可能出现：

```text
Media change: please insert the disc labeled ...
```

或：

```text
Err: cdrom://...
```

## 8.2 根因

系统的软件源配置中仍然存在类似条目：

```text
deb cdrom:[Debian GNU/Linux ...]
```

APT 认为 DVD 仍然是一个可用的软件源，因此会尝试从虚拟光驱读取软件包。

## 8.3 排查与处理

搜索所有 APT 配置中的光盘源：

```bash
grep -R "cdrom" /etc/apt/
```

如果在 `/etc/apt/sources.list` 或 `/etc/apt/sources.list.d/` 中发现 `cdrom` 条目，应删除或注释掉对应内容。

处理完成后重新更新索引：

```bash
sudo apt update
```

> [!warning]
> 切换到网络镜像源后，应移除 DVD 光盘源。否则 APT 可能在安装软件时反复要求插入安装光盘。

# 9. 最小化系统开发环境

## 9.1 最小化安装的特点

Debian Server 最小化安装默认不会提供完整开发环境。以下命令没有输出通常是正常现象：

```bash
which gcc
which g++
which make
which cmake
```

这说明对应工具尚未安装，而不是系统安装失败。

## 9.2 推荐安装的软件包

初始化开发环境可以安装：

```bash
sudo apt update

sudo apt install -y \
    build-essential \
    cmake \
    gdb \
    git \
    vim \
    curl \
    wget \
    tree \
    htop \
    unzip \
    zip \
    python3 \
    python3-pip \
    python-is-python3 \
    pkg-config
```

常用软件包说明：

| 软件包 | 作用 |
|---|---|
| `build-essential` | 安装 GCC、G++、Make、libc 开发头文件等基础编译工具 |
| `cmake` | C/C++ 项目常用构建系统 |
| `gdb` | C/C++ 调试器 |
| `git` | 版本控制工具 |
| `vim` | 终端编辑器 |
| `curl` / `wget` | 网络下载与接口测试工具 |
| `tree` | 目录结构查看工具 |
| `htop` | 交互式进程查看工具 |
| `python3` | Python 3 解释器 |
| `python3-pip` | Python 包管理工具 |
| `python-is-python3` | 让 `python` 命令指向 `python3` |
| `pkg-config` | 查询编译和链接参数，常用于 C/C++ 依赖管理 |

## 9.3 Python 命令说明

Debian 默认以 Python 3 为主，系统中通常有：

```bash
python3
```

但不一定有：

```bash
python
```

如果希望 `python` 命令直接执行 Python 3，可以安装：

```bash
sudo apt install python-is-python3
```

> [!warning]
> 不建议手动用软链接随意覆盖系统 Python 路径。优先使用发行版提供的 `python-is-python3`，这样更符合 Debian 的包管理约定。

# 10. Debian 包管理体系

## 10.1 分层结构

Debian 并不是只有 `apt` 一个工具，而是采用分层的软件包管理体系：

```text
用户常用命令
────────────────────────
apt / apt-get / apt-cache / apt-mark / aptitude
────────────────────────
APT 软件包管理系统
────────────────────────
dpkg
────────────────────────
.deb 软件包文件
```

其中：

- `.deb` 是 Debian 软件包文件格式。
- `dpkg` 负责本地安装、卸载、查询 `.deb` 包。
- APT 负责软件源、依赖解析、下载软件包，并调用 `dpkg` 完成安装。
- `apt`、`apt-get`、`apt-cache` 等是面向用户或脚本的前端命令。

## 10.2 dpkg

`dpkg` 是底层包管理工具，直接操作本地 `.deb` 文件。

示例：

```bash
sudo dpkg -i xxx.deb
```

特点：

- 可以安装本地 `.deb` 包。
- 可以查询系统已安装软件包。
- 不会主动从网络下载依赖。
- 遇到缺失依赖时，需要再交给 APT 修复。

常见修复命令：

```bash
sudo apt -f install
```

## 10.3 APT

APT 建立在 `dpkg` 之上，负责更完整的软件包管理流程：

- 读取软件源配置。
- 下载软件包索引。
- 解析依赖关系。
- 下载目标软件包及其依赖。
- 调用 `dpkg` 完成安装、升级或卸载。

日常安装软件通常使用：

```bash
sudo apt install git
```

## 10.4 apt 与 apt-get

`apt` 是更适合人工交互的命令，输出更友好，日常使用推荐：

```bash
sudo apt update
sudo apt install git
apt search nginx
apt show cmake
```

`apt-get` 是更经典、更稳定的命令，常见于脚本、Dockerfile 和自动化环境：

```bash
sudo apt-get update
sudo apt-get install -y git
```

实践建议：

- 手动管理系统时，用 `apt` 更方便。
- 写脚本或 Dockerfile 时，用 `apt-get` 更常见。

## 10.5 apt-cache 与 apt-mark

`apt-cache` 主要用于查询软件包信息：

```bash
apt-cache depends cmake
apt-cache policy git
```

`apt-mark` 可用于标记软件包状态，例如锁定版本：

```bash
sudo apt-mark hold gcc
```

取消锁定：

```bash
sudo apt-mark unhold gcc
```

版本锁定适合需要避免关键组件被自动升级的场景，但普通学习环境不建议随意锁定系统基础包。

## 10.6 aptitude

`aptitude` 是 APT 的另一个前端，提供文本界面和更复杂的依赖解决能力。

它现在不是日常必需工具，但在处理复杂依赖冲突时仍有参考价值。

# 11. Debian 软件仓库策略

## 11.1 常见仓库分类

| 仓库 | 是否官方 | 主要作用 | 使用建议 |
|---|---|---|---|
| Debian Repository | 官方 | 主软件仓库，提供绝大多数系统软件 | 必须启用 |
| Debian Security | 官方 | 安全更新 | 必须启用 |
| Debian Backports | 官方 | 将较新版本软件回移植到稳定版 | 按需启用 |
| Debian ELTS | 扩展长期支持 | 为已结束常规支持的旧版本提供安全维护 | 企业旧系统按需 |
| Deb Multimedia | 第三方 | 多媒体编解码器和相关软件 | 音视频场景谨慎使用 |
| Debian CN | 第三方社区 | 中文软件、输入法、字体等 | 一般不必启用 |

## 11.2 官方仓库

Debian Repository 是系统最主要的软件来源，提供：

- GCC
- Git
- Python
- OpenSSH
- CMake
- Vim
- Nginx
- PostgreSQL
- Docker 相关组件中的部分包

Debian Security 提供安全补丁，常见涉及：

- OpenSSL
- OpenSSH
- Linux kernel
- Web 服务组件
- 语言运行时和基础库

> [!warning]
> 服务器环境必须保留安全更新源。禁用安全源会增加已知漏洞长期存在的风险。

## 11.3 第三方仓库

第三方仓库可能提供官方仓库中没有的软件，或者提供更新版本的软件。但它们也可能带来以下问题：

- 替换官方库文件，导致依赖关系复杂化。
- 更新节奏与 Debian Stable 不一致。
- 包签名、维护质量和安全策略不一定与 Debian 官方一致。
- 出问题时排查难度更高。

因此，学习 Linux、Shell、Git、C++、Docker、网络编程时，建议保持软件源简单：

```text
推荐：
Debian 官方仓库或可靠国内镜像
Debian Security
Debian Backports（按需）

不推荐默认启用：
Deb Multimedia
Debian CN
其他来源不明的第三方仓库
```

# 12. 初始化检查清单

## 12.1 系统基础检查

安装完成并首次登录后，可以按顺序检查：

```bash
hostnamectl
ip addr
ping -c 4 debian.org
sudo apt update
```

确认内容：

- 主机名是否正确。
- 虚拟机是否获得 IP 地址。
- DNS 和外网访问是否正常。
- APT 软件源是否可用。

## 12.2 权限与远程连接检查

检查当前用户组：

```bash
groups
```

测试 sudo：

```bash
sudo whoami
```

检查 SSH：

```bash
systemctl status ssh
hostname -I
```

从宿主机测试连接：

```bash
ssh 用户名@虚拟机IP
```

## 12.3 开发环境检查

安装开发工具后检查：

```bash
gcc --version
g++ --version
make --version
cmake --version
git --version
python3 --version
```

如果这些命令能正常输出版本信息，说明基础开发环境已经可用。

# 13. 常见问题速查

## 13.1 apt 要求插入 DVD

原因：

- 系统仍保留 `cdrom` 软件源。

处理：

```bash
grep -R "cdrom" /etc/apt/
sudo apt update
```

删除或注释 `cdrom` 条目后重新执行 `sudo apt update`。

## 13.2 sudo 提示用户不在 sudoers 文件中

原因：

- 当前用户不在 `sudo` 用户组。

处理：

```bash
su -
usermod -aG sudo 用户名
```

重新登录后再测试：

```bash
sudo whoami
```

## 13.3 找不到 gcc、make、cmake

原因：

- 最小化安装默认不安装开发工具链。

处理：

```bash
sudo apt install -y build-essential cmake gdb git
```

## 13.4 python 命令不存在

原因：

- Debian 默认使用 `python3`，不一定提供 `python` 命令。

处理：

```bash
sudo apt install python-is-python3
```

# 14. 核心总结

Debian Server 的安装与初始化可以概括为以下流程：

```text
选择 netinst 或 DVD-1
        ↓
VMware 创建轻量虚拟机
        ↓
安装时取消桌面环境
        ↓
保留 SSH server 和 standard system utilities
        ↓
配置 sudo、SSH、APT 软件源
        ↓
移除 DVD 光盘源
        ↓
安装开发工具链
        ↓
保持官方仓库优先，谨慎添加第三方仓库
```

> [!summary]
> 最佳实践是：**用最小化安装获得干净系统，用 APT 按需扩展能力，用官方仓库保证稳定性，用 SSH 和 sudo 建立日常管理入口**。
