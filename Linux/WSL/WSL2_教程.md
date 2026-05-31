---
title: WSL2 使用教程
date: 2024-11-12 21:16:31
description: "WSL2 的安装、配置与使用教程，涵盖 systemd 启用、环境变量隔离、磁盘空间释放等"
tags:
  - WSL
  - WSL2
  - Linux
  - Windows
aliases:
  - WSL2教程
  - WSL2 使用教程
---


# 1. 安装和升级 WSL2

## 1.1 安装前的准备

- 首先需要在 **控制面板 → 程序 → 启用或关闭 Windows 功能** 处开启 **Hyper-V** 和 **适用于 Windows 的 Linux 子系统**，然后重启电脑即可。

- 默认有 2 个版本（WSL 和 WSL2）：
  - **WSL（WSL1）**：使用和主机相同的 IP 地址，兼容性更好，但功能有限。
  - **WSL2**：更加独立的 Linux 子系统，有单独的 IP 地址，通过 Windows 主机访问互联网，支持 `systemd`、完整的 Linux 内核功能。建议安装 WSL2。

> [!tip] 一键安装
> Windows 10（版本 2004+）和 Windows 11 支持通过命令行一键安装 WSL：
> ```powershell
> wsl --install
> ```
> 该命令会自动启用所需功能、安装 WSL2 内核更新包并下载默认的 Ubuntu 发行版。

