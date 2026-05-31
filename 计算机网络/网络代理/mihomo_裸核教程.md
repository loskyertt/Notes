---
title: mihomo(Clash Meta) 纯内核使用教程
date: 2024-07-25 06:49:04
excerpt: "这篇文章主要是记录自己使用 Clash Meta 的一些心得，因为强迫症的原因，所以是通过 docker 来部署整个代理服务的。"
categories: "计算机网络"
tags:
  - mihomo
  - Clash-Meta
  - 代理
  - Docker
  - TUN
aliases:
  - Clash Meta 教程
  - mihomo 纯内核
---

# 1.前言

在 [GitHub](https://github.com/) 上其实有不少的客户端和服务端的软件，如果不喜欢折腾的话，这篇文章可能不怎么适合你，建议直接使用第三方客户端的代理软件。这篇文章主要是记录自己使用 mihomo 的一些心得，因为强迫症的原因，所以是通过 Docker 来部署整个代理服务的。当然，用 Docker 部署也是有好处的，配置好后可以连同镜像一起打包备份或者 `push` 到 [Docker Hub](https://hub.docker.com/) 上，后续电脑环境改变了仍然能使用。

---

# 2.资源下载

先把存放下载文件的项目文件夹创建好：

```bash
mkdir ~/mihomo && cd mihomo
```

后续下载好的文件都放在 `mihomo` 目录下。

1. **内核**：[稳定版下载地址](https://github.com/MetaCubeX/mihomo/releases)

2. **GeoIP & GeoSite 库**：
	- [GeoIP 库下载](https://gcore.jsdelivr.net/gh/MetaCubeX/meta-rules-dat@release/geoip.dat)
	- [GeoSite 库下载](https://gcore.jsdelivr.net/gh/MetaCubeX/meta-rules-dat@release/geosite.dat)

3. **WebUI**：
	- [metacubexd](https://github.com/MetaCubeX/metacubexd)
	- [zashboard](https://github.com/Zephyruso/zashboard)

> [!note] 参考仓库
> 可以参考我的这个仓库：
> - [直接安装在系统中（已通过 Gitee 加速）](https://gitee.com/loskyertt/mihomo)
> - [直接安装在系统中 - GitHub](https://github.com/loskyertt/mihomo)
>
> 仓库里有 `config.yaml` 配置文件，在里面填入 `proxy-providers` 处填入你的机场代理链接即可。

---

# 3.配置

## 3.1 方式一：mihomo 内核（系统服务）

### 3.1.1 Linux

> 这里以 EndeavourOS（Arch 系的发行版）为例。

通过 pacman 直接安装 mihomo，以 systemd 服务方式运行。

**安装：**

```bash
sudo pacman -S mihomo
```

> [!important] 关于软件源
> mihomo 仅在 [archlinuxcn](https://help.mirror.nju.edu.cn/archlinuxcn/) 源、[chaotic-aur](https://aur.chaotic.cx/) 源或 AUR 中提供。建议先添加 archlinuxcn 源，然后从 archlinuxcn 源进行下载安装。

**启动服务：**

```bash
sudo systemctl enable --now mihomo.service
```

**配置：**

复制配置文件和 WebUI：

```bash
# 复制配置文件（记得填入订阅链接）
sudo cp config.yaml /etc/mihomo/config.yaml

# 复制 WebUI（任选一个 dashboard 目录）
sudo cp -r zashboard /var/lib/mihomo/ui
```

**重启服务：**

```bash
sudo systemctl restart mihomo.service
```

> [!info] 文件位置
> - 配置文件：`/etc/mihomo/config.yaml` 或 `~/.config/mihomo/config.yaml`
> - WebUI：放在 `/var/lib/mihomo/` 目录下
>
> 如果遇到问题，参考 [mihomo 纯内核代理使用方法](https://gitee.com/loskyertt/mihomo)。

### 3.1.2 Windows

#### 下载 mihomo 核心文件并解压

各个版本的区别：

1. **核心区别**：`v1`, `v2`, `v3`, `compatible`（CPU 微架构级别）

	Go 1.18+ 针对 x86-64 (amd64) 架构引入了 4 个微架构级别（v1-v4）。级别越高，利用的现代 CPU 新指令集（如 AVX, AVX2, FMA 等）就越多，理论上性能和内存效率越好，但对老旧 CPU 的兼容性越差。

	| 版本 | 要求指令集 | 适用 CPU |
	|------|-----------|---------|
	| `compatible` | 关闭几乎所有特定指令集优化 | 极其古老或非主流的 x86 处理器 |
	| `amd64` / `v1` | SSE/SSE2 | 过去 20 年内的所有 64 位 Intel/AMD CPU |
	| `v2` | SSE4.1、SSE4.2、POPCOUNT | Intel Nehalem（第一代 Core i3/i5/i7）及之后 |
	| `v3` | AVX、AVX2、BMI1、BMI2、FMA3 | Intel Haswell（第四代酷睿，2013 年）及之后，AMD Ryzen 全系列 |

	> [!tip] 简单来说
	> 如果你的电脑是近 10 年内买的，直接无脑选 **`v3`** 性能最好；如果不确定，选 **`v1`** 或不带 v 的标准版最稳妥。

2. **编译器区别**：`-go120`, `-go125`（Go 运行时版本）

	形如 `-go125` 的后缀，代表该可执行文件是用特定版本的 Go 语言编译器编译出来的（例如 `go1.25`）。

	- **带具体 Go 版本（如 `go125`）**：使用特定版本的 Go 编译，通常是为了照顾某些旧系统，或者特定的加密库、内核网络特性在旧版 Go 下更稳定。
	- **不带 Go 版本（如 `mihomo-windows-amd64-v3-v1.19.25.zip`）**：通常使用当前开发团队推荐的最新的、最稳定的主流 Go 版本编译。

	> [!tip] 建议
	> 优先选择 **不带 `-goXXX` 后缀** 的版本。因为它们使用的是官方 CI/CD 默认的主流稳定 Go 环境，通常也包含了最新的安全补丁和运行时优化。

3. **架构区别**：`amd64` vs `arm64`

	- **`amd64`**：适用于绝大多数传统的 Intel 或 AMD 处理器的 Windows 电脑。
	- **`arm64`**：适用于采用 ARM 架构芯片的 Windows 设备，例如搭载高通 Snapdragon X Elite / 8cx 的掌上设备或轻薄本（如 Surface Pro 11 / Copilot+ PC）。

> [!abstract] 选型指南（懒人包）
>
> 1. **绝大多数近年的主流 PC / 笔记本（Intel/AMD 处理器）**：下载 **`mihomo-windows-amd64-v3-<版本号>.zip`**（性能最佳且稳定）
> 2. **比较老的电脑（2013 年以前的旧设备）**：下载 **`mihomo-windows-amd64-v1-<版本号>.zip`**（或直接不带 v 的版本）
> 3. **Surface Pro 11、Mac 虚拟机（运行 Windows ARM）或高通芯片轻薄本**：下载 **`mihomo-windows-arm64-<版本号>.zip`**

#### 裸核运行脚本

在核心同级目录下创建 `mihomo.vbs` 文件：

```vb
set mihomo = CreateObject("WScript.Shell")
mihomo.Run "mihomo-windows-amd64.exe -d .", 0
```

> [!note] 说明
> `.vbs` 脚本的作用是以无窗口方式后台运行 mihomo 内核，避免出现命令行黑窗口。

#### 创建配置文件

在核心同级目录下创建 `config.yaml` 文件，该文件参考这个仓库：[Proxy - GitHub](https://github.com/loskyertt/Proxy)，然后把下载的 UI 目录放到与 `config.yaml` 同层级目录下（UI 目录就是 `zashboard` 或者 `MetaCubeXD`）。

#### 运行核心

运行 `mihomo.vbs` 文件，在浏览器访问 [http://localhost:9090/ui](http://localhost:9090/ui) 进入控制面板。

#### 开机启动

为 `mihomo.vbs` 文件创建快捷方式，置于以下目录：

- **所有用户的开机自启**（需要管理员权限）：

```
C:\ProgramData\Microsoft\Windows\Start Menu\Programs\Startup
```

- **当前用户的开机自启**：

按住 `Win + R`，输入 `shell:startup`，再按 `Enter` 打开。

```
C:\Users\<用户名>\AppData\Roaming\Microsoft\Windows\Start Menu\Programs\Startup
```

## 3.2 方式二：Docker + mihomo（推荐）

通过 Docker 容器运行 mihomo，隔离性更好，支持 TUN 模式。

> [!tip] 为什么推荐 Docker？
> - **环境隔离**：不污染宿主机系统，卸载干净利落
> - **可移植性**：配置好后可以连同镜像一起打包备份或推送到 Docker Hub
> - **TUN 支持**：通过 `--device /dev/net/tun` 和 `--cap-add=NET_ADMIN` 实现透明代理

### 3.2.1 自动部署

安装好 Docker 后，直接运行脚本：

```bash
sudo bash ~/dotfiles/proxy/set_proxy.sh
```

脚本会自动完成：检查 Docker 依赖 → 清理已有容器 → 创建/复用 Volume → 复制配置文件和 WebUI → 启动容器 → 验证运行状态。

### 3.2.2 手动部署

添加用户到 docker 组：

```bash
sudo usermod -aG docker $USER
newgrp docker
```

创建卷挂载目录：

```bash
docker volume create mihomo
```

拉取镜像：

```bash
docker pull registry.cn-hangzhou.aliyuncs.com/loskyertt/meta:1.18.10
```

创建容器实例（host 网络模式，推荐）：

```bash
docker run -itd \
    --name=mihomo \
    --restart=always \
    --network host \
    -v mihomo:/root/.config/mihomo \
    --device /dev/net/tun \
    --cap-add=NET_ADMIN \
    registry.cn-hangzhou.aliyuncs.com/loskyertt/meta:1.18.10
```

> [!info] 参数说明
> - `--device /dev/net/tun`：挂载 TUN 设备，支持透明代理
> - `--cap-add=NET_ADMIN`：授予网络管理权限，允许操作 TUN 接口
> - `--network host`：使用宿主机的网络栈，无需端口映射

也可使用 bridge 模式（需手动映射端口）：

```bash
docker run -itd \
    --name=mihomo \
    --restart=always \
    -p 7890:7890 -p 9090:9090 \
    -v mihomo:/root/.config/mihomo \
    --device /dev/net/tun \
    --cap-add=NET_ADMIN \
    registry.cn-hangzhou.aliyuncs.com/loskyertt/meta:1.18.10
```

> [!warning] bridge 模式注意
> 使用 bridge 网络模式时，TUN 模式可能无法正常工作，因为容器无法直接操作宿主机的路由表。如需 TUN 透明代理，请使用 host 网络模式。

复制配置文件和 UI：

```bash
sudo cp config.yaml /var/lib/docker/volumes/mihomo/_data/
sudo cp -r zashboard /var/lib/docker/volumes/mihomo/_data/ui
```

重启容器：

```bash
docker restart mihomo
```

---

# 4.参考教程

- [【进阶使用】Clash Meta 纯内核使用教程|多机场融合|规则自动更新|Tun虚拟网卡模式|避免DNS泄露|WebRTC泄露](https://www.youtube.com/watch?v=d-2vCYLjXHs&t=23s)
- [mihomo 超精简手搓白名单配置文件 裸核运行方法](https://linux.do/t/topic/236719)
