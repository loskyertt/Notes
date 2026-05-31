---
title: EndeavourOS 安装教程
date: 2024-07-18 16:21:15
description: "EndeavourOS Linux（Arch系发行版）的安装与配置教程，涵盖显卡驱动、桌面环境、输入法、音视频服务等配置"
tags:
  - Linux
  - Arch
  - EndeavourOS
  - 安装教程
aliases:
  - EndeavourOS使用记录
---


# 1. 安装

首先是进入 live CD 安装界面。

1. 修改 `/etc/pacman.d/mirrorlist`，把里面其他镜像源都删除，并添加国内的镜像源：

```bash
Server = https://mirror.nju.edu.cn/archlinux/$repo/os/$arch
```

2. 修改 `/etc/pacman.d/endeavouros-mirrorlist`，同样把里面其他镜像源删除，只留国内的镜像源即可。

> [!tip] 镜像源查询
> 可以在 [MirrorZ Help](https://help.mirror.nju.edu.cn/) 中查看所有镜像源。

3. 初始化密钥环并刷新数据库：

```bash
sudo pacman-key --init
sudo pacman -Sy
```

> [!warning] 注意
> `pacman-key --init` 和 `pacman -Sy` 应分开执行，确保密钥环初始化完成后再同步数据库。使用分号 `;` 连接可能导致密钥环尚未初始化完成就执行同步，从而引发签名错误。

4. 接下来根据 GUI 引导步骤，正常安装即可。推荐选择 No Desktop 这一选项，后续进入系统后单独安装桌面。

## 1.1 分区方案

| 分区 | 大小 | 文件系统 | 挂载点 |
|------|------|----------|--------|
| EFI System | 512M | fat32 | /boot/efi |
| Linux filesystem | 剩余全部 | btrfs | / |

> [!info] EFI 分区说明
> - EFI 分区大小设置在 `300M - 1GB` 之间即可，推荐 512M。
> - 挂载点选择：如果使用 **GRUB** 引导器，挂载到 `/boot/efi`；如果使用 **systemd-boot** 引导器，挂载到 `/boot`。EndeavourOS 在线安装（No Desktop）默认使用 systemd-boot，离线安装默认使用 GRUB。
> - `/boot` 目录里面有内核和初始化镜像，挂到 `/boot/efi` 可以把引导文件和这些文件分开存储。

> [!tip] btrfs 文件系统
> btrfs 文件系统提供快照、透明压缩和 RAID 功能，可以延长磁盘寿命，尤其是对于 SSD。安装时可以在挂载选项中添加 `compress=zstd` 启用透明压缩。

> [!warning] 交换分区
> 不建议再设置交换分区，可以用 [交换文件（Swapfile）](https://wiki.archlinuxcn.org/wiki/Swap) 或者 [ZRAM](https://wiki.archlinuxcn.org/wiki/Zram) 替代。
> - **Swapfile**：适合需要休眠（hibernate）功能的场景，但 btrfs 上的 swapfile 配置较为特殊，需要确保文件是 NOCOW 的。
> - **ZRAM**：在内存中创建压缩的交换空间，速度更快，适合内存充足的场景，但不支持休眠。

## 1.2 引导器选择

EndeavourOS 安装程序支持两种引导器：

- **systemd-boot**：在线安装（选择 No Desktop）时的默认引导器，配置简单，启动速度快，但不支持自动检测其他操作系统。
- **GRUB**：离线安装时的默认引导器，功能更丰富，支持多系统自动检测。

> [!note] 双系统用户
> 如果需要双系统，推荐使用 GRUB，因为它可以自动检测并添加 Windows 启动项。使用 systemd-boot 则需要手动配置。详见 [Grub_配置](../系统配置/Grub_配置)。

## 1.3 安装后必要配置

安装完成进入系统后，需要确认以下服务是否已启用：

```bash
systemctl status NetworkManager
```

如果未启用：

```bash
sudo systemctl enable --now NetworkManager
```

检查时间同步：

```bash
timedatectl status
```

如果 NTP 未启用：

```bash
sudo timedatectl set-ntp true
```

### 1.3.1 启用 multilib 仓库

multilib 仓库提供 32 位软件包，对于运行 Wine、Steam 等软件是必需的：

```bash
sudo nano /etc/pacman.conf
```

找到 `[multilib]` 部分，取消注释：

```ini
[multilib]
Include = /etc/pacman.d/mirrorlist
```

然后同步数据库：

```bash
sudo pacman -Sy
```

### 1.3.2 Locale 与时区

```bash
sudo timedatectl set-timezone Asia/Shanghai
sudo localectl set-locale LANG=en_US.UTF-8
```

> [!tip] Locale 选择
> 推荐系统 Locale 设置为 `en_US.UTF-8`，避免终端中文乱码问题。中文显示由桌面环境和输入法负责，不需要修改系统 Locale。

### 1.3.3 SSD TRIM

如果使用 SSD，建议启用周期性 TRIM 以维持写入性能：

```bash
sudo systemctl enable fstrim.timer
sudo systemctl start fstrim.timer
```

> [!note]
> btrfs 文件系统在挂载时已支持 discard 异步 TRIM（通过挂载选项 `discard=async`），启用 `fstrim.timer` 作为额外保障也是推荐的。

---

# 2. 显卡驱动

> [!note] EndeavourOS NVIDIA 安装
> EndeavourOS 提供了一个 `nvidia-inst` 脚本，它可以自动检测你的显卡型号，并安装合适版本的 NVIDIA 驱动程序。因此在安装该系统时，选择有 Nvidia 那一行的进行安装即可。

> CUDA 安装参考这篇教程：[教你在Arch Linux安裝Nvidia、CUDA、cuDNN、TensorRT专有驱动程序](https://ivonblog.com/posts/archlinux-install-nvidia-drivers/)

## 2.1 安装 Nvidia 显卡

显卡驱动的选择在 [CodeNames · freedesktop.org](https://nouveau.freedesktop.org/CodeNames.html) 这个页面搜索自己的显卡，看看对应的 family 是什么。然后在 [NVIDIA - ArchWiki](https://wiki.archlinux.org/title/NVIDIA) 这个页面查找对应的显卡驱动。

- **Turing（NV160 系列，即 16 系 / 20 系）及更新架构**的显卡使用 `nvidia-open`（内核模块开源的驱动，不是完全的开源驱动，用户空间部分仍为闭源）。
- **Pascal（NV130 系列，即 10 系）及更早架构**的显卡使用 `nvidia`（闭源驱动）。

> [!warning] nvidia-open 注意事项
> `nvidia-open` 在 Linux 内核 6.7+ 和 NVIDIA 驱动 545+ 版本后才可用于桌面 GPU。部分功能（如 VR、某些电源管理特性）可能仍不支持。如果遇到问题，可以回退到闭源 `nvidia` 驱动。

非标准内核要安装的驱动包的后缀不一样，像 zen 内核这样的自定义内核要装 `nvidia-open-dkms`（或 `nvidia-dkms`），具体看 [NVIDIA - ArchWiki](https://wiki.archlinux.org/title/NVIDIA)。除此之外还要安装 `nvidia-utils` 工具集。在 Arch 上会作为驱动包的依赖安装，但以防万一可以手动加上。

```bash
# 闭源驱动（Pascal 及更早）
sudo pacman -S nvidia nvidia-utils lib32-nvidia-utils

# 开源内核模块驱动（Turing 及更新）
sudo pacman -S nvidia-open nvidia-utils lib32-nvidia-utils
```

> [!important] 内核参数
> 安装 NVIDIA 驱动后，建议添加 `nvidia-drm.modeset=1` 内核参数以启用 DRM KMS，这对于 Wayland 会话和更好的电源管理是必需的。
>
> - 如果使用 **systemd-boot**，编辑 `/boot/loader/entries/xxx.conf`，在 `options` 行添加 `nvidia-drm.modeset=1`。
> - 如果使用 **GRUB**，编辑 `/etc/default/grub`，在 `GRUB_CMDLINE_LINUX_DEFAULT` 中添加 `nvidia-drm.modeset=1`，然后执行 `sudo grub-mkconfig -o /boot/grub/grub.cfg`。

安装完成后，终端输入：

```bash
nvidia-smi
```

如果正确返回显卡信息，表示安装成功。

## 2.2 硬件视频加速

参考 [Arch Wiki 硬件视频加速](https://wiki.archlinux.org/title/Hardware_video_acceleration)。

对于 NVIDIA 显卡，硬件视频加速通过 `nvidia-utils` 中包含的 VDPAU 和 NVDEC/NVENC 提供。如果需要 VA-API 兼容层（某些应用需要），可以安装：

```bash
sudo pacman -S libva-nvidia-driver
```

## 2.3 补充说明

### 2.3.1 nvidia-utils

`nvidia-utils` 包含 NVIDIA 驱动程序的核心用户空间组件和实用工具。提供核心的 NVIDIA 驱动库，包含用于查询和配置 NVIDIA 显卡的命令行工具（如 `nvidia-smi`）和 OpenGL 库、Vulkan 库，用于支持高性能图形渲染。

### 2.3.2 lib32-nvidia-utils

`lib32-nvidia-utils` 是 32 位 NVIDIA 驱动程序库的集合，用于在 64 位系统上运行 32 位应用程序（如 Steam 游戏和 Wine 软件）。需要启用 [multilib 仓库](#1.3.1%20启用%20multilib%20仓库)。

### 2.3.3 nvidia-settings

`nvidia-settings` 是一个独立的图形化配置工具，用于调整和配置 NVIDIA 显卡的各种设置。

---

# 3. 桌面环境配置

> [!note] 桌面选择
> 我个人比较喜欢 GNOME 桌面，因为配置起来比较简单，而且配置选项比较少，不像 KDE 那么让人眼花缭乱。

- [KDE_桌面配置](KDE_桌面配置)

## 3.1 安装 GNOME 桌面

下面指令实现最小化安装 GNOME 桌面：

```bash
sudo pacman -S gnome-shell gdm gnome-control-center gnome-settings-daemon gnome-keyring
```

这几个软件包是 GNOME 桌面所必须的：

- `gnome-shell`：GNOME 桌面的核心组件，提供桌面界面和窗口管理。
- `gdm`：GNOME 显示管理器，提供登录界面。
- `gnome-control-center`：GNOME 设置中心。
- `gnome-settings-daemon`：GNOME 设置守护进程，负责各类系统设置的生效。
- `gnome-keyring`：GNOME 密钥环，管理密码和证书。

启用 GDM 登录管理器：

```bash
sudo systemctl enable --now gdm
```

安装常用软件：

```bash
sudo pacman -S flatpak ghostty file-roller firefox
```

- `flatpak`：一款安装软件的工具，搭配 Flathub 仓库，足够解决日常软件使用需求；
- `ghostty`：一款终端模拟器；
- `file-roller`：和 GNOME 的文件管理器集成的压缩解压缩工具。

## 3.2 添加仓库

进入桌面环境后，推荐添加这两个软件包仓库：

- [Arch Linux CN 软件仓库镜像使用帮助](https://help.mirror.nju.edu.cn/archlinuxcn/)

- [Chaotic-AUR](https://aur.chaotic.cx/docs)：该仓库建议配置好网络代理后再安装。

## 3.3 网络代理配置

mihomo 裸核使用方法：
	- [直接安装在系统中（已通过 Gitee 加速）](https://gitee.com/loskyertt/Proxy)
	- [直接安装在系统中 - GitHub](https://github.com/loskyertt/Proxy)

> [!tip] 关于 GUI 代理客户端
> 不管是 Clash Verge Rev 还是 Clash Party，这类基于 mihomo 内核的 GUI 代理软件或多或少都有问题，而且还比较占资源，远没有 mihomo 裸核轻量和稳定。

## 3.4 Flatpak 使用方法

点击跳转：[Flatpak_教程](../系统配置/Flatpak_教程)。

## 3.5 推荐应用

**其他推荐应用如下**：
- `extension-manager`：扩展管理工具
- `gnome-tweaks`：GNOME 调整工具，也可以换成 `refine`
- `mission-center`：任务管理器
- `gnome-console`：GNOME 终端
- `gearlever`：AppImages 管理工具，可以把 `.appimage` 软件拖进 gearlever 中进行安装
- `evince`：PDF 阅读器
- `gnome-text-editor`：文本编辑器，也可以安装 `gedit`
- `gnome-font-viewer`：字体查看器
- `showtime`：视频播放器

> [!tip] 推荐使用 Flatpak 安装用户级软件
> 强烈建议用户级软件包使用 Flatpak 来安装。详见 [[Flatpak_教程]]。

## 3.6 安装字体

```bash
sudo pacman -S wqy-zenhei noto-fonts noto-fonts-emoji
```

还有 Maple-Mono 字体也不错，下载方式具体看 [maple-font](https://github.com/subframe7536/maple-font/tree/variable)，需要先配置好 **archlinuxcn 镜像仓库**。

```bash
sudo pacman -S ttf-maplemono ttf-maplemono-nf-unhinted ttf-maplemono-nf-cn-unhinted
```

还有其他字体，比如 MiSans 或者 HarmonyOS Sans，配合 DConf 字体缩放可以在高分屏上达到非常细腻的显示效果。

> [!info] 更多字体安装说明
> 详见 [[安装字体]]。

---

# 4. 软件包配置

## 4.1 中文输入法配置

> [!note] Fcitx5 vs IBus
> - **Fcitx5**：推荐非 GNOME 桌面使用。也可以在 GNOME 下使用，但在 Wayland 会话下可能存在兼容性问题。
> - **IBus**：推荐在 GNOME 桌面下使用，与 GNOME 集成更好，基本上不需要配置环境变量。

### 4.1.1 方式一：Fcitx5

> 参考：[Fcitx5 Arch Wiki](https://wiki.archlinuxcn.org/wiki/Fcitx5)

#### 4.1.1.1 安装

```bash
sudo pacman -S fcitx5-im fcitx5-rime noto-fonts-emoji
```

> [!note] 补充安装（可不选，因为要使用雾凇拼音）
> - `fcitx5-chinese-addons`：包含与中文相关的 addon，例如拼音、双拼和五笔。
> - `fcitx5-pinyin-zhwiki`：中文拼音词库。
> - `fcitx5-configtool`：一般会包含在 `fcitx5-im` 中，如果没有，再单独安装该配置工具。

#### 4.1.1.2 配置环境变量

**不同的桌面设置方法不同，在 `/etc/environment` 中添加下面内容**：

- GNOME 桌面环境：

```bash
GTK_IM_MODULE=fcitx
QT_IM_MODULE=fcitx
XMODIFIERS=@im=fcitx
SDL_IM_MODULE=fcitx
```

> [!tip] 吞字问题
> 如果在 GNOME 下遇到输入法吞字的问题，可以额外添加 `XDG_CURRENT_DESKTOP=GNOME`。但在较新版本的 Fcitx5 + GNOME 中，此问题通常已修复。

- KDE / Wayland 合成器：

```bash
XMODIFIERS=@im=fcitx
```

> [!info] Wayland 下的输入法
> 在 Wayland 会话下，GTK 和 Qt 应用通过 Wayland 协议的原生输入法支持与 Fcitx5 通信，因此不需要设置 `GTK_IM_MODULE` 和 `QT_IM_MODULE`。仅设置 `XMODIFIERS` 即可。但某些 XWayland 应用可能仍需要完整的环境变量。

#### 4.1.1.3 配置 rime-ice（雾凇）拼音

去 [rime-ice](https://github.com/iDvel/rime-ice) 仓库下载源码文件，把下载好的所有文件（除了 `.git`）全部放到 `~/.local/share/fcitx5/rime/` 目录下即可。

> [!tip] 使用 plum 安装（推荐）
> 也可以使用 [plum](https://github.com/rime/plum)（Rime 的包管理器）来安装雾凇拼音，这种方式更便于后续更新：
> ```bash
> rime_install iDvel/rime-ice:others/recipes/full
> ```

### 4.1.2 方式二：IBus

> 参考：[IBus Arch Wiki](https://wiki.archlinuxcn.org/wiki/IBus)

#### 4.1.2.1 安装

```bash
sudo pacman -S ibus ibus-rime noto-fonts-emoji
```

安装好重启后，IBus 引擎才会生效，你需要手动在图形界面添加它：

1. 打开 **GNOME 设置 (Settings)** → **键盘 (Keyboard)**。
2. 找到 **输入源 (Input Sources)**，点击 **+ (添加)**。
3. 点击 **汉语 (Chinese)**，在列表中找到 **Chinese (Rime)** 并添加。

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

> [!warning] 插件冲突
> 需要把原来的 syntax highlighting 插件删了，避免与 `fast-syntax-highlighting` 冲突。

执行 `zimfw install` 安装好并重启终端后即可使用。

3. 可以安装 [starship](https://starship.rs/zh-cn/) 进行美化：

```bash
sudo pacman -S starship
```

在 `.zshrc` 中填入：

```zsh
eval "$(starship init zsh)"
```

> [!warning] 主题冲突
> 如果同时使用 zimfw 的主题（如 `steeef`）和 starship，两者会冲突。使用 starship 时，请在 `~/.zimrc` 中移除或注释掉 zimfw 的主题模块（如 `zmodule steeef` 和 `zmodule prompt-pwd`）。

> [!info] 更多 zsh 配置
> 详见 [zsh_配置](../系统配置/zsh_配置)。

---

# 5. 用户配置

## 5.1 密码反馈设置

设置在终端输入密码时，显示星号：

```bash
sudo EDITOR=nano visudo
```

> [!info] 使用 nano 进行编辑。

在打开的文件中找到以 `Defaults` 开头的行。如果看到 `Defaults env_reset`，将其修改为：

```bash
Defaults env_reset, pwfeedback
```

如果没有看到该行，直接在文件末尾添加一行：

```bash
Defaults pwfeedback
```

> [!warning] 安全提醒
> `pwfeedback` 会通过星号数量暴露密码长度，在某些安全场景下可能构成信息泄露。请根据实际需求权衡是否启用。

---

# 6. 音视频固件和服务

让音频设备和屏幕分享正常工作。

1. 可选：安装音视频固件

```bash
sudo pacman -S --needed sof-firmware alsa-ucm-conf alsa-firmware
```

- `sof-firmware`：为现代音视频设备提供固件，通常装这个就可以了。
- `alsa-ucm-conf`：提供必要的配置文件。
- `alsa-firmware`：为不常见或者较旧的设备提供固件。

2. 安装音视频服务

```bash
sudo pacman -S --needed pipewire wireplumber pipewire-pulse pipewire-alsa pipewire-jack
```

- `pipewire`：由 Red Hat 主导开发的现代音视频服务。
- `wireplumber`：会智能管理 PipeWire。
- `pipewire-pulse`：为 PulseAudio 提供兼容。
- `pipewire-alsa`：为 ALSA 提供兼容。
- `pipewire-jack`：为 JACK 提供兼容。

> [!note] GNOME 用户
> GNOME 桌面（自 43 版本起）默认已安装并启用 PipeWire，通常无需手动安装。可以通过 `pactl info | grep "Server Name"` 检查是否正在使用 PipeWire-Pulse。

3. 启用服务

```bash
systemctl --user enable --now pipewire pipewire-pulse wireplumber
```

---

# 7. 性能模式切换

`power-profiles-daemon` 是各个桌面环境通用的性能模式切换服务，有三个档位：`performance`（性能）、`balanced`（平衡）、`powersave`（节电）。一般平衡档位就够用了，也不需要调节风扇什么的。

1. 安装

```bash
sudo pacman -S power-profiles-daemon
```

2. 启动服务

```bash
sudo systemctl enable --now power-profiles-daemon
```

> [!warning] 与其他电源管理工具冲突
> `power-profiles-daemon` 与 `tlp`、`auto-cpufreq` 和 `thermald` 等电源管理工具存在冲突，不能同时使用。如果已安装这些工具，需要先禁用它们：
> ```bash
> sudo systemctl disable --now tlp
> ```

> [!tip]
> `power-profiles-daemon` 易用且足够日常使用，不建议使用 tlp 或者 auto-cpufreq，功耗上不会有明显区别。

---

# 8. 蓝牙

1. 安装

```bash
sudo pacman -S --needed bluez bluez-utils
```

> [!note] 软件包说明
> - `bluez`：Linux 官方蓝牙协议栈。
> - `bluez-utils`：提供 `bluetoothctl` 等命令行工具，用于配对和连接蓝牙设备。

2. 启动服务

```bash
sudo systemctl enable --now bluetooth
```

> [!tip] 自动连接
> 如果希望蓝牙设备在开机后自动连接，可以安装 `bluetooth-autoconnect`（AUR）并启用：
> ```bash
> paru -S bluetooth-autoconnect
> sudo systemctl enable --now bluetooth-autoconnect
> ```

---

# 9. btrfs 维护

## 9.1 快照工具

btrfs 文件系统支持快照功能，推荐安装快照工具以防系统更新后出现问题：

- **Timeshift**：图形化快照工具，操作简单。

```bash
sudo pacman -S timeshift
```

- **Snapper**：命令行快照工具，功能更强大，可与 `snapper-gui` 配合使用。

```bash
sudo pacman -S snapper
```

> [!tip] 快照策略
> 建议在每次系统大更新前手动创建快照，或配置自动快照（如每次 `pacman` 事务前自动创建）。这样在系统出问题时可以快速回滚。

## 9.2 SSD TRIM

如果使用 SSD，建议启用周期性 TRIM 以维持写入性能：

```bash
sudo systemctl enable fstrim.timer
sudo systemctl start fstrim.timer
```

---

# 10. 防火墙配置

推荐使用 `firewalld`：

```bash
sudo pacman -S firewalld
sudo systemctl enable --now firewalld
```

> [!info] 更多防火墙配置
> 详见 [[Firewall_教程]]。

---

# 11. 问题汇总

## 11.1 安装软件时的证书问题

1. 首先，更新你的系统证书：

```bash
sudo update-ca-trust
```

2. 如果上面的方法不起作用，可以尝试跳过 PGP 签名验证来构建包，这可以帮助你确定问题是否与签名有关：

```bash
makepkg -si --skippgpcheck
```

> [!warning]
> `--skippgpcheck` 仅跳过源文件的 PGP 签名验证，与 SSL 证书无关。如果问题是 SSL 证书导致的，此参数无效。

3. 确保你的系统时间是正确的，因为时间不正确也可能导致 SSL 证书验证失败：

```bash
timedatectl status
```

如果时间不正确，可以同步：

```bash
sudo timedatectl set-ntp true
```

## 11.2 签名验证问题

**建议先参考这篇官方文章**：[Signature and keyring](https://discovery.endeavouros.com/signature-and-keyring/pacman-keyring-issues/2021/03/)

如果遇到类似以下错误（比如在执行 `sudo pacman -Syu` 时）：

```txt
错误：libinstpatch: 来自 "Brett Cornwall <brett@i--b.com>" 的签名是未知信任的
:: 文件 /var/cache/pacman/pkg/libinstpatch-1.1.6-3-x86_64.pkg.tar.zst 已损坏 (无效或已损坏的软件包 (PGP 签名)).
```

可以尝试以下方法：

1. 重新初始化密钥环：

```bash
sudo pacman-key --init
sudo pacman-key --populate archlinux endeavouros
```

2. 如果仍然不行，可以暂时禁用签名检查。编辑 `/etc/pacman.conf`，找到 `SigLevel` 那一行，修改为：

```txt
SigLevel = Never
```

> [!danger] 安全警告
> `SigLevel = Never` 会完全禁用包签名验证，使系统面临供应链攻击风险。**仅作为最后手段临时使用**，安装完成后务必立即恢复为默认值！

3. 安装完成后，恢复签名验证并重新初始化密钥环：

```bash
sudo pacman-key --init
sudo pacman-key --populate archlinux endeavouros
sudo pacman -Sy
```

## 11.3 文件未通过校验

比如在构建 AUR 包时有类似的如下报错信息：

```txt
正在验证 source 文件，使用sha512sums...
  nekoray ... 通过
  nekobox.sh ... 通过
  nekobox.desktop ... 通过
  1350.patch ... 失败
==> 错误：一个或多个文件没有通过有效性检查！
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

> [!warning] 校验失败的原因
> PKGBUILD 中的校验和与实际下载的源文件不匹配，通常是因为上游更新了文件但未更改版本号。使用 `updpkgsums` 更新校验和前，请确认下载的源文件是正确且未被篡改的。

## 11.4 双系统时间问题

在双系统（Windows + Linux）环境下，两个系统对硬件时钟的解读不同，可能导致时间错乱。

方式一：让 Linux 使用本地时间（与 Windows 一致）：

```bash
sudo timedatectl set-local-rtc 1
```

方式二（推荐）：修改 Windows 注册表，让 Windows 使用 UTC 时间：

```powershell
reg add "HKEY_LOCAL_MACHINE\SYSTEM\CurrentControlSet\Control\TimeZoneInformation" /v RealTimeIsUniversal /t REG_DWORD /d 1 /f
```

> [!tip]
> 推荐方式二，这样两个系统都使用 UTC 时间，避免冲突。详见 [[双系统时间配置]]。
