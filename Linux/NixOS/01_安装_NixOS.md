# 1.下载镜像

下载 [NixOS ISO 镜像](https://mirror.nju.edu.cn/nixos-images/)。

>> 推荐下载 `minima` 镜像，通过 CLI 方式安装，这样安装的时候可以使用镜像源。

---

# 2.开始安装

进入 `root shell`：

```bash
sudo -i
```

## 2.1 网络连接

1. 有线以太网连接：主机连上网线即可。

2. USB 热点：通过 USB 连接手机，手机共享热点。

3. WIFI 连接：

- 查看可用的网络设备：

```bash
nmcli
```

- 列出可用的无线网络：

```bash
nmcli device wifi list
```

![img](imgs/01_Install_NixOS/01_连接WIFI.png)

主要是为了获得 `SSID`。

连接网络：

```bash
nmcli --ask device wifi connect
```

进入交互模式，根据要求依次输入要连接 WIFI 的 SSID 和密码即可。

也可以直接连接：

```bash
nmcli device wifi connect your_SSID password your_password
```

或者采用 TUI 的方式（如果支持的话）：

```bash
nmtui
```

4. 测试网络是否可用：

```bash
# 若此项不通优先先检查域名解析服务器
ping www.baidu.com -c 4

# 腾讯 DNSPod，若不通请检查网络连接
ping 119.29.29.29 -c 4
```

## 2.2 分区

1. 列出系统中的所有块设备（主要是找磁盘）：

```bash
lsblk
```

输出如下：

```bash
[root@nixos:~]$ lsblk
NAME  MAJ:MIN RM  SIZE RO TYPE MOUNTPOINTS
loop0   7:0    0  1.4G  1 loop /nix/.ro-store
sda     8:0    0   40G  0 disk
sr0    11:0    1  1.5G  0 rom  /iso
```

`sda` 就是磁盘设备，这些设备一般都是在 `/dev` 目录下。

2. 在 TUI 交互模式下进行分区：

```bash
cfdisk /dev/sda
```

在界面里，操作示例如下：

```bash
#   → 选 Label type: gpt (UEFI) 或 dos (Legacy)
#   → New → 输入 512M → Type → EFI System        (/boot/efi)
#   → New → 输入 4G   → Type → Linux swap       (swap)
#   → New → 剩余全部 → Type → Linux filesystem  (/)
#   → Write → 输入 yes → Quit
```

>> `swap` 分区可以不用设置，可以在安装好系统后看需要进行设置 `swapfile`。
>> 对于 EFI 分区，如果是把挂载点选为 `/mnt/boot`，建议大小设置为 `>= 1GB`；如果是把挂载点选为 `/mnt/boot/efi`，建议大小设置为 `400MB - 1GB` 之间。
>> 其他各个分区大小根据自己的实际需求调整。

分好区后如下所示：

```bash
[root@nixos:~]$ lsblk
NAME   MAJ:MIN RM  SIZE RO TYPE MOUNTPOINTS
loop0    7:0    0  1.4G  1 loop /nix/.ro-store
sda      8:0    0   40G  0 disk
├─sda1   8:1    0  400M  0 part
└─sda2   8:2    0 39.6G  0 part
sr0     11:0    1  1.5G  0 rom  /iso
```

## 2.3 逻辑格式化

1. 创建根文件系统，把 `/dev/sda2` 格式化为 `ext4` 文件系统。

```bash
mkfs.ext4 -L nixos /dev/sda2
```

>> `-L nixos` 表示设置卷标（Lable)为 `nixos`。也可以叫 `system`、`data`或其它，具体看个人喜好。只是以后挂载可以用 `mount LABEL=root /mnt`，当然，这行指令是和 `mount /dev/sda2 /mnt` （用设备的绝对路径）是等价的。

也可以选择格式化为 `btrfs` 文件系统：

```bash
mkfs.btrfs -L nixos /dev/sda2
```

> btrfs 文件系统提供快照、透明压缩和 RAID 功能。可以延长磁盘寿命，尤其是对于 SSD。

2. 创建 EFI 启动分区，把 `/dev/sda1` 格式化成 **FAT32**（EFI 系统分区必须用 FAT32）。

