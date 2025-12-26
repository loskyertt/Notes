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

## 2.4 模块化配置

在 Nix 语言中，一个 `.nix` 文件通常只能有一个“根”表达式（通常是一个大括号括起来的代码块，比如 `{ config, lib, pkgs, ... }:` 这个）。如果想把“系统基础设置”和“软件包列表”分开，有两种主要的方法：


### 2.4.1 方法 A：在一个文件内逻辑分离（最简单）

你不需要把大括号拆开，只需要在内部通过注释或合理的结构分块即可：

```shell
{ config, lib, pkgs, ... }:

{
  imports = [ <nixos-wsl/modules> ];

  # --- 系统基础配置 ---
  wsl.enable = true;
  wsl.defaultUser = "nixos";
  system.stateVersion = "25.05";

  # --- 软件包管理 ---
  environment.systemPackages = with pkgs; [
    vim
    git
    wget
    fastfetch
    zsh
    htop
  ];
}
```


### 2.4.2 方法 B：真正的物理文件分离（推荐）

如果你希望有一个干净的 `configuration.nix`，而把软件包放在另一个文件（比如 `packages.nix`）里，这才是 NixOS 的“正确打开方式”。

- 步骤 1：创建 packages.nix

在 `/etc/nixos/` 目录下新建一个文件：

```shell
# /etc/nixos/packages.nix
{ pkgs, ... }: # 声明它需要用到 pkgs

{
  environment.systemPackages = with pkgs; [
    vim
    git
    wget
    fastfetch
    zsh
    htop
  ];
}
```

- 步骤 2：在 `configuration.nix` 中引用它

修改你的主配置文件，使用 `imports` 关键字：

```shell
# /etc/nixos/configuration.nix
{ config, lib, pkgs, ... }:

{
  imports = [ 
    <nixos-wsl/modules>
    ./packages.nix  # <--- 在这里引入刚才创建的文件
  ];

  wsl.enable = true;
  wsl.defaultUser = "nixos";
  system.stateVersion = "25.05";
}
```

这种方式更推荐：

1. **解耦**：当你的配置变得非常复杂（比如有几百行）时，你可以按功能拆分，例如 `python-env.nix`、`docker-config.nix` 等。

2. **可重用性**：如果你以后有第二台 NixOS 机器，你只需要复制特定的 `.nix` 文件过去就能复用配置。

3. **清晰度**：`imports` 就像搭积木，你可以通过注释掉某一行来快速禁用一整套功能。

>> **注意点**：在 NixOS 的 module 系统中，参数通常建议写成 `{ config, lib, pkgs, ... }:`。这个 `...` 非常重要，它代表“忽略我不关心的其他参数”，这能避免很多兼容性报错。

## 2.5 常用配置

建议在配置中添加 `nixpkgs.config.allowUnfree = true;`，为了可以安装非自由软件。

### 2.5.1 zsh 配置

创建 `/etc/nixos/shell.nix`，在里面添加下面内容，并把该文件导入到 `configuration.nix` 中：

```shell
# -------------- shell 配置 --------------
{ config, lib, pkgs, ... }:

{
  programs.zsh = {
    enable = true;
    # 启用自动建议
    autosuggestions.enable = true;
    # 启用语法高亮
    syntaxHighlighting.enable = true;
    
    # 如果你想添加其他更复杂的插件
    # ohMyZsh = {
    #   enable = true;
    #   plugins = [ "git" "sudo" "docker" ];
    #   theme = "robbyrussell"; # 虽然可以用 starship，但 OMZ 的插件依然好用
    # };
  };
  
  # 字体配置
  fonts.packages = with pkgs; [
    # 在 NixOS 24.05 及之后版本，建议这样写：
    nerd-fonts.fira-code
    nerd-fonts.jetbrains-mono
  ];
  
  # 设置默认 Shell (针对你的用户)
  # 这里用户名是 nixos
  users.users.nixos.shell = pkgs.zsh;
}
```

### 2.5.2 docker 配置

创建 `/etc/nixos/docker.nix`，在里面添加下面内容，并把该文件导入到 `configuration.nix` 中：

```shell
# -------------- docker 配置 --------------
{ pkgs, ... }:

{
  # 启用 Docker 守护进程
  virtualisation.docker.enable = true;

  # 将你的用户加入 docker 组，这样运行 docker 命令就不需要 sudo 了
  users.users.nixos.extraGroups = [ "docker" ];
}
```

