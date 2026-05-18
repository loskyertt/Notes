---
title: Openwrt 在 Docker 下运行并作为旁路由
date: 2024-07-30 01:32:03
excerpt: "这是一篇如何在 Linux 环境下，用 Docker 搭建 Openwrt 作旁路由的教程。"
categories: "计算机网络"
---


# 1.前言

这几天想试着玩一下 Openwrt 来作旁路由，但是又没有软路由固件，后来考虑到`Openwrt`也是基于 Linux 的，那么在 Docker Hub 上应该有其对应的镜像吧，然后查了下果真有。于是有了后续的操作 ... ...

**推荐的镜像：** [zzsrv/openwrt](https://hub.docker.com/r/zzsrv/openwrt)

# 2.准备工作

建议是在虚拟机上安装一个 Linux 系统，我在实体机上（Linux 系统）试了下，是能成功，但是在该电脑上无法访问 Openwrt 的 IP 地址，在其它设备（比如手机）能正常访问（需要多做一些配置）。

## 2.1 虚拟机安装

虚拟机在这里只推荐 [VirtualBox](https://www.virtualbox.org/wiki/Downloads)。

还需要下载 [插件](https://download.virtualbox.org/virtualbox/7.0.20/Oracle_VM_VirtualBox_Extension_Pack-7.0.20.vbox-extpack)
或者点图片这里红色方框处的下载连接：
![图一](https://im.gurl.eu.org/file/15123858a31e537afcf88.png)

下载完后直接双击下载好后的扩展，就能自动安装到 VirtualBox 中。

## 2.2 Linux安装

推荐的 Linux 发行版：

- [EndeavourOS](https://endeavouros.com/#Download) 
- [CachyOS](https://cachyos.org/download/)，建议下载 `Desktop Edition` 版本
- [LinuxMint](https://www.linuxmint.com/download.php)，建议下载 `Xfce Edition` 版本

前两个是 `Arch` 系的发行版，`LinuxMint` 是 `Debian` 系的发行版。我这里用的是`CachyOS`，因为正好想尝试下这个发行版，好处是配置有国内的镜像源，下载东西嘎嘎快。
在安装时最好选择有桌面环境，并且桌面选择 `xfce4`，毕竟轻量嘛，在虚拟机里用着也会更流畅。当然，也可以不需要桌面环境，那么在安装时就必须把语言设置为英语，因为终端界面的中文会是乱码，而且没有图形化界面也不好配置。

## 2.3 虚拟机的创建

![step1](https://im.gurl.eu.org/file/bd3ae00cf631639f8637d.png)

![step2](https://im.gurl.eu.org/file/d29cd0f0a8b49c85eba19.png)

![step3](https://im.gurl.eu.org/file/5fa57afd8c3f1ce85dbaf.png)

![step3](https://im.gurl.eu.org/file/3238679faab643583e0d2.png)

## 2.4 虚拟机的配置

创建完虚拟机后，需要对其进行配置：

- 网络配置：
![step1](https://im.gurl.eu.org/file/c0ac4b0af36dd0d8d8669.png)

- 显示配置
![step2](https://im.gurl.eu.org/file/30d7d1cc582bb21af4a3e.png)

- 镜像挂载
![step3](https://im.gurl.eu.org/file/fe9361ef16f1efec66bf1.png)

## 2.5 安装系统

然后就是安装 Linux 系统，网上有很多教程，就不在这里讲了。

---

# 3.Docker 配置

## 3.1 安装 Docker

```bash
sudo pamcn -S docker
```

可以通过下面指令查看是否安装成功：

```bash
sudo docker version
```

如果输出类似下面内容就代表安装成功：

```TXT
Client:
 Version:           27.1.1
 API version:       1.46
 Go version:        go1.22.5
 Git commit:        63125853e3
 Built:             Thu Jul 25 17:06:22 2024
 OS/Arch:           linux/amd64
 Context:           default

Server:
 Engine:
  Version:          27.1.1
  API version:      1.46 (minimum version 1.24)
  Go version:       go1.22.5
  Git commit:       cc13f95251
  Built:            Thu Jul 25 17:06:22 2024
  OS/Arch:          linux/amd64
  Experimental:     false
 containerd:
  Version:          v1.7.20
  GitCommit:        8fc6bcff51318944179630522a095cc9dbf9f353.m
 runc:
  Version:          1.1.13
  GitCommit:        
 docker-init:
  Version:          0.19.0
  GitCommit:        de40ad0
```

这里就不把当前用户组添加到`docker`了，总之记住，后续 `docker`的所有操作，都得用`sudo docker`。

## 3.2 Docker 服务配置

- 开机自启动：

```bash
sudo systemctl enable docker
```

- 运行`docker`：

```bash
sudo systemctl start docker
```

---

# 4.虚拟机内的网络配置

## 4.1 查看网络接口信息

- 查看网络接口：

```bash
ip a

# 或者（不一定能用，可能没有预装 net-tools）
ifconfig
```

- 输出结果：

```bash
~
❯ ip a
1: lo: <LOOPBACK,UP,LOWER_UP> mtu 65536 qdisc noqueue state UNKNOWN group default qlen 1000
    link/loopback 00:00:00:00:00:00 brd 00:00:00:00:00:00
    inet 127.0.0.1/8 scope host lo
       valid_lft forever preferred_lft forever
    inet6 ::1/128 scope host noprefixroute 
       valid_lft forever preferred_lft forever
2: enp0s3: <BROADCAST,MULTICAST,UP,LOWER_UP> mtu 1500 qdisc fq_codel state UP group default qlen 1000
    link/ether 08:00:27:ff:93:f7 brd ff:ff:ff:ff:ff:ff
    inet 192.168.2.23/24 brd 192.168.2.255 scope global dynamic noprefixroute enp0s3
       valid_lft 85878sec preferred_lft 85878sec
    inet6 fe80::d521:edc1:580f:1e00/64 scope link noprefixroute 
       valid_lft forever preferred_lft forever
3: docker0: <NO-CARRIER,BROADCAST,MULTICAST,UP> mtu 1500 qdisc noqueue state DOWN group default 
    link/ether 02:42:8a:53:e6:cf brd ff:ff:ff:ff:ff:ff
    inet 172.17.0.1/16 brd 172.17.255.255 scope global docker0
       valid_lft forever preferred_lft forever
```

这里需要着重记下第二个 `enp0s3` 接口，第一个和第三个不用管。从 `inet 192.168.2.23/24` 可以知道虚拟机的 `ipaddr` 是 `192.168.2.23`。

**下面是三个网络接口的详细解释：**

1. **`lo`（Loopback接口）**
- **描述：**  这是本地回环接口，用于主机内部的网络通信。它的IP地址是`127.0.0.1`，也被称为`localhost`。
- **属性**:
  - **IP 地址：** `127.0.0.1/8` 和 `::1/128`
  - **用途：** 允许主机内部进程进行网络通信，而不实际发送数据到网络接口外部。适用于测试和进程间通信。
  - **状态：** 总是`UP`，表示接口始终可用。

2. **`enp0s3`（以太网接口）**
- **描述：** 这是一个物理以太网接口，通常用于连接到网络交换机或路由器。
- **属性：** 
  - **MAC 地址：** `08:00:27:ff:93:f7`
  - **IP 地址：** `192.168.2.23/24`，子网掩码是 24 位（`255.255.255.0`）。
  - **IPv6 地址：** `fe80::d521:edc1:580f:1e00/64`
  - **状态：** `UP`，表示接口已激活并连接到网络。
  - **用途：** 用于连接到局域网或互联网，并提供主机的网络连接。

1. **`docker0`（Docker 虚拟网桥）**
- **描述：** 这是 Docker 默认创建的虚拟网桥，所有未指定其他网络的 Docker 容器默认连接到这个桥接网络。
- **属性**:
  - **MAC 地址：** `02:42:8a:53:e6:cf`
  - **IP 地址：** `172.17.0.1/16`，这是 Docker 的默认子网，用于容器之间的网络通信。
  - **状态：** `DOWN`，表示虚拟网桥目前没有活动的连接。即使接口状态是`DOWN`，Docker 可以在需要时自动启用它。
  - **用途：** 提供一个虚拟的网络环境，使 Docker 容器能够互相通信并访问网络。

## 4.2 开启混杂模式(promisc)

开启网卡混杂模式，且虚拟机和 docker 里都要开启网卡混杂模式，这里网卡标记根据自己的填写：

```bash
sudo ip link set enp0s3 promisc on
```

注意：在我这里是对 `enp0s3`，但是每台电脑情况可能不一样，反正肯定是有 `ipaddr` 的这个网络接口名。

## 4.3 建立 docker 子网

专门建立一个子网，且该网络与家里的局域网在一个网段，网段和网关填自己的：

```bash
sudo docker network create -d macvlan --subnet=192.168.2.0/24 --gateway=192.168.2.1 -o parent=enp0s3 macnet
```

参数说明：
- `subnet` 是要写你自己路由器所在的 **网络**。
- `gateway` 是要写你自己路由器所在的那个 **网关** ，也就是路由器的 IP 地址。
- `parent` 是虚拟（子）网卡的真实父级网卡，也就是刚才看的本地网卡（第二个网络接口）。
- `macnet` 是子网名。

> 比如前面我们知道主机 `enp0s3` 网络接口的 IP 地址 CIDR 形式是 `192.168.2.23/24`，子网掩码是 24 位（`255.255.255.0`），拿 `192.168.2.23` 和 `255.255.255.0` **相与** 就能得到主机所在的网络，即 `192.168.2.0`。

可以查看网卡是否创建成功：

```bash
sudo docker network ls
```

输出结果如下：

```TXT
NETWORK ID     NAME      DRIVER    SCOPE
2731fcf199c8   bridge    bridge    local
556f62e40e30   host      host      local
877d2632a9d1   macnet    macvlan   local
5864881e2dd0   none      null      local
```

解释：
这四个网络接口是在Docker中使用的不同类型的网络驱动程序。每个网络驱动程序具有不同的用途和功能：

1. **`bridge` 网络**
- **驱动程序**: `bridge`
- **作用**: 默认的网络驱动程序，提供容器之间的网络通信。它创建了一个虚拟网络桥（`docker0`），所有使用 `bridge` 网络的容器都会连接到这个桥上。
- **使用场景** : 当你使用 docker 创建容器并不指定自定义网络时，docker 默认会将容器连接到 `bridge` 网络。容器可以通过这个网络相互通信，也可以访问宿主机的网络。

2. **`host` 网络**
- **驱动程序**：`host`
- **作用** : 将容器的网络堆栈与宿主机的网络堆栈直接连接。这意味着容器不会获得一个独立的 IP 地址，而是使用宿主机的 IP 地址。容器内的网络设置将与宿主机相同。
- **使用场景**: 当你需要容器与宿主机之间的网络高度集成，或者需要容器直接使用宿主机的网络接口时，使用 `host` 网络。常用于 **性能优化** 或特殊网络配置需求。

3. **`macnet` 网络**
- **驱动程序**：`macvlan`
- **作用**: 提供 MAC 地址级别的网络隔离。每个连接到 `macvlan` 网络的容器可以被分配一个独立的 MAC 地址，从而在网络中表现为独立的物理设备。
- **使用场景：** 当需要在物理网络中隔离不同的容器，并且每个容器需要有一个独立的 MAC 地址时，使用 `macvlan` 网络。适用于需要直接与物理网络交换数据的容器，例如在一些网络设备上工作时。

4. **`none` 网络**
- **驱动程序：** `null`
- **作用：** 容器不使用任何网络接口。它会创建一个没有网络连接的容器环境。
- **使用场景：** 当你希望容器完全隔离于网络之外，不与任何网络通信时，使用 `none` 网络。这适用于需要完全网络隔离的容器场景，如某些安全或测试需求。

删除网卡(前提是要停止正在使用该网卡的容器)：

```bash
sudo docker network rm <网卡名>
```

---

# 5.搭建 Openwrt

## 5.1 拉取镜像

可以通过阿里云镜像提升镜像拉取速度：

```bash
sudo docker pull registry.cn-hangzhou.aliyuncs.com/zzsrv/openwrt:latest
```

## 5.2 创建容器

```bash
sudo docker run --restart always --name opt3 -d --network macnet --ip 192.168.2.3 --privileged registry.cn-hangzhou.aliyuncs.com/zzsrv/openwrt:latest /sbin/init
```

参数说明：
- `run --restart`：设置容器的重启策略为“总是重启”。如果容器停止或 docker 守护进程重启，docker 将自动重启这个容器。
- `--network macnet`：指定容器连接到名为 `macnet`（我们刚刚创的那个子网卡）的`Docker`网络。
- `--ip 192.168.2.3`：为容器分配一个静态 IP 地址 `192.168.2.3`，在 `macnet` 网络范围内。一定要确保该 IP 地址在网络的子网中并且未被其他设备占用。
- `/sbin/init`：容器启动时要执行的命令或程序。容器将运行 `/sbin/init`，这是 Openwrt 系统中的初始化进程，通常是启动系统的第一步。

## 5.3 容器配置

进入容器内部：

```bash
sudo docker exec -it opt3 /bin/sh
```

配置网络：

```bash
vi /etc/config/network
```

按 `i` 键进入编辑，`esc` 键退出编辑，`:wq` 保存并退出。

需要把 `ipaddr` 设置为在创建容器时设置的 IP 地址，即 `192.168.2.3`；`gateway` 和 `dns` 都设置为网关地址（路由器地址），我的是 `192.168.2.1`。
![如图所示](https://im.gurl.eu.org/file/5cbd9d8cdb9722e37a8eb.png)

重置网络使网络配置生效：

```bash
/etc/init.d/network restart
```

到这一步，整个流程就彻底结束了，回到主机，在浏览器输入地址 `192.168.2.3`（你自己的容器设置的 IP 地址），应该就能进入 Openwrt 的登录界面了，默认是没有密码可以直接点击登录的。

![登录界面](https://im.gurl.eu.org/file/e906fa1645fab9b347b8d.png)

后面我先学一下怎么用 OpenClash，然后更新用 OpenClash 科学上网的教程。

---

# 6.实体机中安装 docker + OpenWrt 的配置

## 6.1 问题说明

前面的步骤与在虚拟机中的一样，但是在创建并配置好 OpenWrt 的容器后，你会发现在宿主机中无法访问 `192.168.2.3`（创建的容器的 IP 地址），但是其它设备（前提是在同一个网关下，比如手机）是能成功访问 `192.168.2.3` 的。

> 原因：在 docker 中使用 `macvlan` 网络驱动时，容器和宿主机的网络栈是分离的。这意味着：
> - 容器的网络接口 `192.168.2.3` 是在 `macvlan` 网络上，而宿主机的网络接口不能直接与 `macvlan` 网络中的容器进行通信。
> - 因为 `macvlan` 网络的特性，宿主机的网络接口不会自动知道 `macvlan` 网络中的设备。

## 6.2 解决办法

### 6.2.1 创建 maclan 接口

创建一个名为 `macvlan0` 的新虚拟网络接口，它基于物理接口 `enp0s3`：

```bash
sudo ip link add macvlan0 link enp0s3 type macvlan mode bridge
```

`macvlan` 允许在一个物理网络接口上创建多个虚拟网络接口，每个都有自己的 MAC 地址。桥接模式允许 macvlan 接口相互通信。

为这个新接口分配了 IP 地址 `192.168.2.250`，使其成为 `192.168.2.0/24` 网络的一部分：

```bash
sudo ip addr add 192.168.2.250/24 dev macvlan0
```

激活这个新接口，使其能够开始网络通信：

```bash
sudo ip link set macvlan0 up
```

注意需要把 `enp0s3` 替换为自己实际的网络驱动接口。这个时候在浏览器输入 `192.168.2.3` 就拿正常访问了。

这种配置的主要用途是允许主机直接参与到 `macvlan` 网络中，这对于与 docker macvlan 网络中的容器通信特别有用。它创建了一个"桥梁"，让主机能够与 macvlan 网络中的设备（如 Docker 容器）进行直接通信，而不需要通过 Docker 的网络栈。

### 6.2.2 撤销创建的 macvlan 配置

如果要删除创建的 `macvlan` 接口，可以进行以下操作：

1. 删除 IP 地址:

```bash
sudo ip addr del 192.168.2.250/24 dev macvlan0
```

2. 关闭 macvlan 接口:

```bash
sudo ip link set macvlan0 down
```

3. 删除 macvlan 接口:

```bash
sudo ip link del macvlan0
```

解释：
- `ip addr del`: 删除之前添加的 IP 地址
- `ip link set down`: 关闭网络接口
- `ip link del`: 完全删除网络接口

执行这些命令后，你创建的 `macvlan` 接口及其配置就会被完全移除。

额外提示：

## 6.3 查看当前配置

1. 在执行这些步骤之前和之后，你可以使用以下命令查看网络配置，以确保更改已经生效：

```bash
ip addr show
ip link show
```

2. 保存配置：如果你经常需要切换这些配置，可以考虑创建脚本来自动化这个过程。例如，创建一个脚本来添加配置，另一个脚本来删除配置。

3. 系统重启：请注意，这些更改在系统重启后不会保持。如果你希望这些更改是永久的，需要将它们添加到网络配置文件中或创建一个系统启动脚本。

4. 小心操作：在执行这些命令时要小心，特别是如果你是通过远程连接（如 SSH）管理系统。错误的网络配置可能会导致你失去对系统的访问。

---

# 7.参考教程

- [docker + openwrt把windows变成最强软路由，游戏、翻墙两不误(Linux-Mint/ubuntu、VirtualBox、docker、openwrt)](https://www.youtube.com/watch?v=6OeGOK2-1zo&list=PL7XUZPsvfw3CKvDijuA5CO_oLbBwJCYTt&index=4)
- [Openwrt Docker 镜像 安装详细过程实现科学上网，没有设备也可以体验|软路由|旁路网关](https://www.youtube.com/watch?v=mlsTzDsBhLs&list=PL7XUZPsvfw3CKvDijuA5CO_oLbBwJCYTt&index=6)
- [zzsrv/openwrt](https://hub.docker.com/r/zzsrv/openwrt)