```
mkfs.fat -F 32 -n boot /dev/sda1
```

>> 如果有警告提示，如 `mkfs.fat: Warning: lowercase labels might not work properly on some systems`，这是正常现象，可以不用管，和标签的大小写有关。

- `-F 32` → 强制使用 **FAT32** 而不是 FAT12/16。
    
- `-n boot` → **设置 FAT 卷标为 `boot`**（DOS 时代的“卷序列名”）。  
    你想叫 `EFI`、`ESP`、`DOODLE` 都可以；同样只在 `lsblk -f`、`blkid`、`/dev/disk/by-label/...` 里出现，**不影响启动**，因为 UEFI 规范靠 **分区类型 GUID（ESP）** 而不是卷标来找启动文件。

## 2.4 挂载分区

1. 先把“将来系统”的根文件系统（`ext4`）挂到 **临时挂载点 /mnt**。

```bash
mount /dev/sda2 /mnt
```

>> 此时 `/mnt` 里就是根分区的顶层目录（bin、etc、home … 还有空目录 boot）。

2. 然后把 **ESP（FAT32）** 挂到 `/mnt/boot`。

```bash
mount --mkdir /dev/sda1 /mnt/boot
```

>> 访问 `/mnt/boot` 时，实际走的是 `/dev/sda1` 的 FAT32；访问 `/mnt` 下除 `boot` 外的路径时，仍走 `/dev/sda2` 的 `ext4`。**并不是** `/dev/sda1` 被“装”进 `/dev/sda2`，而是**目录项 boot 被覆盖了**（挂载掩盖）。

我是 **推荐** 把 EFI 分区的挂载点设为 `/mnt/boot/efi`，那么就要设置：

```shell
{
  boot.loader.efi.efiSysMountPoint = "/boot/efi";
}
```

`/boot` 里面有内核和初始化镜像，挂到 `/boot/efi` 可以把引导文件和这些文件分开存储。

>> 注意：**必须先挂根，再挂下级目录**，顺序反了会导致子挂载点被“隐藏”。

## 2.5 生成配置文件

执行指令：

```bash
nixos-generate-config --root /mnt
```

>> 根据当前挂载好的硬盘系统（在 `/mnt` 下），自动生成 NixOS 安装所需的硬件配置文件，放到 `/mnt/etc/nixos/` 里。

此时在 `/mnt/etc/nixos/` 下有两个配置文件：

| 文件                         | 作用                                                                                                               |
| ---------------------------- | ------------------------------------------------------------------------------------------------------------------ |
| `hardware-configuration.nix` | 自动探测并写出 **文件系统挂载表（UUID）、内核模块、驱动、交换、网络接口** 等硬件相关配置。                         |
| `configuration.nix`          | 给用户的“起点模板”，里面 `import` 了前者，并附带一些常用选项（时区、用户、SSH、GRUB 等），你可以直接改它定制系统。 |

## 2.6 修改配置文件

可以参考如下配置进行修改（`configuration.nix`）：

