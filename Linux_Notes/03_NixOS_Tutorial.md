# 1.模块化配置

在 Nix 语言中，一个 `.nix` 文件通常只能有一个“根”表达式（通常是一个大括号括起来的代码块，比如 `{ config, lib, pkgs, ... }:` 这个）。如果想把“系统基础设置”和“软件包列表”分开，有两种主要的方法：

## 1.1 方法 A：在一个文件内逻辑分离

你不需要把大括号拆开，只需要在内部通过注释或合理的结构分块即可，如下所示：

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

## 1.2 方法 B：物理文件分离（推荐）

如果你希望有一个干净的 `configuration.nix`，而把软件包放在另一个文件（比如 `packages.nix`）里，这才是 NixOS 的“正确打开方式”。

- 步骤 1：创建 `packages.nix`

在 `/etc/nixos/` 目录下新建一个文件：

```shell
# /etc/nixos/packages.nix
{ config, lib, pkgs, ... }: 

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

---

# 2.Flake 功能

>> 要使用该功能，网络代理是必不可少的！

参考 [NixOS 与 Flakes](https://nixos-and-flakes.thiscute.world/zh/nixos-with-flakes/get-started-with-nixos)。

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
Generation  Build-date           NixOS version              Kernel   Configuration Revision  Specialisation
3 current   2025-12-22 07:51:43  25.05.813672.2b0d2b456e4e  Unknown                          *
2           2025-12-22 07:47:01  25.05.813672.2b0d2b456e4e  Unknown                          *
1           2025-12-22 07:36:35  25.05.20250724.3ff0e34     Unknown                          *
```

>> **注**：如果是在 WSL2 下，可以不用在意 `ls: cannot access ... No such file or directory` 这种报错，这在 WSL2 环境下是非常常见的。这是因为 `list-generations` 命令在执行时会去检查内核模块 (`/lib/modules`)。由于 WSL2 使用的是 Windows 提供的特殊 Linux 内核，而不是 NixOS 编译的标准内核，所以它找不到这些模块路径。**这不影响系统使用，可以忽略。**

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

既然 NixOS 不会自动管理代码的版本，那可使用 Git 管理 `/etc/nixos`。

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

也可以重启系统，在引导界面选择对应的 generations 进入。

- 临时激活指定的 generation，重启后失效（比如 `generation 10`）：

```shell
sudo /nix/var/nix/profiles/system-10-link/bin/switch-to-configuration test
```

## 3.3 删除 Generations

每当你运行 `sudo nixos-rebuild switch` 且配置发生了变化（也就是说，若构建出错被中断了，没有完成构建，是不会生成 Generation 的），NixOS 就会创建一个新的 Generation。它本质上是一个指向 `/nix/store` 中特定系统版本的软链接。

历史记录没有默认上限，也不会自动删除。如果你不去管它，这些记录会一直增加。因为每一个 Generation 都会引用它所需的软件包，这些软件包在被引用期间不会被删除。

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

在删除 `generations` 并回收垃圾后，记得重新构建引导，将已删除的 `generations` 的引导项删除：

```bash
sudo nixos-rebuild boot
```

这会重新扫描当前存在的 `system profile`，重写 `grub.cfg`，被删掉的 `generations` 就会从菜单消失。

## 3.4 迁移到另一台电脑

直接复制 `configuration.nix` 到另一台装有 NixOS 的主机执行 `switch` 理论上是可以做到迁移配置的，但实际操作有两个关键点：

1. 硬件配置 (hardware-configuration.nix)：

>> 通常在 `/etc/nixos/` 下还有另一个文件叫 `hardware-configuration.nix`。这个文件是安装时自动生成的（WSL2 下自然没有），包含了硬盘分区 UUID、内核驱动等。不要把旧电脑的这个文件覆盖到新电脑，新电脑应该用自己生成的。

2. 文件引用：

>> 如果当前主机的 `configuration.nix` 里引用了本地的其他文件（比如采用**模块化**配置时创建的其它 `*.nix` 文件，还有某些自定义脚本），这些也需要一起复制过去。

**更高级的做法：** 以后可以学习 **Nix Flakes**，它能锁定所有软件包的版本，确保两台电脑装出来的东西不仅是“一样”，而是“二进制级别的一致”。

---

# 4.系统升级

## 4.1 误区

>> 不要修改 `configuration.nix` 中的 `system.stateVersion`！修改这个字段不仅**不能**升级你的系统版本，反而可能在升级过程中导致你的数据（如数据库）崩溃。

在 NixOS 中，控制系统版本（即软件新旧）的是 **Channels (频道)** 或 **Flakes**。

**`system.stateVersion` 的作用** 是为了**向后兼容**。它告诉 NixOS：“这个系统最初是在哪个版本安装的”。

