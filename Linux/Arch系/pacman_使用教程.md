---
title: pacman 包管理器使用教程
date: 2024-07-24 08:51:17
description: "Arch Linux 及其衍生发行版中 pacman 包管理器的详细使用教程"
tags:
  - Linux
  - Arch
  - pacman
  - 包管理
aliases:
  - pacman使用教程
---


# 1. 介绍

在 Arch Linux 及其衍生发行版（如 EndeavourOS、Manjaro 等）中，`pacman` 是默认的包管理器。下面是 `pacman` 常用选项的详细解释：

- `-S`：同步模式，用于从远程仓库安装软件包。
- `-y`：刷新本地软件包数据库（从服务器下载最新的包列表）。
- `-yy`：强制刷新所有软件包数据库。通常只需要一个 `-y` 就足够，但 `-yy` 用于解决某些情况下可能出现的数据库同步问题。
- `-u`：升级所有已安装的软件包到最新版本。
- `-R`：移除模式，用于卸载软件包。
- `-Q`：查询模式，用于查看本地已安装的软件包信息。

> [!tip] 常用组合
> - `pacman -Syu`：刷新数据库并升级所有包（==最常用的系统更新命令==）。
> - `pacman -S <包名>`：安装指定软件包。
> - `pacman -Rns <包名>`：卸载软件包及其配置和未使用的依赖。

## 1.1 AUR 助手

`pacman` 只能安装官方仓库中的软件包。对于 Arch User Repository（AUR）中的软件，需要使用 AUR 助手：

- **yay**：最流行的 AUR 助手，用 Go 编写。
- **paru**：用 Rust 编写的 AUR 助手，更注重安全。

安装 yay：

```bash
sudo pacman -S yay
```

安装 paru：

```bash
sudo pacman -S paru
```

> [!note] yay 和 paru 的用法
> yay 和 paru 的命令语法与 pacman 基本兼容，可以直接替换 `pacman` 使用：
> ```bash
> yay -S <包名>      # 从 AUR 或官方仓库安装
> yay -Syu           # 更新系统（包括 AUR 包）
> yay -Rns <包名>    # 卸载（实际调用 pacman）
> ```

---

# 2. 下载和更新

## 2.1 安装软件包

```bash
sudo pacman -S 包名
```

`pacman` 会检查你指定的软件包是否有新版本，如果有的话，就会下载并安装更新后的版本。

> [!tip] 安装包组
> 某些软件以包组（group）的形式提供，安装时会提示选择组中的具体包。例如：
> ```bash
> sudo pacman -S xorg
> ```
> 如果想跳过选择提示，安装整个包组：
> ```bash
> sudo pacman -S --needed xorg
> ```

## 2.2 更新系统

```bash
sudo pacman -Syu
```