```bash
{ config, lib, pkgs, ... }:

{
  # 这里导入其它配置文件
  imports =
    [
      ./hardware-configuration.nix  # 硬件配置
    ];

  # 启用 GRUB + EFI 模式
  boot.loader = {
    # 确保主板能自动识别 NixOS 启动项
    efi.canTouchEfiVariables = true; 
    # 如果是挂载到 /mnt/boot/efi（默认是 "/boot"）
    efi.efiSysMountPoint = "/boot/efi";
  
    grub = {
      enable = true;
      efiSupport = true;  # 生成 grubx64.efi
      device = "nodev";
      # Win + Linux 双系统配置
      useOSProber = true;  # 自动识别其他 OS 的引导项（双系统配置）
    };
  };

  # 启用 flakes 功能
  nix.settings.experimental-features = [ "nix-command" "flakes" ];

  # Use latest kernel.
  boot.kernelPackages = pkgs.linuxPackages_latest;

  networking.hostName = "nixos";  # Define your hostname.

  # 使用 nmcli 或 nmtui 交互式配置网络连接
  networking.networkmanager.enable = true;

  # set clash-verge-rev and enble TUN mode
  programs.clash-verge = {
    enable = true;
    package = pkgs.clash-verge-rev;
    serviceMode = true;
    tunMode = true;
  };
  # 若 TUN 模式不管用，则启用 systemd DNS 解析守护进程
  # services.resolved.enable = true;

  # Set your time zone.
  time.timeZone = "Asia/Shanghai";
  # Win + Linux 双系统配置
  # time.hardwareClockInLocalTime = true;  # 将 RTC 时间标准设置为本地时间

  # 设置系统的中文环境
  i18n = {
    defaultLocale = "zh_CN.UTF-8";
    extraLocaleSettings = {
      LC_ADDRESS = "zh_CN.UTF-8";
      LC_IDENTIFICATION = "zh_CN.UTF-8";
      LC_MEASUREMENT = "zh_CN.UTF-8";
      LC_MONETARY = "zh_CN.UTF-8";
      LC_NAME = "zh_CN.UTF-8";
      LC_NUMERIC = "zh_CN.UTF-8";
      LC_PAPER = "zh_CN.UTF-8";
      LC_TELEPHONE = "zh_CN.UTF-8";
      LC_TIME = "zh_CN.UTF-8";
    };

    supportedLocales = [ "zh_CN.UTF-8/UTF-8" "en_US.UTF-8/UTF-8" ];
  };

  # Enable the X11 windowing system.
  # GNOME desktop
  services = {
    xserver.enable = true;
    displayManager.gdm.enable = true;
    desktopManager.gnome.enable = true;
    # 设为 false，不使用 GNOME 的应用程序
    # gnome.core-apps.enable = false;
  };

  # 音频服务设置
  services.pipewire = {
    enable = true;
    alsa.enable = true;
    alsa.support32Bit = true;
    pulse.enable = true;
  };

  # 启用触摸板支持（在大多数桌面管理器中默认启用）
  # services.libinput.enable = true;

  # Define a user account. Don't forget to set a password with ‘passwd’.
  # TODO 把 sky 改成你要设置的用户名
  users.users.sky = {
    isNormalUser = true;
    extraGroups = [ "wheel" ]; # Enable ‘sudo’ for the user.
    packages = with pkgs; [
      tree
    ];
  };

  # 启用 firefox 浏览器
  programs.firefox.enable = true;

  # allow user to install unfree software
  nixpkgs.config.allowUnfree = true;

  # List packages installed in system profile.
  # You can use https://search.nixos.org/ to find more packages (and options).
  environment.systemPackages = with pkgs; [
    vim
    wget
    git  # 重中之重！
  ];

  # 设置镜像源
  nix.settings.substituters = [
    "https://mirror.sjtu.edu.cn/nix-channels/store"
  ];

  # Enable the OpenSSH daemon.
  services.openssh.enable = true;

  # 先禁用防火墙，避免妨碍网络代理
  networking.firewall.enable = false;

  # 不要修改原来系统的 configuration.nix 的这部分！！
  system.stateVersion = "25.11";
}
```

这里不对每段配置进行说明，因为每段配置前均有注释。其它配置可以参考：