记得要要下载 docker，可以去官方仓库查看 docker 的版本，选择需要的版本下载。

### 2.5.3 配置 VSCode-Sercer

在 `configuration.nix` 中添加下面两行配置即可，具体说明可以参见[官方文档](https://nix-community.github.io/NixOS-WSL/how-to/vscode.html#setup-vscode-remote)。

```shell
wsl.useWindowsDriver = true;

programs.nix-ld.enable = true;
```

---

# 3.NixOS 的回滚机制

## 3.1 回滚机制说明

如果你改乱了配置文件导致系统出问题，或者想撤销刚才安装的软件，或者不小心把系统配坏了，你可以随时跳回之前的状态。

1. 查看所有历史版本：

```shell
nixos-rebuild list-generations
```

输出是这样的：

```shell
[nixos@nixos:~]$ nixos-rebuild list-generations
ls: cannot access '/nix/store/da87cksdzpd02n8rvl659h2qq4zgr561-nixos-system-nixos-25.05.813672.2b0d2b456e4e/lib/modules': No such file or directory
ls: cannot access '/nix/store/vwvwiz9n56kpivnqmb0x3kfhzpid1c74-nixos-system-nixos-25.05.813672.2b0d2b456e4e/lib/modules': No such file or directory
ls: cannot access '/nix/store/whafqgji97lpx778rqci6884m75a9mcr-nixos-system-nixos-25.05.20250724.3ff0e34/lib/modules': No such file or directory
Generation  Build-date           NixOS version              Kernel   Configuration Revision  Specialisation
3 current   2025-12-22 07:51:43  25.05.813672.2b0d2b456e4e  Unknown                          *
2           2025-12-22 07:47:01  25.05.813672.2b0d2b456e4e  Unknown                          *
1           2025-12-22 07:36:35  25.05.20250724.3ff0e34     Unknown                          *
```

>> **注**：WSL2 下，可以不用在意 `ls: cannot access ... No such file or directory` 这种报错，这在 WSL2 环境下是非常常见的。这是因为 `list-generations` 命令在执行时会去检查内核模块 (`/lib/modules`)。由于 WSL2 使用的是 Windows 提供的特殊 Linux 内核，而不是 NixOS 编译的标准内核，所以它找不到这些模块路径。**这不影响系统使用，可以忽略。**

2. 如果当前配置有问题，想切回上一个版本：

```shell
sudo nixos-rebuild switch --rollback
```

**NixOS 的回滚是“运行环境”的回滚，而不是“源代码”的回滚。**虽然**软件**和**系统配置**会随版本回滚，但**个人文件**（比如在 `Home` 目录下创建的文档、图片或 `.txt` 文件）还有配置文件（如 `/etc/nixos/*.nix`）是**不会**回滚的。NixOS 的版本管理主要负责“系统环境”和“软件版本”，而不负责用户的个人数据。

在 NixOS 的架构中，存在两个完全独立的世界：

1. **配置界（Input）**：即 `/etc/nixos/` 下的 `.nix` 文件。这是你手动编写的代码，它是**受你控制**的普通文本文件。
    
2. **系统界（Output）**：即 `/nix/store` 中的系统快照。这是 Nix 根据你的代码**编译生成**出来的成品。

当执行 `switch` 时，发生的过程是：

代码 (.nix) → 编译 → 生成新的系统快照 (比如 Generation 9) → 将系统指针指向 9。

当你执行 rollback 时，发生的过程是：

系统指针重新指向了旧的快照 (Generation 8)。

**但是，Nix 永远不会反过来修改你的源代码**。因此像修改过的 `/etc/nixos/*.nix` 中的配置文件，NixOS 是不会动的。

>> 想象一下这个场景：你写了一段很复杂的 Docker 配置，结果执行 `switch` 后系统崩了，你赶紧执行 `rollback` 回到了稳定版本。**幸好** Nix 没有把你的配置文件也“变回去”，你还能打开那个 `.nix` 文件，看着你刚才写的代码，研究到底是哪一行写错了，然后修复它。

既然 NixOS 不会自动管理代码的版本，那颗使用 Git 管理 `/etc/nixos`。

如果你用了 Git，你的工作流会变成这样：

1. 比如修改 `packages.nix`（添加 vim）。
    
2. `git commit -m "Add vim"`。
    
3. `sudo nixos-rebuild switch` (生成 Generation 9)。
    
4. 如果你想彻底回到版本 8：
    
    - 执行 `sudo nixos-rebuild --rollback`（系统变回去）。
        
    - 执行 `git checkout HEAD^`（代码也变回去）。

## 3.2 回滚操作

- 永久回滚到上一个 generation：

```shell
sudo nixos-rebuild switch --rollback
```

- 永久回到指定 generation：

```shell
# 1. 将系统当前的 profile 切换到第 10 号
sudo nix-env --profile /nix/var/nix/profiles/system --switch-generation 10

# 2. 激活这个版本（这一步很重要，否则你的环境不会立即改变）
sudo /nix/var/nix/profiles/system/bin/switch-to-configuration switch
```

- 临时激活指定 generation 10（重启后失效）：

```shell
sudo /nix/var/nix/profiles/system-10-link/bin/switch-to-configuration test
```

## 3.3 历史记录（Generations）

每当你运行 `sudo nixos-rebuild switch` 且配置发生了变化（也就是说，若构建出错被中断了，没有完成构建，是不会生成 `Generation` 的），NixOS 就会创建一个新的 Generation。它本质上是一个指向 `/nix/store` 中特定系统版本的软链接。

历史记录没有默认上限，也不会自动删除。 如果你不去管它，这些记录会一直增加。因为每一个 `Generation` 都会引用它所需的软件包，这些软件包在被引用期间不会被删除。

但是可以通过“垃圾回收”来清理这些历史记录。如下：

- 删除 7 天以前的所有历史版本：

```shell
sudo nix-env -p /nix/var/nix/profiles/system --delete-generations +7d
```

- 删除除了当前版本以外的所有版本（慎用，删了就不能回滚了）：

```shell
sudo nix-env -p /nix/var/nix/profiles/system --delete-generations old
```

- 删除多个指定编号的版本：

```shell
# 删除 40, 41, 42
sudo nix-env -p /nix/var/nix/profiles/system --delete-generations 40 41 42

# 删除除最近 3 个以外的所有 generations
sudo nix-env -p /nix/var/nix/profiles/system --delete-generations +3
```

- 最后，必须执行垃圾回收来释放物理空间：

```shell
sudo nix-store --gc

# 或者
sudo nix-collect-garbage
```

>> `nix-collect-garbage` 会真正释放磁盘空间，而 `--delete-generations` 只是移除 profile 链接。

因为 `Nix` 的机制是“声明式”的。因此不需要手动“添加”历史，因为配置文件本身就是备份。只要你把 `configuration.nix` 存好（比如备份或者传到 GitHub），你随时可以重建出任何历史状态。如果你想给某个版本起个别名，通常是通过 Git 管理配置文件来实现的。

## 3.4 迁移到另一台电脑

一本直接复制 `configuration.nix` 到另一台装有 NixOS 的主机执行 `switch` 理论上是可以做到迁移配置的，但实际操作有两个关键点：

1. 硬件配置 (hardware-configuration.nix)：

>> 通常在 `/etc/nixos/` 下还有另一个文件叫 `hardware-configuration.nix`。这个文件是安装时自动生成的（WSL2 下自然没有），包含了硬盘分区 UUID、内核驱动等。不要把旧电脑的这个文件覆盖到新电脑，新电脑应该用自己生成的。

2. 文件引用：

>> 如果当前主机的 `configuration.nix` 里引用了本地的其他文件（比如采用**模块化**配置时创建的其它 `*.nix` 文件，还有某些自定义脚本），这些也需要一起复制过去。

**更高级的做法：** 以后可以学习 **Nix Flakes**，它能锁定所有软件的版本版本，确保两台电脑装出来的东西不仅是“一样”，而是“二进制级别的一致”。

---

# 4. 搭建多语言开发环境

## 4.1 说明

首先根据官方文档要求，再 WSL2 NixOS 中开启 `flake` 功能：[Setup Nix Flakes](https://nix-community.github.io/NixOS-WSL/how-to/nix-flakes.html) 

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

再然后打开 `/etc/nixos/configuration.nix`，**删掉**类似 `imports = [ <nixos-wsl/modules> ];` 这种带尖括号的代码，因为 Flake 不再需要它。

然后进入进入 `/etc/nixos` 目录（即 `flake.nix` 所在的目录），执行：

```shell
# 指定参数 --flake 时，必须在 flake.nix 所在目录下执行
sudo nixos-rebuild switch --flake

# 或者也可以直接执行
sudo nixos-rebuild switch
```

构建成功后，后续每一个开发环境的搭建都是通过在对应项目中写 `flake.nix` 实现，然后在终端中执行 `nix develop` 就能构建项目环境，再在终端中执行 `code .` 就能在 VSCode 中打开该项目，并使用该终端中的环境。如下示例：

```shell
 tree . -L 1
.
├── cmake_build_debug.sh
├── CMakeLists.txt
├── example
├── flake.lock
├── flake.nix
├── format_count.sh
├── include
├── lib
├── LICENSE
├── README.md
├── src
└── test
```

这是我当前的 `test_cpp` 目录下的项目结构，重点是要有 `flake.nix`。

然后执行 `nix develop`：

![img](imgs/WSL2_NixOS/01flake配置示例.png)

在原来的系统环境中是没有 C++ 工具链的。

因为各个开发环境的 `flake.nix` 是能复用的，所以可以集中保存好，需要的时候复制过去即可。

## 4.2 C/C++ 开发环境

在 `flake.nix` 中写入：

```shell
{
  description = "C++ Clang 开发环境";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";
  };

  outputs = { self, nixpkgs }:
    let
      system = "x86_64-linux"; # 如果是 ARM 架构则改为 aarch64-linux
      pkgs = import nixpkgs { inherit system; };
      # 创建一个基于 clang 的 shell
      chanllengedShell = pkgs.mkShell.override { stdenv = pkgs.clangStdenv; };
    in
    {
      devShells.${system}.default = chanllengedShell {
        # 1. 编译和构建工具
        nativeBuildInputs = with pkgs; [
          cmake              # 构建工具
          ninja              # 更快的构建后端
          clang-tools        # 包含 clangd (LSP), clang-format, clang-tidy
          gdb                # 调试器
        ];

        # 2. 运行时依赖库 (第三方库放这里)
        buildInputs = with pkgs; [
          # 示例：添加常用库
          llvmPackages.openmp
          boost
          nlohmann_json
          fmt
        ];

        # 环境变量设置
        shellHook = ''
          echo "C++ 开发环境已加载！"
          echo "编译器: $(clang --version | head -n 1)"
          # 告诉 CMake 如何找到 Nix 安装的库
          export CMAKE_EXPORT_COMPILE_COMMANDS=1
        '';
      };
    };
}
```

然后在 `.vscode/settings,json` 中添加下面的内容：

```json
{
    "clangd.arguments": [
        "--query-driver=/nix/store/**-clang-**/bin/clang++",
        "--compile-commands-dir=build",
        "--background-index",
        "--log=verbose"
    ],
    "clangd.path": "clangd",
}
```

>> 这两个字段必须添加，否走 VSCode 编辑器中的 clangd 无法找到标准库！

## 4.3 Node.js 开发环境

在 `flake.nix` 中写入：

```shell
{
  description = "Node.js 开发环境";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";
  };

  outputs = { self, nixpkgs }:
    let
      # 建议使用此方式支持多架构 (x86_64-linux, aarch64-darwin 等)
      system = "x86_64-linux"; 
      pkgs = import nixpkgs { inherit system; };
    in
    {
      devShells.${system}.default = pkgs.mkShell {
        # 1. 开发工具 (编译时工具)
        nativeBuildInputs = with pkgs; [
          nodejs_20           # Node.js 运行时 (也可以选 nodejs_18, nodejs_22 等)
          yarn
          typescript-language-server # LSP 服务
          nodePackages.typescript    # tsc 编译器
          nodePackages.prettier      # 代码格式化
        ];

        # 2. 运行时依赖或系统库
        buildInputs = with pkgs; [
          # openssl
          # pkg-config
        ];

        # 环境变量设置
        shellHook = ''
          # 自动启用 corepack 以支持 pnpm/yarn
          # corepack enable --node-path ${pkgs.nodejs_20}/bin/node

          echo "Node.js 开发环境已加载！"
          echo "Node版本: $(node --version)"
          echo "NPM版本:  $(npm --version)"
          
          # 防止 node_modules 中的二进制文件找不到
          export PATH="$PWD/node_modules/.bin:$PATH"
        '';
      };
    };
}
```

>> **注意事项**：在 Nix 的逻辑中，我们**永远不要**使用 `npm install -g` 或 `pnpm add -g`。 如果你需要某个全局工具（比如 `pm2` 或 `vercel-cli`），你应该把它们直接写在 `flake.nix` 的 `nativeBuildInputs` 列表里。同样在 Python 环境中，也不要直接 `pip install`，而是先创建虚拟环境，把包安在虚拟环境中。