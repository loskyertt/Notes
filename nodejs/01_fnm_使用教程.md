---
title: fnm 使用教程
date: 2024-11-12
tags:
  - fnm
  - nodejs
  - node
  - javascript
  - version-manager
  - rust
  - tutorial
  - tool
aliases:
  - fnm 教程
  - Fast Node Manager
  - fnm 配置
---

# 1. fnm 使用教程

## 1.1 概述

**fnm** (Fast Node Manager) 是一款基于 **Rust** 编写的高性能 Node.js 版本管理器，==相较于 nvm 和 nvm-windows，fnm 在运行速度和跨平台体验上具有显著优势==。核心使用流程分为安装、配置 Shell 环境和常用命令管理三个步骤。

> [!abstract] 核心优势
> - 基于 Rust 编写，启动和切换速度极快
> - 完美支持 Windows、macOS 和 Linux
> - 支持通过 `.node-version` 或 `.nvmrc` 文件自动切换版本

## 1.2 安装方法

根据操作系统选择对应的包管理器进行安装。

### 1.2.1 macOS / Linux

```bash
brew install fnm
```

### 1.2.2 Windows

```powershell
# 使用 Winget（Windows 自带，推荐）
winget install Schniz.fnm

# 或使用 Scoop（需预先安装 Scoop）
scoop install fnm
```

也可以手动下载安装包：[Schniz/fnm](https://github.com/Schniz/fnm)，把压缩包解压后添加到环境变量（PATH）中即可。

> [!tip] Windows 用户须知
> Windows 上可以完美安装和使用 fnm，且运行速度远快于 nvm-windows。安装完成后仍需配置 Shell 环境变量才能正常使用。

## 1.3 配置 Shell 环境

==安装完成后，必须将 fnm 注入到终端配置文件中，否则 `node` 命令无法正常使用。==请根据使用的终端选择对应的配置方案。

### 1.3.1 PowerShell（推荐）

1. 在 PowerShell 中运行以下命令打开配置文件：
   ```powershell
   notepad $PROFILE
   ```
   如提示文件不存在，选择创建新文件。

2. 在文件末尾添加以下内容：
   ```powershell
   fnm env --use-on-cd | Out-String | Invoke-Expression
   ```

3. 保存并关闭文件。

> [!warning] PowerShell 执行策略
> 如果运行 `$PROFILE` 时报权限错误，以管理员身份运行 PowerShell，执行以下命令后重试：
> ```powershell
> Set-ExecutionPolicy RemoteSigned -Scope CurrentUser
> ```

### 1.3.2 CMD（命令提示符）

1. 按 `Win + R` 输入 `sysdm.cpl` 打开系统属性。
2. 点击 **高级** 选项卡 → **环境变量**。
3. 在 **用户变量** 中新建变量：
   - 变量名：`FNM_DIR`
   - 变量值：`%USERPROFILE%\AppData\Local\fnm`
4. 如使用 `cmder` 或 `clink`，需将 `fnm env --use-on-cd` 配置到其启动脚本中。

### 1.3.3 Bash

编辑 `~/.bashrc`，添加以下内容：

```bash
eval "$(fnm env --use-on-cd)"
```

### 1.3.4 Zsh

编辑 `~/.zshrc`，添加以下内容：

```zsh
eval "$(fnm env --use-on-cd)"
```

关于 Zsh 的更多配置，参见 [[zsh_配置]]。

> [!tip] 使配置生效
> 配置完成后重启终端，或运行 `source ~/.zshrc`（以 Zsh 为例）使配置立即生效。`--use-on-cd` 参数可在进入包含 `.node-version` 或 `.nvmrc` 文件的项目目录时**自动切换**到对应的 Node 版本。

## 1.4 常用命令

### 1.4.1 查看与安装

```bash
# 查看可安装的远程版本
fnm list-remote

# 安装最新的 LTS 版本
fnm install --lts

# 安装指定版本（如 20.x）
fnm install 20

# 查看已安装的本地版本
fnm list
```

### 1.4.2 切换与使用

```bash
# 临时切换版本（仅当前终端生效）
fnm use 20

# 设置全局默认版本
fnm default 20

# 查看当前使用的版本
node -v
fnm current
```

### 1.4.3 卸载版本

```bash
fnm uninstall 18
```

## 1.5 验证安装

配置完成后，关闭所有终端窗口并重新打开，依次运行以下命令验证：

```powershell
fnm install 20
fnm default 20
node -v
npm -v
```

==如果 `node -v` 报错，说明 Shell 环境配置未正确生效==，请检查 1.3 节的配置步骤。

## 1.6 常见问题排查

### 1.6.1 `which node` 找不到命令

> [!bug] 问题原因
> 成功安装了 Node.js，但终端提示找不到命令。原因是 **Shell 环境变量尚未正确配置**，导致终端无法找到 fnm 安装的 Node 路径。

**第一步：确认 Shell 类型并写入配置**

```bash
echo $SHELL
```

根据 Shell 类型，运行对应命令将 fnm 注入配置：

```zsh
# Zsh
echo 'eval "$(fnm env --use-on-cd)"' >> ~/.zshrc

# Bash
echo 'eval "$(fnm env --use-on-cd)"' >> ~/.bashrc
```

**第二步：使配置立即生效**

```zsh
source ~/.zshrc
```

Bash 用户请替换为 `source ~/.bashrc`。

**第三步：验证**

```zsh
# 检查 node 路径是否正确指向 fnm 目录
which node

# 检查版本号
node -v
npm -v
```

验证成功时，终端会输出类似 `/Users/您的用户名/.local/share/fnm/.../node` 的路径及版本号。

> [!warning] 注意事项
> 如果执行 `source` 后仍提示 `node not found`，请检查：
> - Shell 类型是否正确（运行 `echo $SHELL` 确认）
> - 配置文件是否写入到了正确的路径（`~/.zshrc` 或 `~/.bashrc`）
