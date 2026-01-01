# 1. 设置密码

安装方式请查看 NixOS 社区提供的 [NixOS WSL2 Document](https://nix-community.github.io/NixOS-WSL/)。

如果安装好后不知道初始密码，或者系统没提示你设置，最稳妥的方法是利用 WSL 的 **root 权限** 直接强制重置。

请在 Windows 的 **PowerShell** 中执行以下步骤：

1. **以 root 用户进入 NixOS：**

```shell
wsl -u root -d NixOS
```

_（注意：如果你的发行版名称不是 `NixOS`，请换成你实际的名字）_

2. **在 NixOS 终端内修改 `nixos` 用户的密码：**

```shell
passwd nixos
```

_此时系统会提示你输入新密码（输入时屏幕不会显示字符），输入两次并回车即可。_

3. **退出并回到 Windows：**

```
exit
```

---

# 2. 初始配置

在 WSL 下使用 NixOS 是一种非常硬核且高效的体验。NixOS 与 Ubuntu 等传统发行版最大的区别在于它的**“声明式配置”**：你不是通过运行命令来安装软件，而是把想要的软件写在一个配置文件里，然后让系统自动变成你描述的样子。

以下是在 WSL 中开始使用 NixOS 的保姆级指南：

## 2.1 初始化系统环境

NixOS 刚装好时，它的软件仓库（Channels）是空的。你需要更新一次，否则无法安装任何软件：

```shell
# 1. 更新软件源频道
sudo nix-channel --update

# 2. 第一次建议执行一次系统重构，确保 WSL 环境正常
sudo nixos-rebuild switch
```

NixOS 日常工作流

|**动作**|**命令/操作**|
|---|---|
|**安装软件**|修改 `/etc/nixos/configuration.nix`|
|**应用更改**|`sudo nixos-rebuild switch`|
|**临时用软件**|`nix-shell -p 软件名`|
|**清理旧版本**|`sudo nix-collect-garbage -d` (清理不用的旧版本，释放空间)|

## 2.2 配置 `configuration.nix`

在 NixOS 中，你绝大多数时间都在操作 `/etc/nixos/configuration.nix` 这个文件，这是 NixOS 的核心操作。

1. **找到并编辑它**

由于 NixOS 默认可能只装了极简编辑器（如 `nano`），你可以这样打开它：

```shell
sudo nano /etc/nixos/configuration.nix
```

系统自带的 `configuration.nix` 一般是这样的：

```shell
# Edit this configuration file to define what should be installed on
# your system. Help is available in the configuration.nix(5) man page, on
# https://search.nixos.org/options and in the NixOS manual (`nixos-help`).

# NixOS-WSL specific options are documented on the NixOS-WSL repository:
# https://github.com/nix-community/NixOS-WSL

{ config, lib, pkgs, ... }:

{
  imports = [
    # include NixOS-WSL modules
    <nixos-wsl/modules>
  ];

  wsl.enable = true;
  wsl.defaultUser = "nixos";

  # This value determines the NixOS release from which the default
  # settings for stateful data, like file locations and database versions
  # on your system were taken. It's perfectly fine and recommended to leave
  # this value at the release version of the first install of this system.
  # Before changing this value read the documentation for this option
  # (e.g. man configuration.nix or on https://nixos.org/nixos/options.html).
  system.stateVersion = "25.05"; # Did you read the comment?
}
```

2. **添加软件（示例）**

找到 `environment.systemPackages` 这一行，把你想要的工具加进去。比如你想装 `vim`、`git` 和 `wget`：

```shell
{ config, lib, pkgs, ... }:

{
  imports = [
    # include NixOS-WSL modules
    <nixos-wsl/modules>
  ];

  wsl.enable = true;
  wsl.defaultUser = "nixos";

  # ----------- 在这里添加要安装的应用 -----------
  environment.systemPackages = with pkgs; [
    vim
    git
    wget
    curl  # 随便加
  ];

  system.stateVersion = "25.05"; # Did you read the comment?
}
```

这些软件包的名称可以在 NixOS 的[官方仓库](https://search.nixos.org/packages)查看。

3. **应用修改**

每当你修改了配置文件，你必须运行下面这条命令，系统才会真正去下载并安装软件：

```bash
sudo nixos-rebuild switch
```

>> **提示：** 这就是 NixOS 的精髓。如果你把这个 `.nix` 文件备份到另一台电脑，运行同样命令，你会得到一个完全一模一样的系统。

## 2.3 临时试用软件

如果不想把软件写死在配置文件里，比如只想临时用一下 `neofetch` 看看系统信息：

```shell
nix-shell -p neofetch --run neofetch
```

这条命令会为用户创建一个临时的环境，装好 `neofetch` 供用户用，用完退出后，系统里不会留下任何垃圾文件。

---

# 3.启用 Flakes 功能

首先根据官方文档要求，在 WSL2 NixOS 中开启 `flake` 功能：[Setup Nix Flakes](https://nix-community.github.io/NixOS-WSL/how-to/nix-flakes.html) 

在 `configuration.nix` 中加入下面的内容：

```shell
{
  # 启用 flakes 实验功能
  nix.settings.experimental-features = [ "nix-command" "flakes" ];
}
```

创建 `/etc/nixos/flake.nix`，然后填入：

```shell
{
  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";
    nixos-wsl.url = "github:nix-community/NixOS-WSL/main";
  };

  outputs = { self, nixpkgs, nixos-wsl, ... }: {
    nixosConfigurations = {
      # 这里的 nixos 是主机的名称，即 hostname
      nixos = nixpkgs.lib.nixosSystem {
        system = "x86_64-linux";
        modules = [
          nixos-wsl.nixosModules.default
	      ./configuration.nix  # 导入旧的配置文件，使其也能生效
          {
            system.stateVersion = "25.05";
            wsl.enable = true;
          }
        ];
      };
    };
  };
}
```

再然后打开 `/etc/nixos/configuration.nix`，**删掉**类似 `imports = [ <nixos-wsl/modules> ];` 这种带尖括号的代码，因为 Flakes 不再需要它。

然后进入进入 `/etc/nixos` 目录（即 `flake.nix` 所在的目录），执行：

```shell
# 指定参数 --flake 时，必须在 flake.nix 所在目录下执行
sudo nixos-rebuild switch --flake

# 或者也可以直接执行
sudo nixos-rebuild switch
```

---

# 4.配置 VSCode-Sercer

在 `configuration.nix` 中添加下面两行配置即可，具体说明可以参见[官方文档](https://nix-community.github.io/NixOS-WSL/how-to/vscode.html#setup-vscode-remote)。

```shell
wsl.useWindowsDriver = true;

programs.nix-ld.enable = true;
```

---