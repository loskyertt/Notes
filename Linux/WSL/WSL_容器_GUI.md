---
title: WSL2 和 Docker 容器中运行 GUI 程序
date: 2024-11-27 12:14:33
excerpt: "本文主要介绍如何在 WSL2 或者 Docker 容器中运行 GUI 程序。"
categories: "WSL教程"
---

# 1.WSL2 执行 GUI 程序（X11）

需要在 Windows 上安装 MobaXterm，这里推荐安装中文版的（原版可以在官网下载）：[中文版 Mobaxterm](https://github.com/RipplePiam/MobaXterm-Chinese-Simplified?tab=readme-ov-file)。这个软件自带 x11-server。

然后只需要在 WSL2 （Linux 子系统发行版）中安装 x11-apps 即可。

对于 Ubuntu：

```bash
sudo apt install x11-apps -y
```

执行这条命令就能安装好配套的 x11-apps，比如 xclock 和 xeyes。

对于 Arch：

```bash
sudo pacman -S xorg
```

这条命令只是安装对于的 x11-server，然后执行下面的命令：

```bash
sudo pacman -S xclock xeyes
```

这样直接在终端、VSCode WSL2 中的终端或者 MobaXterm 中运行 xclock 或者 xeyes 就能显示 GUI 程序了。

```bash
xclock
```

如下图所示：
[![示例.png](https://s21.ax1x.com/2024/11/27/pA4NSkn.png)](https://imgse.com/i/pA4NSkn)

这样也能在 WSL2 中进行可是开发，比如用 Python 的 matplotlib 绘图等。

如果运行不成功的话，可以执行下面的命令试试：

```bash
DISPALY=<你的宿主机的IP>:0 xclock
```

比如：`DISPALY=172.27.158.40:0 xclock`。

---

# 2.Docker 容器中执行 GUI 程序（X11）

整体操作方式跟在 WSL2 中是一样的，这里以 Docker 的 Ubuntu 镜像为例子。

```bash
docker pull ubuntu
```

```bash
docker run -it --name=test-gui --env HTTP_PROXY=http:172.27.158.40:7890 --env HTTPS_PROXY=http:172.27.158.40:7890 ubuntu:latest
```

这里的 `--env HTTP_PROXY=http:172.27.158.40:7890 --env HTTPS_PROXY=http:172.27.158.40:7890` 是我配置的代理服务，`172.27.158.40` 是宿主机的 IP 地址。是为了方便执行容器中一系列 apt 的命令。当然，也可以配置镜像源。

如果要配置镜像源，记得安装 CA 证书：

```bash
apt-get install ca-certificates
```

进入容器后，先进行更新，执行：

```bash
apt-get update
```

然后下载：

```bash
apt-get install x11-apps
```

这里要运行 GUI 程序，只能在 MobaXterm 和 VSCode 中运行。

## 2.1 在 MobaXterm 中运行

```bash
DISPALY=<你的宿主机的IP>:0 xclock
```

如下图所示：
[![MobaXterm中运行.png](https://s21.ax1x.com/2024/11/27/pA4NEm4.png)](https://imgse.com/i/pA4NEm4)

如果觉得麻烦，可以把 `DISPALY=<你的宿主机的IP>` 导入到环境变量中。

```bash
export DISPALY=<你的宿主机的IP>
```

如下图所示：
[![MobaXterm中运行.png](https://s21.ax1x.com/2024/11/27/pA4NMp6.png)](https://imgse.com/i/pA4NMp6)

## 2.2 在 VSCode 中运行

需要通过 Dev Containers 扩展，进入到容器内部，然后直接执行命令即可：

```bash
xclock
```

如下图所示：
[![VSCode中运行.png](https://s21.ax1x.com/2024/11/27/pA4NtAA.png)](https://imgse.com/i/pA4NtAA)

注意：如果是在 Linux 系统下，在 VSCode 中运行可能会出现这样的错误：

```txt
root@710c5762df8e:~# xclock
Authorization required, but no authorization protocol specified
Error: Can't open display: :0
```

意味着当前用户或容器中的环境没有被授权访问 X11 显示服务器（`DISPLAY=:0`）。这是一个典型的 X11 权限问题。

只需要在宿主机上运行以下命令，允许容器的用户访问显示服务：

```bash
xhost +local:docker
```

这样再运行 xclock 就能成功了。

如果希望更安全，只允许特定用户访问，可以指定用户（比如这里指定 root 用户）：

```bash
xhost +SI:localuser:root
```

---

# 3.Docker 容器中执行 GUI 程序（Wayland）

运行容器时，挂载 Wayland 必需的 Socket 和环境变量：

```bash
docker run -it --name=test-wayland -e WAYLAND_DISPLAY=$WAYLAND_DISPLAY -e XDG_RUNTIME_DIR=$XDG_RUNTIME_DIR -v $XDG_RUNTIME_DIR/$WAYLAND_DISPLAY:$XDG_RUNTIME_DIR/$WAYLAND_DISPLAY --device=/dev/dri ubuntu:latest
```

解释：
- `-e WAYLAND_DISPLAY=$WAYLAND_DISPLAY`：传递 Wayland 显示环境变量（通常为 `wayland-0`）。
- `-e XDG_RUNTIME_DIR=$XDG_RUNTIME_DIR`：传递 Wayland 的运行时目录。
- `-v $XDG_RUNTIME_DIR/$WAYLAND_DISPLAY:$XDG_RUNTIME_DIR/$WAYLAND_DISPLAY`：挂载 Wayland Socket 文件。
- `--device=/dev/dri`：允许容器访问 GPU。

## 3.1 在容器中验证 Wayland 配置

进入容器后，验证环境是否正确：

1. 检查 `WAYLAND_DISPLAY`：

```bash
echo $WAYLAND_DISPLAY
```

应返回 `wayland-0` 或类似内容。

2. 检查 `XDG_RUNTIME_DIR`：

```bash
echo $XDG_RUNTIME_DIR
ls $XDG_RUNTIME_DIR
```

应包含 `wayland-0`。

## 3.2 运行 Weston-terminal（测试）

1. 安装 Weston 系列程序：

```bash
apt install -y weston
```

2. 运行 Weston-terminal 测试：

```bash
weston-terminal
```

如果配置正确，应该会打开一个终端窗口。如下图所示：

![Snipaste_2024-12-08_13-17-50.png](https://s2.loli.net/2024/12/08/2vfPZbtHQkBIsdV.png)

## 3.3 补充：切换 Wayland/X11 环境

如果你希望 `gedit` 和其他应用也通过 Wayland 运行，可以启动它们时强制启用 Wayland：

```bash
WAYLAND_DISPLAY=wayland-0 gedit
```

如果继续使用 X11，则无需修改。