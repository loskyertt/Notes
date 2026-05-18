---
title: Linux 如何安装 KDE 桌面
date: 2024-12-05 23:52:32
excerpt: "这篇文章是关于如何最小化安装 kde plasma 桌面的，注意：是最小化安装。这里用的发行本是 Arco Linux，我感觉比 EndeavourOS 好用。"
categories: "Linux教程"
---


# 1.安装教程

前提是确保驱动已经安装好，并且执行了 `pacman -Syu` 进行更新。

安装 `x11` 会话服务：

```bash
sudo pacman -S xorg-server
```

安装 `wayland` 会话（如果只使用 `x11` 的话，可以不选）：

```bash
sudo pacman -S wayland xorg-xwayland
```

不过我建议还是使用 `wayland` 会话好。

最小化安装 plasma 桌面：

```bash
sudo pacman -S plasma-desktop
```

安装必要的组件：

```bash
sudo pacman -S sddm dolphin konsole
```

- `sddm`： `KDE` 推荐的显示管理器，如果只使用 `x11` 会话，可以不安装，如果要使用 `wayland` 会话，则建议安装好。
- `dolphin`：文件管理器。
- `konsole`：终端模拟器。

启动 `sddm` 服务：

```bash
sudo systemctl enable --now sddm.service
```

如何更改 `sddm` 主题，请查看 `Arch Wiki`：[SDDM](https://wiki.archlinuxcn.org/wiki/SDDM)

安装音频服务：

```bash
sudo pacman -S pipewire pipewire-pulse pipewire-alsa
```

对 `jack` 应用有要求的话，可以再安装个 `pipewire-jack`。

安装 `kde` 音频控件：

```bash
sudo pacman -S plasma-pa
```

启动音频服务：

```bash
systemctl --user enable pipewire pipewire-pulse
systemctl --user start pipewire pipewire-pulse
```

确保安装后运行以下命令检查未使用的包然后并删除：

```bash
sudo pacman -Qtdq | sudo pacman -Rns -
```

还有需要的在这基础上补充就行了，这里只提供桌面环境必要的安装。可以参考`Arch Wiki`：[KDE](https://wiki.archlinuxcn.org/wiki/KDE)

---

# 2.针对 x11 的补充

如果只用 `x11` 的话，又不想安装 `sddm`，可以通过下面的方式启动 `x11` 会话窗口。默认是先进入的 `tty`。

创建 `~/.xinitrc` 文件：

```bash
touch ~/.xinitrc
```

添加下面内容（直接执行该命令）：

```bash
echo "exec startplasma-x11" > ~/.xinitrc
```

> ~~可以添加 `exec startplasma-wayland` 来使得启动的会话窗口是 `wayland`。~~

然后启动 `plasma`：

```bash
startx
```

---

# 3.卸载多余的图标和鼠标

使用下面的指令可以列出已安装的图标：

```bash
sudo pacman -Q | grep icon
```

比如输出是：

```bash
breeze-icons 5.24.5-1
papirus-icon-theme 20211029-1
hicolor-icon-theme 0.17-2
```

使用下面的指令可以列出已安装的鼠标主题：

```bash
sudo pacman -Q | grep cursor
```

比如输出：

```bash
breeze-cursor-theme 5.24.5-1
xcursor-breeze 1.0.3-1
adwaita-icon-theme 3.38.0-1
```

---

# 4.美化

> 注意：下载的主题和组件的版本需要是 `plasma6` 的，作者一般会说明。

## 4.1 全局主题

建议去 KDE Store 下载主题，然后把下载的 **全局主题** 解压后，放到 `~/.local/share/plasma/look-and-feel` 目录下。

壁纸和全局主题推荐 `Utterly Nord`，图标推荐 `Dracula Circle`。

参考图：
[![Utterly Nord](https://s21.ax1x.com/2024/08/25/pAkSS6H.png)](https://imgse.com/i/pAkSS6H)

## 4.2 组件

[musicWaves](https://store.kde.org/p/2186442)
[thot CPU Plasma 6](https://www.pling.com/p/2148472/)

解压后，把文件夹移动到 `~/.local/share/plasma/plasmoids/` 目录下。

参考图：
[![Desktop Widgets](https://s21.ax1x.com/2024/08/27/pAksXO1.md.png)](https://imgse.com/i/pAksXO1)

## 4.3 Login SDDM

建议先在 KDE Store 下载好，然后解压到存放登录界面的目录下。

[Nordic sddm Plasma 6](https://www.pling.com/p/2144235/)

把解压后的文件移到 `/usr/share/sddm/themes/` 目录下。