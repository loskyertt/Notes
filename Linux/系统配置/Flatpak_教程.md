---
title: Flatpak 使用教程
date: 2024-09-11 10:32:38
tags:
  - linux/系统配置
  - flatpak
  - 包管理
---

# 1. 初始配置

## 1.1 添加仓库

- 给系统级安装应用添加 Flathub 仓库：

```bash
flatpak remote-add --if-not-exists flathub https://flathub.org/repo/flathub.flatpakrepo
```

- 给用户级也添加 Flathub 仓库：

```bash
flatpak remote-add --user --if-not-exists flathub https://flathub.org/repo/flathub.flatpakrepo
```

---

# 2. 基础管理命令

## 2.1 核心命令

| **操作** | **命令** | **备注** |
|---|---|---|
| **搜索应用** | `flatpak search 关键词` | 比如 `flatpak search qq` |
| **运行应用** | `flatpak run 应用ID` | 命令行启动可以看到报错日志 |
| **更新应用** | `flatpak update` | 一键更新所有已安装的 Flatpak 应用 |

## 2.2 安装应用

> [!tip] 安装后可能需要重启系统才能在桌面环境的应用菜单栏显示图标。

1. 系统级安装 (System-wide)

这是默认的安装方式，安装路径为 `/var/lib/flatpak/`：

```bash
flatpak install flathub org.mozilla.firefox
```

2. 用户级安装 (User-specific)

只为当前登录的用户安装，安装路径为 `~/.local/share/flatpak/`：

```bash
flatpak install --user flathub org.mozilla.firefox
```

> [!info] 个人数据存放位置
> 无论你把程序装在系统级还是用户级，它们产生的**个人数据**（比如浏览记录、登录状态）==永远存放在 `~/.var/app/`==。

## 2.3 查看应用

```bash
flatpak list
```

可以使用 `--app` 参数只查看具体的应用程序。输出示例：

| **Name** | **Application ID** | **Version** | **Branch** | **Installation** |
|---|---|---|---|---|
| Firefox | org.mozilla.firefox | 122.0 | stable | system |
| Discord | com.discordapp.Discord | 0.0.43 | stable | user |

- **Application ID**：应用的"身份证号"，卸载或更新时通常用这个 ID。
- **Installation**：显示该应用是安装在系统级（`system`）还是用户级（`user`）。

查看应用占用磁盘空间：

```bash
flatpak list --columns=name,size
```

查看更完整的信息：`flatpak list -d`

> [!tip] 推荐工具：Flatseal
> 如果你使用的是 GNOME，可以安装 **Flatseal** 专门管理 Flatpak Apps。它不仅能列出所有已安装的 Flatpak 应用，还能让你以开关的形式管理它们的权限（比如是否允许访问摄像头、下载目录或外部驱动器）。

## 2.4 卸载应用

1. **卸载指定程序：**

```bash
flatpak uninstall org.example.App
```

可以加参数 `--delete-data` 清理数据。

2. **清理不再需要的依赖（运行时环境）：**

```bash
flatpak uninstall --unused
```

3. **清除用户配置数据（可选）：**

卸载程序默认会保留你的存档和设置。如果你确定不要了：

```bash
rm -rf ~/.var/app/org.example.App
```

4. **卸载全部应用：**

卸载所有应用，并删除它们在 `~/.var/app` 下的配置和数据：

```bash
flatpak uninstall --all --delete-data
```

---

# 3. 问题汇总

## 3.1 应用菜单栏没有显示应用图标

- **方法一（推荐）：注销并重新登录或重启电脑**

大多数现代桌面环境（如 GNOME, KDE）在检测到 Flatpak 安装后，会在下次登录时自动把这些路径加入环境变量。

- **方法二：立即生效（手动刷新环境变量）**

如果你不想重启，可以尝试在终端执行以下命令：

```bash
export XDG_DATA_DIRS="$HOME/.local/share/flatpak/exports/share:$XDG_DATA_DIRS"
```

> [!warning] 注意
> 这只在当前终端窗口有效。

- **方法三：永久修复（如果重启后依然无效）**

把路径写入 shell 配置文件（如 `.bashrc` 或 `.zshrc`）：

```bash
export XDG_DATA_DIRS="$HOME/.local/share/flatpak/exports/share:/var/lib/flatpak/exports/share:$XDG_DATA_DIRS"
```

保存后运行 `source ~/.bashrc`（或 `source ~/.zshrc`）。

---

# 4. 卸载 flatpak

通过包管理器卸载 flatpak 后，需要清理以下三个目录：

```bash
rm -rf ~/.local/share/flatpak
rm -rf ~/.var/app
sudo rm -rf /var/lib/flatpak
```