> [!warning] 避免部分升级
> 不要使用 `pacman -Sy <包名>` 来安装软件包（只刷新数据库而不升级系统），这可能导致依赖关系破坏。正确的做法是先 `pacman -Syu` 完整升级系统，再安装新包。详见 [Arch Wiki - 部分升级不受支持](https://wiki.archlinuxcn.org/wiki/System_maintenance#部分升级不受支持)。

## 2.3 下载但不安装

```bash
sudo pacman -Sw 包名
```

包会被下载到缓存目录 `/var/cache/pacman/pkg/`，但不安装。

---

# 3. 查看包/库信息

## 3.1 查看远程仓库中的包信息

显示远程仓库中指定软件包的信息（版本、依赖关系、描述等），但不会安装或更新：

```bash
pacman -Si <pkgname>
```

## 3.2 查看已安装包的信息

查看已安装包的详细信息：

```bash
pacman -Qi <pkgname>
```

查看已安装包的简略信息（模糊搜索）：

```bash
pacman -Qs <关键词>
```

> [!tip] yay 查询
> yay 也支持类似的查询：
> ```bash
> yay -Si <pkgname>   # 查询远程仓库（包括 AUR）
> yay -Qi <pkgname>   # 查询已安装包
> yay -Qs <关键词>     # 模糊搜索已安装包
> ```

## 3.3 查看可升级的包

列出有可用更新的已安装软件包：

```bash
pacman -Qu
```

## 3.4 查看外部软件包

列出所有非官方仓库安装的包（如 AUR 软件包）：

```bash
pacman -Qm
```

## 3.5 查看包所属的仓库

```bash
pacman -Qo <文件路径>
```

例如：`pacman -Qo /usr/bin/pacman` 可以查看某个文件属于哪个包。

---

# 4. 卸载

## 4.1 卸载单个软件包

要卸载单个软件包，保留其配置文件和依赖：

```bash
sudo pacman -R package_name
```

## 4.2 卸载软件包及其未使用的依赖

有时卸载一个包后，它的一些依赖包可能不再被其他软件包使用。要卸载软件包及其未使用的依赖和配置文件：

```bash
sudo pacman -Rns package_name
```

参数解释：
- `-R`（`--remove`）：卸载指定的包。
- `-n`（`--nosave`）：从系统中删除安装包的所有配置文件。
- `-s`（`--recursive`）：递归地卸载未使用的依赖包。

> [!tip] 推荐使用 -Rns
> `pacman -Rns` 是最常用的卸载方式，能干净地移除包及其不再需要的依赖和配置文件。

## 4.3 强制卸载（不推荐）

在极少数情况下，可能需要强制卸载一个包，即使这可能会破坏系统的依赖关系。请谨慎使用此选项：

```bash
sudo pacman -Rdd package_name
```

参数解释：
- `-d`（`--nodeps`）：跳过依赖关系检查。
- 第二个 `d`：同时跳过被其他包依赖的检查。

> [!danger] 危险操作
> 强制卸载可能导致系统中的其他包因缺少依赖而无法正常工作。仅在明确知道后果的情况下使用，卸载后应尽快修复依赖关系。

## 4.4 清理未使用的孤立包

系统中可能会有一些未使用的孤立包，这些包是作为依赖安装的，但现在没有任何包依赖它们。可以使用以下命令清理这些孤立包：

```bash
sudo pacman -Rns $(pacman -Qtdq)
```

参数解释：
- `pacman -Qtdq`：列出所有未使用的孤立包。
- `-Rns`：递归地卸载未使用的包及其配置文件。

> [!warning] 先检查再删除
> 建议先运行 `pacman -Qtdq` 查看孤立包列表，确认没有误删需要的包，再执行清理命令。如果 `pacman -Qtdq` 没有输出，说明没有孤立包，此时执行上述命令会报错。

---

# 5. 查看安装包的文件及其路径

查看已安装包所包含的所有文件：

```bash
pacman -Ql boost
```

输出示例：

```bash
boost /usr/share/boostbook/xsl/source-highlight.xsl
boost /usr/share/boostbook/xsl/template.xsl
boost /usr/share/boostbook/xsl/testing/
boost /usr/share/boostbook/xsl/testing/Jamfile.xsl
boost /usr/share/boostbook/xsl/testing/testsuite.xsl
boost /usr/share/boostbook/xsl/type.xsl
boost /usr/share/boostbook/xsl/utility.xsl
boost /usr/share/boostbook/xsl/xhtml.xsl
boost /usr/share/boostbook/xsl/xref.xsl
```

可以加上 `grep` 进行匹配：

```bash
pacman -Ql boost | grep cmake
```

在 `grep` 后加上 `-i` 参数可以忽略大小写进行匹配查找。

> [!tip] 查询未安装包的文件列表
> 如果想查看远程仓库中尚未安装的包包含哪些文件，可以使用：
> ```bash
> pacman -Fl <pkgname>
> ```
> 需要先安装 `pacman-contrib` 包并更新文件数据库：
> ```bash
> sudo pacman -S pacman-contrib
> sudo pacman -Fy
> ```

---

# 6. 清理包缓存

`pacman` 下载的包会缓存在 `/var/cache/pacman/pkg/` 中，随着时间推移会占用大量磁盘空间。

## 6.1 清理旧版本缓存

保留最近 3 个版本的缓存：

```bash
sudo paccache -r
```

保留最近 1 个版本：

```bash
sudo paccache -rk1
```

## 6.2 清理所有缓存

删除所有缓存中的包文件（当前未安装的）：

```bash
sudo pacman -Sc
```

删除所有缓存（包括当前安装的包的缓存）：

```bash
sudo pacman -Scc
```

> [!warning] 谨慎清理
> `pacman -Scc` 会删除所有缓存，包括当前安装包的缓存。如果之后需要降级某个包，将不得不重新下载。建议使用 `paccache` 保留最近几个版本。

---

# 7. 提高编译速度

可以通过设置更多的并行编译任务来加速编译过程。使用与 CPU 核心数量相等或更多的并行任务数。设置 `MAKEFLAGS` 来实现这一点：

编辑 `/etc/makepkg.conf` 文件，找到以下行并进行修改：

```bash
MAKEFLAGS="-j$(nproc)"
```

`$(nproc)` 会自动检测 CPU 核心数，并设置相同数量的并行任务。包括在执行 `make` 指令时，可以通过加 `-j<核心数>` 来手动指定编译时用 CPU 的核心数。

> [!tip] 临时指定
> 也可以在构建 AUR 包时临时指定：
> ```bash
> MAKEFLAGS="-j$(nproc)" makepkg -si
> ```

---

# 8. 问题汇总

## 8.1 PGP 签名问题

有时候执行 `pacman -Syu` 时，会出现这种问题：

```bash
error: <包名>: signature from "<维护者>" is unknown trust
error: failed to commit transaction (invalid or corrupted package (PGP signature))
```

解决办法：

1. 更新密钥环：

```bash
sudo pacman -S archlinux-keyring endeavouros-keyring
```

2. 重新初始化密钥环：

```bash
sudo pacman-key --init
sudo pacman-key --populate archlinux endeavouros
```

3. 如果还不行，手动删除损坏的包缓存：

```bash
sudo rm /var/cache/pacman/pkg/<损坏的包文件>
```

然后重新执行 `sudo pacman -Syu`。

> [!tip] 更多签名问题解决方案
> 详见 [[EndeavourOS_安装教程#11.2 签名验证问题]]。

## 8.2 数据库锁定问题

如果 `pacman` 操作异常中断，可能会出现以下错误：

```bash
error: failed to initialize alpm library (database already registered)
```

或者：

```bash
error: failed to update (cannot lock database)
```

解决方法：删除锁文件：

```bash
sudo rm /var/lib/pacman/db.lck
```

> [!warning] 确保没有其他 pacman 进程
> 删除锁文件前，请确认没有其他 `pacman` 进程正在运行：
> ```bash
> ps aux | grep pacman
> ```

## 8.3 依赖冲突

安装或更新时出现依赖冲突：

```bash
error: could not satisfy dependencies
```

1. 尝试完整更新系统：

```bash
sudo pacman -Syu
```

2. 如果仍然冲突，查看具体冲突信息，可能需要手动卸载冲突的包再重新安装。

---

# 9. 相关笔记

- [[EndeavourOS_安装教程]] — EndeavourOS 安装与配置
- [Arch Wiki - pacman](https://wiki.archlinuxcn.org/wiki/Pacman) — 官方文档
- [pacman Rosetta](https://wiki.archlinuxcn.org/wiki/Pacman/Rosetta) — 与其他发行版包管理器的命令对照表