>> 举个例子：假设 PostgreSQL 数据库在版本 24.11 和 25.05 之间的存储格式发生了变化。如果你保持 `stateVersion = "24.11"`，即使你升级了系统，NixOS 也会通过特殊配置让数据库以旧格式运行，确保你的数据不会因为格式不兼容而无法启动。如果你把它改成 `"25.05"`，NixOS 会尝试用新标准启动，如果你的数据还没迁移，数据库可能直接就**挂了**。

**官方建议**：除非你阅读了版本发布说明（Release Notes）并完成了手动的数据迁移，否则 **永远不要手动修改这个值**。

## 4.2 正确的升级步骤（非 Flake 用户）

如果你使用的是标准的 Channel 模式，升级系统的正确流程如下：

1. 查看当前的 Channel：

```bash
sudo nix-channel --list
```

2. 切换到新版本的 Channel（例如从 24.11 升级到 25.05）：

```bash
sudo nix-channel --add https://nixos.org/channels/nixos-25.05 nixos
```

3. 更新下载 Channel 信息：

```bash
sudo nix-channel --update
```

4. 执行系统重构（升级）：

```bash
sudo nixos-rebuild switch
```

## 4.3 正确的升级步骤（Flake 用户）

1. 修改 `flake.nix` 中的 `inputs.nixpkgs.url`（将 `nixos-24.11` 改为 `nixos-25.05`）。

2. 执行：

```bash
nix flake update
sudo nixos-rebuild switch
```

## 4.4 补充

[NixOS Upgrade Tutorial](https://www.youtube.com/watch?v=7KypjXRBEnE)。这个视频详细解释了 NixOS 升级到新发行版的正确流程，特别强调了为什么不应该随意更改 stateVersion。

---

# 5.Channel 和 Store 的关系

## 5.1 说明

简单来说：“仓库（Store）”里存的是“货”，而“清单（Channel）”里存的是“货架索引”。

- **`.../store` (Substituter)**: 它是二进制缓存。你本地计算出一个 Hash，问它有没有。它只管“给货”，它不告诉你现在最新的版本是多少。
    
- **`.../nixos-25.05` (Channel)**: 它是一个**下载链接**。当你执行 `nix-channel --update` 时，它会从镜像站下载一个名为 `nixexprs.tar.xz` 的压缩包。这个包里包含了成千上万个 `.nix` 文件（即 **Nixpkgs 源码**）。

|**特性**|**nix-channel (数据源)**|**substituters (二进制缓存)**|
|---|---|---|
|**本质**|决定了软件的**版本**和**配置**。|决定了下载软件的**速度**和**来源**。|
|**内容**|Nix 表达式 (代码/描述文件)。|编译好的二进制成品。|
|**版本号**|**必须指定**，否则系统不知道你要用哪个发行版。|**不需要**，它根据文件的哈希值（Hash）寻址。|
|**镜像**|是为了让你快速下载那几百 MB 的**软件定义（代码）**。|是为了让你快速下载编译好的**软件成品（二进制）**。|

## 5.2 多频道共存的逻辑

当你执行 `sudo nix-channel --add ...nixos-24.11 nixos` 和 `sudo nix-channel --add ...nixos-unstable nixos-unstable` 时：

>> **注**：后面跟的 `nixos` 和 `nixos-unstable` 都是给 Channel 取的别名，可自定义。
>> 虽然你可以乱起名字，但 **`nixos`** 这个名字是特殊的。NixOS 的系统工具（比如 `nixos-rebuild`）在执行时，默认会去寻找名为 **`nixos`** 的频道来构建内核、驱动和系统组件。如果你把唯一的频道起名叫 `my-system` 而没有叫 `nixos` 的频道，当你执行 `sudo nixos-rebuild switch` 时，系统会报错，因为它找不到名为 `nixos` 的数据源。

当前系统里现在有两个“频道”，它们分别有自己的名字（别名）：

- 第一个频道叫 `nixos`（这是系统默认查找的名字）。
    
- 第二个频道叫 `nixos-unstable`。

当你执行 `sudo nixos-rebuild switch` 时，Nix 默认会去查找名为 `nixos` 的频道。

- 如果你之前用 `24.11` 覆盖了 `nixos` 这个名字，它就用 `24.11`。
    
- 如果你执行 `sudo nix-channel --add ...unstable nixos`（注意最后一个词），你把 `nixos` 这个名字指向了 `unstable`，那系统就变 `unstable` 了。

>> 即名字叫 `nixos` 的那个频道决定了你系统的基础版本。

那么另一个频道 `nixos-unstable` 还有用吗？当然有用，不过你需要手动配置。

你之前的 24.11 依然存在，但如果你想在 configuration.nix 里用到 nixos-unstable 里的软件，你得在代码里显式地引用它：

```shell
{ config, pkgs, ... }:
let
  # 显式导入名为 "nixos-unstable" 的频道
  unstablePkgs = import <nixos-unstable> { config.allowUnfree = true; };
in
{
  environment.systemPackages = [
    pkgs.vim              # 默认去名为 "nixos" 的频道找 (24.11)
    unstablePkgs.vscode   # 显式去名为 "nixos-unstable" 的频道找
  ];
}
```

---