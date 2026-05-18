---
title: pacman 包管理器使用教程
date: 2024-07-24 08:51:17
excerpt: "发行版是 EndeavourOS"
cover: https://softwarelab.org/wp-content/uploads/Linux.jpg
categories: "Linux教程"
---


# 1.介绍

  在 EndeavouOS（以及其他基于 Arch 的发行版）中，`pacman` 是一个用于软件包管理的命令行工具。下面是 `pacman` 命令的详细解释：

   - `-S`：用于安装软件包。
   - `-y`：强制刷新软件包数据库。
   - `-yy`：强制刷新所有的软件包数据库。通常只需要一个 `-y` 就足够，但 `-yy` 用于解决某些情况下可能出现的数据库同步问题。
   - `-u`：更新所有已安装的软件包，用于更新系统或者`pacman`包含的软件以及以来库。

对于所有用 `pacman` 进行的操作，都可以通过 `yay` 实现：

```bash
sudo pacman yay
```

`yay`支持的软件多一些（通过`aur`源）。

---

# 2.下载和更新

- **要更新或下载指定的软件包：**

```bash
sudo pacman -S 包名
```

`pacman` 会检查你指定的软件包是否有新版本，如果有的话，就会下载并安装更新后的版本。

---

# 3.查看包/库信息

- **查看包名信息**：

显示指定软件包的信息，但不会安装或更新。这个选项可以用来查看某个软件包的详细信息，包括它的版本、依赖关系等。例如：

```bash
sudo pacman -Si <pkgname>
```

查看包的简略信息：

```bash
yay -Qs <pkgname>
```

查看包的详细信息：

```bash
yay -Qi <pkgname>
```

- **查看可升级的包/库：**

列出有可用更新的已安装软件包及其最新版本。

```bash
sudo pacman -Qu
```

列出所有的外部软件包（即非官方仓库安装的包，如`AUR`软件包）：

```bash
sudo pacman -Qm
```

---

# 4.卸载

## 4.1 卸载单个软件包

要卸载单个软件包，可以使用以下命令：

```bash
sudo pacman -R package_name
```

## 4.2 卸载软件包及其未使用的依赖

有时卸载一个包后，它的一些依赖包可能不再被其他软件包使用。要卸载软件包及其未使用的依赖，可以使用以下命令：

```bash
sudo pacman -Rns package_name
```

解释：
- `-R`(--remove)：卸载指定的包。
- `-n`(--nosave)：从系统中删除安装包的所有配置文件。
- `-s`(--recursive)：递归地卸载未使用的依赖包。

## 4.3 强制卸载（不推荐）

在极少数情况下，可能需要强制卸载一个包，即使这可能会破坏系统的依赖关系。请谨慎使用此选项：

```bash
sudo pacman -Rdd package_name
```

解释：
- `-d`：忽略依赖关系检查。

## 4.4 清理未使用的孤立包

系统中可能会有一些未使用的孤立包，这些包是作为依赖安装的，但现在没有任何包依赖它们。可以使用以下命令清理这些孤立包：

```bash
sudo pacman -Rns $(pacman -Qtdq)
```

解释：
- `pacman -Qtdq`：列出所有未使用的孤立包。
- `-Rns`：递归地卸载未使用的包及其配置文件。

---

# 5.查看安装包的文件及其路径

```bash
sudo pacman -Ql boost
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
sudo pacman -Ql boost | grep cmake
```

在 `grep` 后加上 `-i` 参数可以忽略大小写进行匹配查找。

---

# 6. 提高编译速度

可以通过设置更多的并行编译任务来加速编译过程。使用与 CPU 核心数量相等或更多的并行任务数。设置 `MAKEFLAGS` 来实现这一点：

编辑 /etc/makepkg.conf 文件，找到以下行并进行修改：

```
MAKEFLAGS="-j$(nproc)"
```

`$(nproc)`会自动检测 CPU 核心数，并设置相同数量的并行任务。包括在执行`make`指令时，可以通过加`-j<核心数>`来手动指定编译时用CPU的核心数。

---

# 7.问题汇总

## 7.1 PGP 签名问题

有时候执行 `pacman -Syu` 时，会出现这种问题：

```bash
invalid or corrupted package (PGP signature) error
```

解决办法：

```bash
sudo pacman -S archlinux-keyring

sudo pacman -S pacman-key --init

sudo pacman -S pacman-key --populate
```

一般执行这两条命令后，就能成功解决了，如果还不行，就手动删除 `corrupted pkgs`（提示所损坏的包）。