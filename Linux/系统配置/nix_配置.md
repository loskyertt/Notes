---
title: Linux 命令行工具推荐
date: 2026-01-27 21:54:00
excerpt: 如何在非 NixOS 发行版上安装并使用 nix 包管理器，这里用的发行版是 Arch。
categories: Linux教程
---


# 1. 安装

对于 Arch Linux，用 pacman 安装即可：

```bash
sudo pacman -S nix
```

---

# 2. 基础配置

## 2.1 启动 Nix 守护进程 (Multi-user install)

> Arch 的包管理器安装方式通常配置为 **多用户模式**。因此需要启用并启动 `nix-daemon` 服务，这样 Nix 才能处理构建任务和权限管理。

在终端运行以下命令：

```bash
sudo systemctl enable --now nix-daemon
```

## 2.1 配置当前用户环境

虽然后台进程跑起来了，但你的 Shell（如 bash 或 zsh）还不知道如何找到 Nix 的路径。所以需要将当前用户添加到 `nix-users` 用户组，并加载环境变量。

- **添加用户组：**
  
```
sudo usermod -aG nix-users $USER
```

> **注意**：修改用户组后，你需要重新登录（注销再登录）才能生效。

- **配置 Shell 启动项：**

通常 Arch 的安装包会自动在 `/etc/profile.d/nix.sh` 创建脚本。如果重新登录后 `nix --version` 报错，请手动在你的 `~/.bashrc` 或 `~/.zshrc` 中添加：

```bash
if [ -e /etc/profile.d/nix.sh ]; then . /etc/profile.d/nix.sh; fi
```

## 2.2 设置频道 (Channels)

Nix 的“频道”类似于 pacman 的软件仓库。默认情况下，刚安装好的 Nix 可能是空的。你需要添加一个频道来获取软件包定义：

```bash
# 添加官方的 unstable 频道（推荐，因为滚动更新更符合 Arch 用户的习惯）
nix-channel --add https://nixos.org/channels/nixpkgs-unstable nixpkgs

# 更新频道索引
nix-channel --update
```

---

# 3. 基础使用指南

nix 有三个最常用的场景：

|**命令**|**用途**|**举例**|
|---|---|---|
|**`nix-shell -p`**|**临时**进入包含某软件的环境（不污染系统）|`nix-shell -p python3`|
|**`nix-env -iA`**|**永久**安装软件到用户环境|`nix-env -iA nixpkgs.hello`|
|**`nix-collect-garbage -d`**|**清理**不再使用的旧版本和缓存|释放磁盘空间|

---

# 4. 启用 Flakes (推荐)

现代 Nix 社区都在转向 **Flakes**，它能提供更好的版本锁定和可重现性。

1. 创建配置文件目录：`mkdir -p ~/.config/nix`

2. 编辑 `~/.config/nix/nix.conf`，添加以下内容：

```bash
experimental-features = nix-command flakes
```

3. 重启 `nix-daemon`：`sudo systemctl restart nix-daemon`

---

# 6. 常见问题

> - **不要用 sudo 安装 Nix 软件**：Nix 的设计初衷就是让普通用户管理自己的环境。使用 `nix-env` 或 `nix profile` 时无需 `sudo`。
> - **磁盘空间**：Nix 会保留不同版本的软件以实现回滚，如果你频繁折腾，记得定期运行 `nix-collect-garbage -d`。

## 6.1 zsh 通配符问题

比如执行：

```bash
nix shell nixpkgs#tldr
```

出现 `zsh: no matches found: nixpkgs#tldr`。

这是因为与 **Zsh 的通配符冲突**：在 Zsh 中，`#` 是一个特殊字符（用于扩展通配符）。当你输入 `nixpkgs#tldr` 时，Zsh 试图在当前目录下寻找匹配的文件，找不到自然就报错了。

解决方法：你可以通过给参数加**引号**来绕过 Zsh 的检查：

```bash
nix shell "nixpkgs#tldr"
```

或者在执行命令前加个反斜杠：`nix shell nixpkgs\#tldr`。