如果手动安装 WSL2，还需要安装内核更新包：[wsl_update_x64](https://wslstorestorage.blob.core.windows.net/wslblob/wsl_update_x64.msi)，该软件来自于 [Windows 官网](https://learn.microsoft.com/zh-cn/windows/wsl/install-manual#step-4---download-the-linux-kernel-update-package)。

## 1.2 升级 WSL2

设置默认版本：

```powershell
wsl --set-default-version 2
```

更新版本：

```powershell
wsl --update
```

> [!tip] 离线安装
> 上面这种命令行方式容易因为网络原因卡住，因此建议离线下载安装：[microsoft/WSL](https://github.com/microsoft/WSL)，下载好后直接双击安装即可。

验证是否安装成功：

```powershell
wsl --version
```

输入这行指令有这样的输出，表示安装成功：

```txt
WSL 版本： 2.3.24.0
内核版本： 5.15.153.1-2
WSLg 版本： 1.0.65
MSRDC 版本： 1.2.5620
Direct3D 版本： 1.611.1-81528511
DXCore 版本： 10.0.26100.1-240331-1435.ge-release
Windows 版本： 10.0.19045.2673
```

---

# 2. WSL 的指令

## 2.1 常用指令

- 搜索可安装版本

```powershell
wsl --list --online
```

- 安装指定的版本

```powershell
wsl --install -d Ubuntu
```

- 查看安装的版本信息

```powershell
wsl -l -v
```

- 关闭 WSL

```powershell
wsl --shutdown
```

> [!note] WSL 命令在 Windows 中执行
> 以上 `wsl` 命令需要在 **Windows PowerShell** 或 **CMD** 中执行，不是在 WSL 内部执行。

## 2.2 备份与还原

下面的一系列操作以发行版 Ubuntu-22.04 为例。

- 停止指定的 WSL

```powershell
wsl --shutdown
```

- 导出（备份）WSL 子系统（需要先停止 WSL 子系统）

```powershell
wsl --export Ubuntu-22.04 D:\Ubuntu-22.04.tar
```

`D:\Ubuntu-22.04.tar` 是导出的路径。

- 卸载 WSL 子系统

```powershell
wsl --unregister Ubuntu-22.04
```

> [!warning] 不可逆操作
> `--unregister` 会永久删除该发行版的所有数据，操作前请确保已备份。

- 导入（还原）WSL 子系统

```powershell
wsl --import Ubuntu-22.04 D:\WSL D:\Ubuntu-22.04.tar
```

`D:\WSL` 是导入路径，`D:\Ubuntu-22.04.tar` 是前面备份子系统的路径。

还原后的子系统一般默认是 `root` 用户，需要修改为其它用户。只要修改还原后的 Linux 子系统中的 `/etc/wsl.conf` 配置文件即可，如下：

```bash
nano /etc/wsl.conf
```

加入下面的内容：

```ini
[user]
default=用户名
```

然后按前面的方法进行重启 WSL 子系统即可。

---

# 3. WSL 的配置

## 3.1 启用 systemd

编辑 `/etc/wsl.conf` 配置文件：

```bash
sudo nano /etc/wsl.conf
```

在文件中添加下面的内容：

```ini
[boot]
systemd=true
```

> [!tip] 验证 systemd 是否启用
> 重启 WSL 后，执行以下命令验证：
> ```bash
> systemctl list-unit-files --type=service
> ```
> 如果能正常列出服务列表，说明 systemd 已启用。

## 3.2 隔离环境变量

在 WSL 中，微软默认启用了一个叫作 `WSLENV` 的共享机制，会自动把 Windows 的 `PATH` 变量拼接进 WSL 的 `PATH` 中。比如执行指令：

```bash
echo $PATH | tr ':' '\n'
```

输出：

```bash
/usr/local/sbin
/usr/local/bin
/usr/sbin
/usr/bin
/sbin
/bin
/usr/games
/usr/local/games
/usr/lib/wsl/lib
/usr/bin/site_perl
/usr/bin/vendor_perl
/usr/bin/core_perl
/mnt/c/Program Files/OpenSSH/
/mnt/c/Windows/system32
/mnt/c/Windows
/mnt/c/Windows/System32/Wbem
/mnt/c/Windows/System32/WindowsPowerShell/v1.0/
/mnt/c/Program Files (x86)/NVIDIA Corporation/PhysX/Common
/mnt/c/Windows Kits/10/Windows Performance Toolkit/
......
/mnt/c/Packages/PCL_1.15.1/3rdParty/VTK/bin
/mnt/c/Packages/PCL_1.15.1/3rdParty/OpenNI2/Tools
/mnt/c/Apps/Windsurf/bin
```

这上面包含了一大堆 `/mnt/c/...` 和 `/mnt/d/...` 路径。

如果你想阻止 Windows 的环境变量混入 WSL，有以下两种主流的解决方法：

### 3.2.1 方法一：彻底切断

> 禁止所有 Windows PATH 混入（最干净）。

如果你完全不需要在 WSL 里调用任何 Windows 里的命令（比如 `cmd.exe`、`code`、`git.exe` 等），可以直接彻底禁用该功能。

1. 在 WSL 终端内，创建或编辑 `/etc/wsl.conf` 文件（需要管理员权限）：

```bash
sudo nano /etc/wsl.conf
```

2. 在文件中添加以下内容：

```ini
[interop]
appendWindowsPath = false
```

3. 保存并退出（在 `nano` 中按 `Ctrl + O` 保存，`Ctrl + X` 退出）。

4. **非常重要**：你必须在 Windows 的 PowerShell 中**重启 WSL** 才能生效：

```powershell
wsl --shutdown
```

5. 再次打开 WSL，输入 `echo $PATH | tr ':' '\n'`，你会发现 Windows 上的环境变量路径全部消失了。

> [!note] 单独引入需要的 Windows 路径
> 如果你想在进入终端时，使用特定的 Windows 路径（比如 VSCode），可以直接在 WSL 的 `~/.bashrc` 或 `~/.zshrc` 末尾加一段导入环境变量的代码。
>
> 例如，添加 VSCode 的路径：
>
> ```bash
> export PATH="$PATH:/mnt/c/Apps/Microsoft VS Code/bin"
> ```
>
> 关于在 WSL 中运行 GUI 程序，参见 [[WSL_容器_GUI]]。

### 3.2.2 方法二：精准控制

> 利用 `WSLENV` 过滤指定变量。

如果你希望保留绝大部分 Windows 路径，但**只想排除个别特定的环境变量**，可以通过在 Windows 中设置 `WSLENV` 变量来实现。

`WSLENV` 是一个由冒号分隔的列表，用来声明哪些变量可以在 Windows 和 WSL 之间共享。它支持 4 个后缀修饰符：

- `/p`：跨平台自动转换路径（Windows 路径转为 `/mnt/...`）
- `/l`：当该变量从 WSL 传回 Windows 时生效
- `/w`：当该变量从 Windows 传入 WSL 时生效
- `/u`：仅在从 Windows 启动 WSL 时生效

**操作步骤**：
1. 在 Windows 中打开**系统环境变量设置**。
2. 新建一个**用户变量**或**系统变量**：
    - 变量名：`WSLENV`
    - 变量值：`YOUR_VAR/w:ANOTHER_VAR/p`（表示只允许 `YOUR_VAR` 和 `ANOTHER_VAR` 传入 WSL）。
3. **关键点**：默认情况下，Windows 会隐式地把 `PATH` 包含在共享中。只要你在 Windows 的 `WSLENV` 里显式地指定了其他变量，且**不把 `PATH` 写进去**，Windows 的 `PATH` 就会被过滤掉。

> [!example] WSLENV 配置示例
> 假设你希望只将 `JAVA_HOME` 和 `GOPATH` 传入 WSL：
> 1. 设置 `WSLENV=JAVA_HOME/p:GOPATH/p`
> 2. 重启 WSL 后，Windows `PATH` 不再混入 WSL，只有这两个变量会被共享。

## 3.3 更换内核

在 Windows 下的资源管理器中的地址栏输入 `%UserProfile%`，然后按回车。在里面新建一个 `.wslconfig` 文件（如果没有的话）。在文件中填入下面的内容：

```ini
[wsl2]
kernel=C:\\WSL\\Kernel\\linux-wsl-stable-6.19.5
```

这里的 `C:\WSL\Kernel\linux-wsl-stable-6.19.5` 是内核路径，你可以自己手动编译内核。

- 手动编译内核参考官方仓库：[microsoft/WSL2-Linux-Kernel](https://github.com/microsoft/WSL2-Linux-Kernel)。
- 也可以用仓库里现成的：[Nevuly/WSL2-Linux-Kernel-Rolling](https://github.com/Nevuly/WSL2-Linux-Kernel-Rolling)。

> [!tip] .wslconfig 更多配置项
> `.wslconfig` 还支持配置内存限制、处理器数量等：
> ```ini
> [wsl2]
> memory=8GB
> processors=4
> swap=4GB
> ```
> 修改后需在 PowerShell 中执行 `wsl --shutdown` 重启生效。

---

# 4. 释放 WSL 占用的磁盘空间

> [!info] 动态扩容机制
> 有时候会发现，你在 WSL 中删掉了一些文件，但是磁盘剩余空间并没有增加。这是因为 WSL 的磁盘是 ==动态扩容== 的，但不会 ==自动释放==。

## 4.1 操作步骤

1. 关闭 WSL 中的 Linux 发行版：

```powershell
wsl --shutdown
```

2. 在 PowerShell 或者 CMD 中运行管理计算机的驱动器的 DiskPart 命令：

```powershell
diskpart
```

3. 选择虚拟磁盘文件（根据自己实际情况填写路径）：

```powershell
select vdisk file="D:\WSL\Ubuntu24.04\ext4.vhdx"
```

4. 只读，附加 vhdx 磁盘镜像文件：

```powershell
attach vdisk readonly
```

5. 压缩 vhdx 磁盘镜像文件：

```powershell
compact vdisk
```

6. 分离 vhdx 磁盘镜像文件：

```powershell
detach vdisk
```

7. 退出

```powershell
exit
```

或者按住 `Ctrl + C`。

## 4.2 可能出现的问题

```powershell
DISKPART> select vdisk file="D:\WSL\Arch\ext4.vhdx"

DiskPart 已成功选择虚拟磁盘文件。

DISKPART> compact vdisk

DiskPart 遇到错误: 由于虚拟磁盘系统限制，无法完成请求的操作。虚拟硬盘文件必须是未压缩和未加密的文件，并且不能是稀疏文件。
有关详细信息，请参阅系统事件日志。
```

这说明 `ext4.vhdx` 文件可能是 **压缩的（compressed）、加密的（encrypted）或稀疏的（sparse）**，而 `diskpart` 要求它是一个普通的未压缩、未加密的 VHDX 文件。因此，`compact vdisk` 失败了。

所以需要**移除压缩、加密和稀疏文件属性**，然后再尝试 `compact vdisk`。

## 4.3 解决方法

1. **检查 VHDX 文件属性**

执行以下命令来检查 `ext4.vhdx` 是否是稀疏文件、压缩文件或加密文件：

检查文件是否是稀疏文件：

```powershell
fsutil sparse queryflag "D:\WSL\Arch\ext4.vhdx"
```

如果输出：

```powershell
文件 D:\WSL\Arch\ext4.vhdx 是稀疏文件
```

那么该文件是稀疏文件，需要去掉稀疏标志。

检查文件是否被 NTFS 压缩：

```powershell
compact /q "D:\WSL\Arch\ext4.vhdx"
```

如果输出类似：

```powershell
1 个文件总计 10 GB
8 GB 已压缩
```

说明该文件已被压缩，需要取消压缩。

检查文件是否被加密：

```powershell
cipher /c "D:\WSL\Arch\ext4.vhdx"
```

如果输出类似：

```powershell
E  D:\WSL\Arch\ext4.vhdx
```

说明该文件被加密，需要解密。

2. **解除压缩、加密和稀疏属性**

执行以下命令，确保 VHDX 文件是普通文件：

取消稀疏文件标志：

```powershell
fsutil sparse setflag "D:\WSL\Arch\ext4.vhdx" 0
```

> [!note] 取消稀疏标志
> 如果 `fsutil sparse setflag ... 0` 不生效，可以使用以下方式：先创建一个同大小的非稀疏文件，再用 `fsutil sparse setflag` 设置。或者更简单的方法是复制文件并删除原文件：
> ```powershell
> copy "D:\WSL\Arch\ext4.vhdx" "D:\WSL\Arch\ext4_backup.vhdx"
> del "D:\WSL\Arch\ext4.vhdx"
> ren "D:\WSL\Arch\ext4_backup.vhdx" "ext4.vhdx"
> ```

取消 NTFS 压缩：

```powershell
compact /u /q "D:\WSL\Arch\ext4.vhdx"
```

取消加密：

```powershell
cipher /d "D:\WSL\Arch\ext4.vhdx"
```

3. **再次尝试挂载并压缩 VHDX**

执行：

```powershell
diskpart
select vdisk file="D:\WSL\Arch\ext4.vhdx"
attach vdisk
compact vdisk
detach vdisk
exit
```

---

# 5. WSL2 挂载 U 盘

U 盘插入电脑后，WSL 并不会识别出 U 盘，如果想要在 WSL2 中使用或查看 U 盘文件，需要挂载 USB 设备。

首先建一个用来挂载 USB 设备里文件的文件夹：

```bash
sudo mkdir /mnt/e
```

挂载（Windows 里显示 USB 设备为哪个盘，就将 E 换成对应的字母）：

```bash
sudo mount -t drvfs E: /mnt/e
```

现在就可以在 WSL 里访问 USB 设备里的内容了。

当想要卸载驱动器以便可以安全地将其删除时：

```bash
sudo umount /mnt/e
```

这样就恢复到原来 WSL 不识别 USB 设备的状态了。

> [!tip] usbipd-win（推荐）
> 如果需要直接访问 USB 设备（而不仅仅是文件系统），可以使用 [usbipd-win](https://github.com/dorssel/usbipd-win) 工具将 USB 设备直接附加到 WSL2 中。这对于 USB 串口设备、调试器等场景特别有用。
>
> 安装：
> ```powershell
> winget install usbipd
> ```
>
> 使用：
> ```powershell
> usbipd list
> usbipd bind --busid <BUSID>
> usbipd attach --wsl --busid <BUSID>
> ```

---

# 6. 相关笔记与参考链接

- [[WSL_容器_GUI]] — WSL2 和 Docker 容器中运行 GUI 程序
- [[Docker_配置]] — Docker 在 Linux 下的配置

**外部参考链接**：
- [Windows 安装 Linux 子系统 WSL](https://xujinzh.github.io/2023/08/04/windows-wsl-install/index.html)
- [WSL2 子系统的备份和还原](https://www.cnblogs.com/Chary/p/18011740)
- [WSL2 Arch+Docker 个人配置过程和踩坑记录，以及一些建议](https://blog.azurezeng.com/wsl2-arch-docker-configuration/)
- [WSL2 编译升级 Linux 内核](https://nxdong.com/wsl-update-kernel/)
- [在 WSL2 中删除文件后，磁盘空间未释放怎么办](https://blog.csdn.net/qq_23865133/article/details/141642123)
- [WSL2 挂载 USB 设备](https://blog.csdn.net/qq_59475883/article/details/123314000)
