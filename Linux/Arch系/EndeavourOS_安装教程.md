---
title: EndeavourOS 使用记录
date: 2024-07-18 16:21:15
excerpt: "这篇文章主要记录自己使用EndeavourOS Linux（Arch系的发行版）的过程，有一些配置，以及遇到的一些问题和解决办法。理论上这些解决办法在其它Linux发行版下也适用。"
categories: "Linux教程"
---


# 1. 安装

首先是进入 live CD 安装界面。

1. 修改 `/etc/pacman.d/mirrorlist`，把里面其他镜像源都删除，并添加国内的镜像源：

```bash
Server = https://mirror.nju.edu.cn/archlinux/$repo/os/$arch
```

2. 修改 `/etc/pacman.d/endeavouros-mirrorlist`，同样把里面其他镜像源删除，只留国内的镜像源即可。

> 可以在 [MirrorZ Help](https://help.mirror.nju.edu.cn/) 中查看所有镜像源。

3. 初始化密钥环并刷新数据库：

```bash
sudo pacman-key --init; sudo pacman -Sy
````

4. 接下来根据 GUI 引导步骤，正常安装即可。推荐选择 No Desktop 这一选项，后续进入系统后单独安装桌面。

分区和逻辑格式化说明：

| 分区 | 大小 | 文件系统 | 挂载点 |
|------|------|----------|--------|
| EFI System | 512M | fat32 | /boot/efi |
| Linux filesystem | 剩余全部 | btrfs | / |

> EFI 分区大小设置在 `400M - 1GB` 之间即可。`/boot` 目录里面有内核和初始化镜像，挂到 `/boot/efi` 可以把引导文件和这些文件分开存储。

> btrfs 文件系统提供快照、透明压缩和 RAID 功能。可以延长磁盘寿命，尤其是对于 SSD。

不建议再设置交换分区，可以用 [交换文件（Swapfile）](https://wiki.archlinuxcn.org/wiki/Swap) 或者 [ZRAM](https://wiki.archlinuxcn.org/wiki/Zram) 替代。

# 2. 显卡驱动

>  EndeavourOS 提供了一个 `nvidia-inst` 脚本，它可以自动检测你的显卡型号，并安装合适版本的 NVIDIA 驱动程序。因此在安装该系统时，选择有 Nvidia 那一行的进行安装即可。

> CUDA 安装参考这篇教程：[教你在Arch Linux安裝Nvidia、CUDA、cuDNN、TensorRT专有驱动程序](https://ivonblog.com/posts/archlinux-install-nvidia-drivers/)

## 2.1 安装 Nvidia 显卡

显卡驱动的选择在[CodeNames · freedesktop.org](https://nouveau.freedesktop.org/CodeNames.html)这个页面搜索自己的显卡，看看对应的 family 是什么。然后在 [NVIDIA - ArchWiki](https://wiki.archlinux.org/title/NVIDIA) 这个页面查找对应的显卡驱动。nv160family（差不多16系）往后的显卡用 `nvidia-open`。`nvidia-open` 是内核模块开源的驱动，不是完全的开源驱动。非标准内核要安装的驱动包的后缀不一样，像 zen 内核这样的自定义内核要装 `nvidia-open-dkms`，具体看 [NVIDIA - ArchWiki](https://wiki.archlinux.org/title/NVIDIA)。除此之外还要安装 `nvidia-utils` 工具集。在 Arch 上会作为驱动包的依赖安装，但以防万一可以手动加上。

```bash
sudo pacman -S nvidia-open nvidia-utils lib32-nvidia-utils
```

终端输入：

```bash
nvidia-smi
```

如果正确的返回信息，表示安装成功。

## 2.2 硬件视频加速

参考 [Arch Wiki 硬件视频加速](https://wiki.archlinux.org/title/Hardware_video_acceleration)。


## 2.3 补充说明

### 2.3.1 nvidia-utils

`nvidia-utils` 包含 NVIDIA 驱动程序的核心用户空间组件和实用工具。这个包提供了核心的 NVIDIA 驱动库。包含用于查询和配置 NVIDIA 显卡的命令行工具（如 `nvidia-smi`）和  OpenGL 库和 Vulkan 库，用于支持高性能图形渲染。

### 2.3.2 lib32-nvidia-utils

`lib32-nvidia-utils` 是 32 位 NVIDIA 驱动程序库的集合，用于在 64 位系统上运行 32 位应用程序（如一些游戏和软件）。

### 2.3.3 nvidia-settings

`nvidia-settings` 是一个独立的图形化配置工具，用于调整和配置 NVIDIA 显卡的各种设置。

---

# 3. 桌面环境配置

> 我个人比较喜欢 GNOME 桌面，因为配置起来比较简单，而且配置选项比较少，不像 KDE 那么让人眼花缭乱。

- [KDE 桌面配置](KDE_桌面配置.md)

## 3.1 安装 GNOME 桌面

下面指令实现最小化安装 GNOME 桌面：

```bash
sudo pacman -S gdm gnome-control-center gnome-settings-daemon gnome-keyring
```

这几个软件包是 GNOME 桌面所必须的。

```bash
sudo pacman -S flatpak ghostty file-roller firefox
```

> flatpak ：一款安装软件的工具，aur 搭配 Flathub 仓库，足够解决日常软件使用需求了；
> ghostty ：一款终端模拟器；
> file-roller ：和gnome的文档管理器集成的压缩解压缩工具。

## 3.2 添加仓库

进入桌面环境后，推荐添加这两个软件包仓库：

- [Arch Linux CN 软件仓库镜像使用帮助](https://help.mirror.nju.edu.cn/archlinuxcn/)。

- [Chaotic-AUR](https://aur.chaotic.cx/docs)。该仓库建议配置好网络代理后再安装。

## 3.3 网络代理配置

- mihomo 裸核使用方法：
	- 方式一：
		- [直接安装在系统中（已通过 Gitee 加速](https://gitee.com/loskyertt/mihomo)
		- [直接安装在系统中 - Github](https://github.com/loskyertt/mihomo)
	- 方式二：[搭配 docker 使用](mihomo_教程.md)

> 我个人觉得，不管是 clash-verge-rev 还是 clash-party，这类基于 mihomo 内核 GUI 代理软件或多或少都有问题，而且还比较占资源，远没有 mihomo 裸核轻量和稳定。

## 3.4 Flatpak 使用方法

点击跳转：[Flatpak 使用方法](Linux/flatpak_使用方法.md)。

## 3.5 推荐应用

**其他推荐应用如下**：
- extension-manager : 扩展管理工具
- gnome-tweaks : GNOME 调整工具，也可以换成 refine。
- mission-center : 任务管理器。
- gnome-console : GNOME 终端。
- gearlever : AppImages 管理工具，可以把 `.appimage` 软件拖进 gearlever 中进行安装。
- evince : pdf 阅读器
- gnome-text-editor : 文本编辑器。也可以安装 gedit。
- gnome-font-viewer : 字体查看器。
- showtime : 视频播放器。

> 强烈建议用户级软件包使用 flatpak 来安装。[flatpak 使用方法](Linux/flatpak_使用方法.md)。

## 3.6 安装字体

```bash
sudo pacman -S wqy-zenhei noto-fonts noto-fonts-emoji
```

还有 Maple-Mono 字体也不错，下载方式具体看 [maple-font](https://github.com/subframe7536/maple-font/tree/variable)，需要先配置好 **archlinuxcn 镜像仓库**。

```bash
sudo pacman -S ttf-maplemono ttf-maplemono-nf-unhinted ttf-maplemono-nf-cn-unhinted
```

还有其他字体，比如 MiSans 或者 HarmonyOS Sans，配合字体 DConf 字体缩放可以在高分屏上达到非常细腻的显示效果。

---

# 4. 软件包配置

## 4.1 中文输入法配置

> Fcitx5 推荐非 GNOME 桌面使用（当然，也可以在 GNOME 下使用）；Ibus 推荐在 GNOME 桌面下使用，基本上不需要配置环境变量。

### 4.1.1 方式一：Fcitx5

> 参考：[Fcitx5 Arch Wiki](https://wiki.archlinuxcn.org/wiki/Fcitx5)。

#### 4.1.1.1 安装

```bash
sudo pacman -S fcitx5-im fcitx5-rime noto-fonts-emoji
```

> 补充安装（可不选，因为要使用雾凇拼音）
> 
> - `fcitx5-chinese-addons`：包含与中文相关的 addon，例如拼音、双拼和五笔。
> - `fcitx5-pinyin-zhwiki`：中文拼音词库。
> - `fcitx5-configtool`：一般会包含在 `fcitx5-im` 中，如果没有，再单独安装该配置工具。

#### 4.1.1.2 配置环境变量

**不同的桌面设置方法不同，在 `/etc/environment` 中添加下面内容**：

- GNOME 桌面环境：

```bash
XIM="fcitx"
GTK_IM_MODULE=fcitx
QT_IM_MODULE=fcitx
XMODIFIERS=@im=fcitx
XDG_CURRENT_DESKTOP=GNOME
```

`GTK_IM_MODULE=fcitx`，`QT_IM_MODULE=fcitx`，`XMODIFIERS=@im=fcitx` 这三个是最主要的环境变量，`XDG_CURRENT_DESKTOP=GNOME` 这一项可以解决吞字问题。

- KDE/Wayland 合成器的话写入：

```
XMODIFIERS=@im=fcitx
```

#### 4.1.1.3 配置 rime-ice（雾凇）拼音

去 [rime-ice](https://github.com/ivel/rime-ice) 仓库下载源码文件，把下载好的所有文件（除了 `.git`）全部放到 `~/.local/share/fcitx5/rime/` 目录下即可。

### 4.1.2 方式二：Ibus

> 参考：[IBus Arch Wiki](https://wiki.archlinuxcn.org/wiki/IBus)。

#### 4.1.2.1 安装

```bash
sudo pacman -S ibus ibus-rime noto-fonts-emoji
```

安装好重启后，IBus 引擎才会生效，你需要手动在图形界面添加它：

1. 打开 **GNOME 设置 (Settings)** -> **键盘 (Keyboard)**。
2. 找到 **输入源 (Input Sources)**，点击 **+ (添加)**。
3. 点击 **汉语 (Chinese)**，在列表中找到 **Chinese (Rime)** 并添加。

#### 4.1.2.2 配置 rime-ice（雾凇）拼音

去 [rime-ice 仓库](https://github.com/iDvel/rime-ice) 下载源码文件，把下载好的所有文件（除了 `.git`）全部放到 `~/.config/ibus/rime` 目录下即可。

## 4.2 zsh 配置

1. 安装 `zimfw`：

```bash
# 通过 curl
curl -fsSL https://raw.githubusercontent.com/zimfw/install/master/install.zsh | zsh

# 或者通过 wget
wget -nv -O - https://raw.githubusercontent.com/zimfw/install/master/install.zsh | zsh
```

2. 配置插件。在 `~/.zimrc` 中添加：

```zsh
zmodule zdharma-continuum/fast-syntax-highlighting
zmodule prompt-pwd
# 主题
zmodule steeef
```

> 需要把原来的 syntax highlighting 插件删了。

执行 `zimfw install` 安装好并重启终端后即可使用。

3. 可以安装 [starship](https://starship.rs/zh-cn/) 进行美化：

```bash
sudo pacman -S starship
```

在 `.zshrc` 中填入：

```zsh
# ~/.zshrc

eval "$(starship init zsh)"
```

---

# 5. 用户配置

## 5.1 密码反馈设置

设置在终端输入密码时，显示星号：

```bash
sudo EDITOR=nano visudo
```

> 使用 nano 进行编辑。

在打开的文件中找到以 `Defaults` 开头的行。如果看到 `Defaults env_reset`，将其修改为：

```bash
Defaults env_reset, pwfeedback
```

如果没有看到该行，直接在文件末尾添加一行：

```bash
Defaults pwfeedback
```

---

# 6. 音视频固件和服务

让音频设备和屏幕分享正常工作。

1. 可选：安装音视频固件

```bash
sudo pacman -S --needed sof-firmware alsa-ucm-conf alsa-firmware 
```

> `sof-firmware` 为现代音视频设备提供固件，通常装这个就可以了。
> `alsa-ucm-conf` 提供必要的配置文件。
> `alsa-firmware` 为不常见或者较旧的设备提供固件。

2. 安装音视频服务

```bash
sudo pacman -S --needed pipewire wireplumber pipewire-pulse pipewire-alsa pipewire-jack 
```

> `pipewire` 是由 redhat 主导开发的现代音视频服务
> `wireplumber` 会智能管理pipewire
> `pipewire-pulse pipewire-alsa pipewire-jack` 分别为 pulseAudio、ALSA、JACK 提供兼容。

3. 启用服务

```bash
systemctl --user enable --now pipewire pipewire-pulse wireplumber
```

---

# 7. 性能模式切换

`power-profiles-daemon` 是各个桌面环境通用的性能模式切换服务，有三个档位，performance 性能、balance 平衡、powersave 节电。一般平衡档位就够用了，也不需要调节风扇什么的。

1. 安装

```bash
sudo pacman -S power-profiles-daemon
```

2. 启动服务

```bash
sudo systemctl enable --now power-profiles-daemon 
```

这个易用而且足够，不建议使用 tlp 或者 auto-cpufreq，功耗上不会有明显区别。

# 8. 蓝牙

1. 安装

```bash
sudo pacman -S --needed bluez
```

1. 启动服务

```bash
sudo systemctl enable --now bluetooth
```

---

# 7. 问题汇总

## 7.1 安装软件时的证书问题

1. 首先，更新你的系统证书:

```bash
sudo update-ca-trust
```

2. 如果上面的方法不起作用，你可以尝试临时禁用 SSL 验证。这不是一个安全的长期解决方案，但可以帮助你确定问题是否确实与证书有关:

```bash
makepkg -si --skippgpcheck
```

3. 确保你的系统时间是正确的，因为时间不正确也可能导致 SSL 证书验证失败:

```bash
timedatectl status
```

如果时间不正确，可以同步:

```bash
sudo timedatectl set-ntp true
```

## 7.2 签名验证问题

**建议先参考这篇官方文章**： [Signature and keyring](https://discovery.endeavouros.com/signature-and-keyring/pacman-keyring-issues/2021/03/)

```txt
(735/735) 正在检查软件包完整性                        [------------------------------------] 100% **错误：**libinstpatch: 来自 "Brett Cornwall <brett@i--b.com>" 的签名是未知信任的- **:: 文件 /var/cache/pacman/pkg/libinstpatch-1.1.6-3-x86_64.pkg.tar.zst 已损坏 (无效或已损坏的软件包 (PGP 签名))**. **打算删除吗？ [Y/n] ** **错误：**fluidsynth: 来自 "Brett Cornwall <brett@i--b.com>" 的签名是未知信任的**坏** **:: 文件 /var/cache/pacman/pkg/fluidsynth-2.3.6-1-x86_64.pkg.tar.zst 已损坏 (无效或已损坏的软件包 (PGP 签名)).** **打算删除吗？ [Y/n]
```

如果遇到以上这种问题（比如在执行 `sudo pacman -Syu` 时），可以这样做：

```bash
nano /etc/pacman.conf
```

然后找到 `SigLevel` 那一行，暂时禁用签名检查（这是一个临时的、有风险的解决方案），修改为：

```txt
SigLevel = Never
```

为了保证主机安全，在安装好后记得要修改回来！

## 7.3 文件未通过校验

比如在构建包时有类似的如下报错信息：

```txt
 正在验证 source 文件，
 使用sha512sums... 
  nekoray ... 通过
  nekobox.sh ... 通过
  nekobox.desktop ... 通过
  1350.patch ... 失败. ==> 错误： 一个或多个文件没有通过有效性检查！0
```

1. 清理之前的构建：

```bash
makepkg --clean
```

2. 更新 PKGBUILD 文件中的校验和。进入构建文件的目录，然后使用以下命令：

```bash
updpkgsums
```

它会自动下载源文件（如果需要），计算新的校验和，并更新 PKGBUILD 文件。

3. 重新构建：

```bash
makepkg -si
```
