---
title: Nix 包管理器配置指南（非 NixOS）
date: 2026-01-27 21:54:00
tags:
  - linux/系统配置
  - nix
  - 包管理
---

# 1. 安装

对于 Arch Linux，用 pacman 安装即可：

```bash
sudo pacman -S nix
```

---

# 2. 基础配置

## 2.1 启动 Nix 守护进程 (Multi-user install)

> [!info] 多用户模式
> Arch 的包管理器安装方式通常配置为**多用户模式**。因此需要启用并启动 `nix-daemon` 服务，这样 Nix 才能处理构建任务和权限管理。

```bash
sudo systemctl enable --now nix-daemon
```

## 2.2 配置当前用户环境

虽然后台进程跑起来了，但你的 Shell（如 bash 或 zsh）还不知道如何找到 Nix 的路径。需要将当前用户添加到 `nix-users` 用户组，并加载环境变量。

- **添加用户组：**

```bash
sudo usermod -aG nix-users $USER
```

> [!warning] 重新登录生效
> 修改用户组后，你需要重新登录（注销再登录）才能生效。

- **配置 Shell 启动项：**

通常 Arch 的安装包会自动在 `/etc/profile.d/nix.sh` 创建脚本。如果重新登录后 `nix --version` 报错，请手动在你的 `~/.bashrc` 或 `~/.zshrc` 中添加：

```bash
if [ -e /etc/profile.d/nix.sh ]; then . /etc/profile.d/nix.sh; fi
```

## 2.3 设置频道 (Channels)

Nix 的"频道"类似于 pacman 的软件仓库。默认情况下，刚安装好的 Nix 可能是空的。你需要添加一个频道来获取软件包定义：

```bash
nix-channel --add https://nixos.org/channels/nixpkgs-unstable nixpkgs
nix-channel --update
```

> [!tip] 推荐 unstable 频道
> 官方的 unstable 频道滚动更新更符合 Arch 用户的习惯。

---

# 3. 基础使用指南

nix 有三个最常用的场景：

| **命令** | **用途** | **举例** |
|---|---|---|
| **`nix-shell -p`** | **临时**进入包含某软件的环境（不污染系统） | `nix-shell -p python3` |
| **`nix-env -iA`** | **永久**安装软件到用户环境 | `nix-env -iA nixpkgs.hello` |
| **`nix-collect-garbage -d`** | **清理**不再使用的旧版本和缓存 | 释放磁盘空间 |

---

# 4. 启用 Flakes（推荐）

现代 Nix 社区都在转向 **Flakes**，它能提供更好的版本锁定和可重现性。

1. 创建配置文件目录：

```bash
mkdir -p ~/.config/nix
```

2. 编辑 `~/.config/nix/nix.conf`，添加以下内容：

```ini
experimental-features = nix-command flakes
```

3. 重启 `nix-daemon`：

```bash
sudo systemctl restart nix-daemon
```

---

# 5. 常见问题

> [!warning] 不要用 sudo 安装 Nix 软件
> Nix 的设计初衷就是让普通用户管理自己的环境。使用 `nix-env` 或 `nix profile` 时无需 `sudo`。

> [!tip] 磁盘空间管理
> Nix 会保留不同版本的软件以实现回滚，如果你频繁折腾，记得定期运行 `nix-collect-garbage -d`。

## 5.1 zsh 通配符问题

比如执行：

```bash
nix shell nixpkgs#tldr
```

出现 `zsh: no matches found: nixpkgs#tldr`。

> [!bug] 原因
> 与 Zsh 的通配符冲突：在 Zsh 中，`#` 是一个特殊字符（用于扩展通配符）。当你输入 `nixpkgs#tldr` 时，Zsh 试图在当前目录下寻找匹配的文件，找不到自然就报错了。

解决方法——给参数加**引号**来绕过 Zsh 的检查：

```bash
nix shell "nixpkgs#tldr"
```

或者在执行命令前加个反斜杠：

```bash
nix shell nixpkgs\#tldr
```

> [!note] 相关笔记
> 更多 zsh 配置问题参见 [[zsh_配置#3. 问题汇总]]，CLI 工具推荐参见 [[软件包]]。