- [NixOS 中文：双系统安装](https://nixos-cn.org/tutorials/installation/DualBoot.html)

- [配置选项查询](https://search.nixos.org/options)

>> 第一次安装时，不建议修改太多的配置，简单修改下即可（先生成一个最简单的 generation，建议不要删除 `generation 1`），后续进了桌面环境再修改。`hardware-configuration.nix` 基本上不需要改动，除非你明确知道自己在做什么！

## 2.7 安装

添加 channel 镜像：

```bash
nix-channel --add https://mirrors.cernet.edu.cn/nix-channels/nixos-unstable nixos
nix-channel --update
```

>> 版本可以自己指定，比如把 `nixos-unstable` 改成 `nixos-25.05`。

执行下面这条命令进行安装（通过镜像源）：

```bash
nixos-install --option substituters https://mirror.sjtu.edu.cn/nix-channels/store
```

[其它镜像源](https://help.mirror.nju.edu.cn/nix-channels/)。

---

# 3.基础配置

建议采用 [模块化系统配置](https://nixos-and-flakes.thiscute.world/zh/nixos-with-flakes/modularize-the-configuration)。

## 3.1 初始化用户密码

先以 `root` 身份登录，然后修改用户的密码，根据前面 `configuration.nix` 中的配置，我设置的用户名是 `sky`：

```bash
passwd sky
```

根据提示设置密码即可。

允许使用非自由软件：

```bash
{
  # 允许使用非自由软件
  nixpkgs.config.allowUnfree = true;
}
```

## 3.2 系统语言设置

将系统语言设置为中文，在 `configuration.nix` 中添加：

```bash
{
  # 设置系统的中文环境
  i18n = {
    defaultLocale = "zh_CN.UTF-8";
    extraLocaleSettings = {
      LC_ADDRESS = "zh_CN.UTF-8";
      LC_IDENTIFICATION = "zh_CN.UTF-8";
      LC_MEASUREMENT = "zh_CN.UTF-8";
      LC_MONETARY = "zh_CN.UTF-8";
      LC_NAME = "zh_CN.UTF-8";
      LC_NUMERIC = "zh_CN.UTF-8";
      LC_PAPER = "zh_CN.UTF-8";
      LC_TELEPHONE = "zh_CN.UTF-8";
      LC_TIME = "zh_CN.UTF-8";
    };

    supportedLocales = [ "zh_CN.UTF-8/UTF-8" "en_US.UTF-8/UTF-8" ];
  };
}
```

添加中文字体，建议创建一个 `packages.nix`，这个文件里专门管理包：

```bash
# /etc/nixos/packages.nix
{ config, lib, pkgs, ... }: 

{
  # 系统软件包
  environment.systemPackages = [
    pkgs.vim
    pkgs.git
    pkgs.wget
    pkgs.bat
    pkgs.fastfetch
    pkgs.starship
    pkgs.btop
    pkgs.google-chrome
  ];
  
  # 字体设置
  fonts.packages = with pkgs; [
    # noto-fonts-cjk-sans        # 思源黑体，覆盖所有 CJK 字符
    # noto-fonts-cjk-serif       # 宋体
    noto-fonts-color-emoji  # emoji 样式
    # Maple Mono (Ligature TTF unhinted)
    maple-mono.truetype
    # Maple Mono NF (Ligature unhinted)
    maple-mono.NF-unhinted
    # Maple Mono NF CN (Ligature unhinted)
    maple-mono.NF-CN-unhinted
  ];
}
```

`gnome_pkgs.nix`：

```bash
# /etc/nixos/gnome_pkgs.nix
{ config, lib, pkgs, ... }: 

{
  # gnome 软件包
  environment.systemPackages = [
    pkgs.gnome-extension-manager
    pkgs.gnome-tweaks
  ];
}
```

# 4.输入法设置

## 4.1 Fcitx5 + rime-ice 配置

>> **注**：在 GNOME 下，为了搭配 Fcitx5 使用，一般会安装 “Input Method Panel” 这个扩展，这会使得 Fcitx5 的主题配置失效，强制变成适应 GNOME 的主题样式。

### 4.1.1 方式一（推荐）

创建 `fcitx5.nix`，然后导入进 `configuration.nix`，内容如下：

```bash
# /etc/nixos/fcitx5.nix
{ config, lib, pkgs, ... }: 

{
  # fcitx5 环境变量设置
  environment.variables = {
    GTK_IM_MODULE = "fcitx";
    QT_IM_MODULE = "fcitx";
    XMODIFIERS = "@im=fcitx";
  };
  
  i18n.inputMethod = {
    enable = true;
    type = "fcitx5";
    fcitx5.addons = with pkgs; [
      fcitx5-rime  # 数据目录：~/.local/share/fcitx5/rime/
      qt6Packages.fcitx5-chinese-addons  # table input method support
      fcitx5-gtk             # alternatively, kdePackages.fcitx5-qt
      catppuccin-fcitx5    # a color theme
    ];
  };
}
```

然后去 [rime-ice 仓库](https://github.com/iDvel/rime-ice) 下载源码文件，把下载好的所有文件（除了 `.git`）全部放到 `~/.local/share/fcitx5/rime/` 目录下即可。

### 4.1.2 方式二

创建 `fcitx5.nix`，然后导入进 `configuration.nix`，内容如下：

```bash
# /etc/nixos/fcitx5.nix
{ config, lib, pkgs, ... }: 

{
  # fcitx5 环境变量设置
  environment.variables = {
    GTK_IM_MODULE = "fcitx";
    QT_IM_MODULE = "fcitx";
    XMODIFIERS = "@im=fcitx";
  };
  
  i18n.inputMethod = {
    enable = true;
    type = "fcitx5";
    fcitx5.addons = with pkgs; [
      (fcitx5-rime.override { rimeDataPkgs = [ rime-ice ]; })  # 雾凇拼音
      qt6Packages.fcitx5-chinese-addons  # table input method support
      fcitx5-gtk             # alternatively, kdePackages.fcitx5-qt
      catppuccin-fcitx5    # a color theme
    ];
  };
}
```

接下来进行 rime-ice（雾凇拼音） 配置：

1. 修改 `~/.local/share/fcitx5/rime/default.custom.yaml` 这个文件，添加：

```yaml
patch:
  __include: rime_ice_suggestion:/

  schema_list:
    - schema: rime_ice
```

重载输入法配置即可。

2. 若 `~/.local/share/fcitx5/rime/build` 目录下有这些内容：

```bash
❯ ls .local/share/fcitx5/rime/build
default.yaml          melt_eng.table.bin          rime_ice.prism.bin
fcitx5.yaml           radical_pinyin.prism.bin    rime_ice.reverse.bin
melt_eng.prism.bin    radical_pinyin.reverse.bin  rime_ice.schema.yaml
melt_eng.reverse.bin  radical_pinyin.schema.yaml  rime_ice.table.bin
melt_eng.schema.yaml  radical_pinyin.table.bin
```

包含 `rim_ice.*` 文件，说明 Rime 以及部署成功了 rime-ice。

### 4.1.3 激活输入法

1. 在 Fcitx5 配置工具中进行修改：

![img](imgs/01_Install_NixOS/02_fcitx5配置01.png)

2. 检查是否成功激活：

![img](imgs/01_Install_NixOS/03_fcitx5配置02.png)

## 4.2 Ibus + rime-ice 配置

>> 该方案适合 GNOME 桌面环境。

### 4.2.1 方式一（推荐）

创建 `ibus.nix`，然后导入进 `configuration.nix`，内容如下：

```bash
# /etc/nixos/ibus.nix
{ config, lib, pkgs, ... }: 

{
  i18n.inputMethod = {
    enable = true;
    type = "ibus";
    ibus.engines = with pkgs.ibus-engines; [
      rime  # 数据目录：~/.config/ibus/rime
    ];
  };
}
```

重启后，IBus 引擎才会生效，你需要手动在图形界面添加它：

1. 打开 **GNOME 设置 (Settings)** -> **键盘 (Keyboard)**。
2. 找到 **输入源 (Input Sources)**，点击 **+ (添加)**。
3. 点击 **汉语 (Chinese)**，在列表中找到 **Chinese (Rime)** 并添加。

然后去 [rime-ice 仓库](https://github.com/iDvel/rime-ice) 下载源码文件，把下载好的所有文件（除了 `.git`）全部放到 `~/.config/ibus/rime` 目录下即可。

### 4.2.2 方式二

创建 `ibus.nix`，然后导入进 `configuration.nix`，内容如下：

```bash
# /etc/nixos/ibus.nix
{ config, lib, pkgs, ... }: 

{
  i18n.inputMethod = {
    enable = true;
    type = "ibus";
    ibus.engines = with pkgs.ibus-engines; [
      # 注入雾凇拼音数据
      (rime.override { rimeDataPkgs = [ pkgs.rime-ice ]; })
    ];
  };
}
```

重启后，IBus 引擎才会生效，你需要手动在图形界面添加它：

1. 打开 **GNOME 设置 (Settings)** -> **键盘 (Keyboard)**。
2. 找到 **输入源 (Input Sources)**，点击 **+ (添加)**。
3. 点击 **汉语 (Chinese)**，在列表中找到 **Chinese (Rime)** 并添加。

4. 修改 `~/.config/ibus/rime/default.custom.yaml` 这个文件，添加：

```yaml
patch:
  __include: rime_ice_suggestion:/

  schema_list:
    - schema: rime_ice
```

### 4.2.3 激活输入法

点击输入法托盘的部署，等待部署成功即可：

![img](imgs/01_Install_NixOS/04_ibus配置.png)

## 4.3 参考文章

1. [Fcitx5 配置](https://nixos.wiki/wiki/Fcitx5)。

2. [NixOS 安装 Rime 输入法](https://zhuanlan.zhihu.com/p/1963358188226183647)

3. [oh-my-rime 输入法快速初始化 rime ⚡，同时适配多个平台💻](https://www.mintimate.cc/zh/guide/)

---

# 5.常用配置

## 5.1 zsh 配置

创建 `/etc/nixos/shell.nix`，在里面添加下面内容，并把该文件导入到 `configuration.nix` 中：

```shell
# /etc/nixos/shell.nix
{ config, lib, pkgs, ... }:

{
  # 设置默认 Shell (用户名是 nixos)
  users.users.nixos.shell = pkgs.zsh;
  
  programs.zsh = {
    enable = true;
    # 启用自动建议
    autosuggestions.enable = true;
    # 启用语法高亮
    syntaxHighlighting.enable = true;
  };
}
```

## 5.2 docker 配置

创建 `/etc/nixos/docker.nix`，在里面添加下面内容，并把该文件导入到 `configuration.nix` 中：

```shell
# /etc/nixos/docker.nix
{ config, lib, pkgs, ... }: 

{
  virtualisation.docker = {
    # 启用 Docker 守护进程
    enable = true;
    # 具体版本名取决于你当前的 nixpkgs 仓库中包含哪些定义,你可以改为你需要的版本
    package = pkgs.docker_28;
  };
  
  # 将你的用户（我这里是 sky）加入 docker 组
  users.users.sky.extraGroups = [ "docker" ];
}
```

记得要要下载 docker，可以去官方仓库查看 docker 的版本，选择需要的版本下载。

如果要让 Nvidia GPU 直通进 docker 容器，还需要添加设置：

```shell
{
  hardware.nvidia-container-toolkit.enable = true;
  # Regular Docker
  virtualisation.docker.daemon.settings.features.cdi = true;
  # If using Rootless Docker
  # virtualisation.docker.rootless.daemon.settings.features.cdi = true;
}
```

然后，在使用 docker-cli 时，您应该能够执行以下操作：

```bash
docker run --rm -it --device=nvidia.com/gpu=all ubuntu:latest nvidia-smi
```

---

# 6.Nvidia 配置

## 6.1 安装 Nvidia 驱动程序

创建 `/etc/nixos/gpu_modules/default.nix` ：

```bash
# /etc/nixos/gpu_modules/default.nix
let
  mode = "offload";   # 想切 sync 就改成 "sync"
in
{
  imports = [
    ./nvidia.nix
    ./${mode}_mode.nix
  ];
}

# 单显卡只用保留：
# {
#   imports = [
#     ./nvidia.nix
#   ];
#  }
```

在 `/etc/nixos/configuration.nix` 中这样导入：

```shell
{
  imports = [
    ./hardware-configuration.nix
    ./gpu_modules
    ...
  ];
}
```

创建 `/etc/nixos/gpu_modules/nvidia.nix`，在里面添加下面内容：

```shell
# /etc/nixos/gpu_modules/nvidia.nix
{ config, lib, pkgs, ... }:

{
  # Enable OpenGL
  hardware.graphics = {
    enable = true;
  };

  # Load nvidia driver for Xorg and Wayland（若是单显卡，就取消注释）
  # services.xserver.videoDrivers = [ "nvidia" ];

  hardware.nvidia = {

    # Modesetting is required.
    modesetting.enable = true;

    # 默认（false）情况下，系统休眠时只保存必要的 GPU 状态。
    # 开启后，它会将全部显存内容保存到磁盘。
    # 建议: 如果你休眠唤醒后发现桌面花屏、软件崩溃，请将其设为 true。
    powerManagement.enable = false;

    # true: 细粒度的电源管理，不使用时关闭 GPU。
    # 仅支持 Turing 架构 (RTX 20) 及之后的笔记本电脑。
    powerManagement.finegrained = false;

    # false (默认): 使用 Nvidia 的闭源内核模块。这是最稳定的选择，支持所有显卡。
    # true: 使用 Nvidia 官方近两年发布的开源内核模块，与 Linux 内核集成更好。仅支持 Turing (RTX 20) 架构及之后的新卡。
    open = false;

    # Enable the Nvidia settings menu
    nvidiaSettings = true;

    # 根据需要，你可能需要为你的特定显卡选择合适的驱动程序版本。
    package = config.boot.kernelPackages.nvidiaPackages.stable;
  };
}
```

>> 这是单显卡的配置，下面是双显卡（核显 + 独显）的配置方法。

## 6.2 双显卡配置

### 6.2.1 获取显卡总线 ID

执行 `nix-shell -p lshw` 或者 `nix-shell -p toybox` 获取相关软件包。可以通过下面指令来获取：

```bash
sudo lshw -c display

# 或者
lspci | grep -i vga
```

>> **注意**：获取的总线 ID（`bus info`）是十六进制的，需转成十进制：

```bash
# lshw 获取的输出
...
bus info: pci@0000:0e:00.0
...
configuration: depth=32 driver=nvidia ...
...
```

这表示 Nvidia 总线 ID 为 `0e:00.0`，转成十进制为 `14:0:0`。

```bash
{
  hardware.nvidia.prime = {
    # Make sure to use the correct Bus ID values for your system!
    intelBusId = "PCI:0:2:0";
    nvidiaBusId = "PCI:14:0:0";
    # amdgpuBusId = "PCI:54:0:0"; For AMD GPU
  };
}
```

>> 有 Offload Mode 和 Sync Mode，**这两种模式互斥**，不能同时启用。

### 6.2.2 Offload 模式（推荐）

- **节能优先**：默认使用集成显卡（Intel/AMD iGPU）处理所有显示任务（如桌面、浏览器等），NVIDIA dGPU 保持休眠。
- **按需使用独显**：仅当你显式加载某个程序到 NVIDIA GPU 时（如游戏、渲染软件），才激活 dGPU。
- 适用于大多数日常使用场景，兼顾续航与性能。

```bash
# /etc/nixos/gpu_modules/offload_mode.nix
{ config, lib, pkgs, ... }:

{
  # 如果是 amd 核显，就把 modesetting 改为 amdgpu
  # 注意：顺序不能换，必须是核显在前，独显在后
  services.xserver.videoDrivers = [ "modesetting" "nvidia" ];

  # Offload 模式
  hardware.nvidia.prime = {
    offload = {
      enable = true;
      enableOffloadCmd = true;  # 自动生成 nvidia-offload 命令
    };
    # Make sure to use the correct Bus ID values for your system!
    intelBusId = "PCI:0:2:0";
    nvidiaBusId = "PCI:14:0:0";
    # amdgpuBusId = "PCI:54:0:0"; For AMD GPU
  };
}
```

日常用法：

- 通过终端启动：

```bash
nvidia-offload glxgears

nvidia-offload steam

nvidia-offload blender
```

- Steam 游戏：在 Steam → Settings → Set Launch Options 里给“全局”或逐条游戏写 `nvidia-offload %command%`。

- 查看是否成功 offload：`nvidia-smi` 里只能看到你要跑的进程，其余时间为空。

### 6.2.3 Sync 模式

- **显示输出由 dGPU 渲染并同步到 iGPU 屏幕**，减少画面撕裂（tearing）。
- **性能优先**：即使没有运行图形密集型程序，NVIDIA GPU 也可能保持活跃（功耗更高）。
- 适合外接显示器、使用 clamshell 模式（合盖用外显）或对画面流畅性要求高的场景。

```bash
# /etc/nixos/gpu_modules/sync_mode.nix
{ config, lib, pkgs, ... }:

{
  # 如果是 amd 核显，就把 modesetting 改为 amdgpu
  # 注意：顺序不能换，必须是核显在前，独显在后
  services.xserver.videoDrivers = [ "modesetting" "nvidia" ];

  # Sync 模式
  hardware.nvidia.prime = {
    sync.enable = true;

    # Make sure to use the correct Bus ID values for your system!
    nvidiaBusId = "PCI:14:0:0";
    intelBusId = "PCI:0:2:0";
    # amdgpuBusId = "PCI:54:0:0"; For AMD GPU
  };
}
```

日常用法：

- 什么都不用管，所有程序默认走 Nvidia。

- `nvidia-smi` 里随时都能看到 Xorg 和正在跑的程序。

- 如果想临时让某个程序回核显，目前做不到（除非改配置切回 offload）。

## 6.3 参考文章

- [Nvidia - NixOS Wiki](https://nixos.wiki/wiki/Nvidia)

- [NVIDIA - NixOS 官方 Wiki](https://wiki.nixos.org/wiki/NVIDIA)

---

# 7.代理配置

>> 建议使用 TUN 模式，下面都是 TUN 模式的配置。各种配置方式任选一种即可！

## 7.1 mihomo 配置（推荐）

创建 `/etc/nixos/mihomo.nix`，在里面添加下面内容，并把该文件导入到 `configuration.nix` 中：

```bash
# /etc/nixos/proxy/mihomo.nix
{ config, lib, pkgs, ... }:

{
  services.mihomo = {
    enable = true;
    package = pkgs.mihomo;
    tunMode = true;
    webui = pkgs.metacubexd;

    # 指定你的配置文件路径
    configFile = ./config.yaml;
  };

  # 内核转发。默认是开启的，可以用 sysctl net.ipv4.ip_forward 查看
  # 若未开启，则手动开启
  # boot.kernel.sysctl = {
  #   "net.ipv4.ip_forward" = 1;
  #   # "net.ipv6.conf.all.forwarding" = 1;
  # };
}
```

### 7.1.1 故障排除

可以用 `ip a` 检查是否有 TUN 网卡和用 `sudo journalctl -fu mihomo` 查看日志。

若日志中有 `bind: permission denied`，则启用下面选项：

```bash
{
  # 允许服务绑定 53 端口
  systemd.services.mihomo.serviceConfig.AmbientCapabilities = [ "CAP_NET_BIND_SERVICE" "CAP_NET_ADMIN" ];
  systemd.services.mihomo.serviceConfig.CapabilityBoundingSet = [ "CAP_NET_BIND_SERVICE" "CAP_NET_ADMIN" ];
}
```

仍然无法使用 TUN，可以使用 `sudo dmesg` 检查内核日志。如果看到大量关于特定网络设备的 refuse 消息，那么可能是防火墙的原因，可以先关闭试试:

```bash
{
  networking.firewall.enable = false;
}
```

如果关掉防火墙后问题得到解决，可以依次尝试：

- 将 tun 设备添加到 `trustedInterfaces`
- 禁用 `checkReversePath`

```bash
{
  # 若确实是防火墙的原因，则用下面这段配置
  networking.firewall = {
    enable = true;
    # 填入 tun 接口名字（通过 ip a 确认）
    trustedInterfaces = [ "Meta" ];
    checkReversePath = "loose";
  };
}
```

>> 当然，最关键的还得是你 `config.yaml` 中的配置，是否其中有问题？

参考文章：

- [NixOS mihomo Wiki](https://wiki.nixos.org/wiki/Mihomo/zh)。

## 7.2 Clash-Verge-Rev 配置

创建 `/etc/nixos/clash-verge.nix`，在里面添加下面内容，并把该文件导入到 `configuration.nix` 中：

```bash
# /etc/nixos/proxy.nix
{ config, lib, pkgs, ... }:

{
  programs.clash-verge = {
    enable = true;
    package = pkgs.clash-verge-rev;  # 下载 clah-verge-rev
    serviceMode = true;
    tunMode = true;
  };

  # 启用 systemd DNS 解析器守护程序 systemd-resolved
  # services.resolved.enable = true;
}
```

如果启用了 `systemd-resolved`（或其他任何 DNS 解析器），但仍然无法使用 TUN，则可能是启用了默认的 NixOS 防火墙。解决方法见 **6.1.1 故障排除**。

---

# 8.其它配置

后续的配置就跟据个人具体情况而定了。可以参考这个仓库终端配置 [my_nixos](https://github.com/loskyertt/my_nixos)。

---