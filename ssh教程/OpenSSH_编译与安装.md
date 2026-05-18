---
title: 从源码安装 SSH
date: 2024-09-11 11:24:33
excerpt: "完整的升级和安装 ssh 方案，主要用于服务器。这里以本地的 ubuntu docker 镜像作为例子来讲述。"
categories: "ssh教程"
---

# 1.构建基础镜像

在当期目录下（与`Dockerfile`同目录），添加`sources.list`文件，并添加镜像源：

```txt
# Ubuntu sources have moved to the /etc/apt/sources.list.d/ubuntu.sources
# file, which uses the deb822 format. Use deb822-formatted .sources files
# to manage package sources in the /etc/apt/sources.list.d/ directory.
# See the sources.list(5) manual page for details.

# 默认注释了源码镜像以提高 apt update 速度，如有需要可自行取消注释
deb https://mirrors.tuna.tsinghua.edu.cn/ubuntu/ noble main restricted universe multiverse
# deb-src https://mirrors.tuna.tsinghua.edu.cn/ubuntu/ noble main restricted universe multiverse
deb https://mirrors.tuna.tsinghua.edu.cn/ubuntu/ noble-updates main restricted universe multiverse
# deb-src https://mirrors.tuna.tsinghua.edu.cn/ubuntu/ noble-updates main restricted universe multiverse
deb https://mirrors.tuna.tsinghua.edu.cn/ubuntu/ noble-backports main restricted universe multiverse
# deb-src https://mirrors.tuna.tsinghua.edu.cn/ubuntu/ noble-backports main restricted universe multiverse

# 以下安全更新软件源包含了官方源与镜像站配置，如有需要可自行修改注释切换
# deb http://security.ubuntu.com/ubuntu/ noble-security main restricted universe multiverse
# deb-src http://security.ubuntu.com/ubuntu/ noble-security main restricted universe multiverse

# 预发布软件源，不建议启用
# deb https://mirrors.tuna.tsinghua.edu.cn/ubuntu/ noble-proposed main restricted universe multiverse
# # deb-src https://mirrors.tuna.tsinghua.edu.cn/ubuntu/ noble-proposed main restricted universe multiverse
```

[`Ubuntu`清华镜像源](https://mirrors.tuna.tsinghua.edu.cn/help/ubuntu/)

`Dockerfile`编写：

```Dockerfile
FROM ubuntu

ENV http_proxy=http://172.27.150.74:7890/
ENV https_proxy=http://172.27.150.74:7890/

RUN apt-get update && apt-get install -y nano wget sudo net-tools

COPY sources.list /etc/apt/

EXPOSE 22

WORKDIR /root/
```

根据自己实际情况设置代理环境（`ENV`）。

然后开始构建镜像：

```bash
docker build -t ubuntu:openssh .
```

---

# 2.创建容器

运行一个容器实例：

```bash
docker run -it --name=ssh ubuntu:openssh
```

---

# 3.从源码安装 SSH

要从源码安装和升级 OpenSSH，在 Ubuntu 上可以按照以下步骤操作：

## 3.1 准备环境

首先，确保系统中安装了编译 OpenSSH 所需的依赖包:

```bash
sudo apt update

sudo apt install build-essential zlib1g-dev libssl-dev libpam0g-dev libselinux1-dev
```

## 3.2 下载 OpenSSH 源码

从官方 OpenSSH 网站下载最新的源码压缩包：

```bash
wget https://cdn.openbsd.org/pub/OpenBSD/OpenSSH/portable/openssh-X.XX.tar.gz
```

将 `X.XX` 替换为最新版本的版本号，可以在 [OpenSSH 官网](https://www.openssh.com/portable.html) 查询最新版本。

解压缩下载的文件：

```bash
tar -xzf openssh-X.XX.tar.gz先设置下容器的`root`账户的
cd openssh-X.XX
```

## 3.3 编译

在编译 OpenSSH 之前，使用 `configure` 脚本来生成 Makefile。可以根据你的系统环境和需求指定一些配置选项：

```bash
./configure --prefix=/usr --sysconfdir=/etc/ssh --with-pam --with-ssl-engine
```

常用的选项包括：

- `--prefix=/usr`: 将安装路径设置为 `/usr` 而不是默认的 `/usr/local`。
- `--sysconfdir=/etc/ssh`: 指定配置文件目录为 `/etc/ssh`。
- `--with-pam`: 启用 PAM（可插拔认证模块）。
- `--with-ssl-engine`: 使用 OpenSSL 的 SSL 引擎。

你可以使用 `./configure --help` 来查看所有可用的配置选项。

配置完成后，编译源码：

```bash
make
```

编译完成后，可以通过 `make tests` 运行测试，以确保 OpenSSH 正确构建。

## 3.4 安装

在编译成功并通过测试后，使用 `make install` 来安装 OpenSSH。

```bash
sudo make install
```

这个命令将会把编译好的文件安装到系统中。

执行时可能会出现这种错误：

```txt
Privilege separation user sshd does not exist
```
该错误与`sshd`用户的缺失有关，这是 OpenSSH 用于权限隔离的一个功能。没有这个用户，SSH 守护进程可能无法正常启动。

解决办法：

- 创建 `sshd` 用户：

```bash
sudo useradd -r -d /var/empty -s /usr/sbin/nologin sshd
```

- 验证配置：

```bash
sudo /usr/sbin/sshd -t -f /etc/ssh/sshd_config
```

如果没有输出或错误，说明配置文件是正确的。

## 3.5 配置

安装完成后，检查 OpenSSH 的配置文件 `/etc/ssh/sshd_config`。

找到以下配置项，并确保它们没有被注释掉（前面没有`#`）并且值设置正确：

```bash
PasswordAuthentication yes
PermitRootLogin yes
```

记得设置容器中 `root` 用户的密码：

```bash
passwd
```

升级或重新安装可能会覆盖某些配置，因此你可能需要备份原有的配置文件并重新应用它们：

```bash
sudo cp /etc/ssh/sshd_config /etc/ssh/sshd_config.backup
```

编辑新的配置文件：

```bash
sudo nano /etc/ssh/sshd_config
```

## 3.6 重启 SSH 服务

安装完成后，需要重新启动 SSH 服务来应用新版本：

```bash
sudo systemctl restart ssh
```

你可以通过以下命令确认 SSH 版本是否更新：

```bash
ssh -V
```

环境中可能没有 `systemctl`，先检查下是否有 `service`：

```bash
which service
```

可以用 `service` 来控制：

```bash
sudo service ssh status
```

```bash
sudo service ssh start
```

如果 `service` 不可用，可以直接运行 `sshd` 来启动：

```bash
which sshd
```

运行：

```bash
/usr/sbin/sshd
```

## 3.7 防止自动更新覆盖

默认情况下，Ubuntu 使用 `apt` 来自动管理和更新软件包。如果你不希望 OpenSSH 通过 `apt` 更新被覆盖，可以将其锁定：

```bash
sudo apt-mark hold openssh-server openssh-client
```

这样可以确保 `apt` 不会自动更新 OpenSSH。

## 3.8 验证安装

最后，确保 OpenSSH 正常运行。通过以下命令验证 SSH 是否正在监听：

```bash
sudo systemctl status ssh
```

没有`systemctl`的话，可以用：

```bash
ps aux | grep sshd
```

输出是这样表示启动成功：

```bash
root       10106  0.0  0.0   8784  4508 ?        Ss   06:28   0:00 sshd: /usr/sbin/sshd [listener] 0 of 10-100 startups
root       10150  0.0  0.0   3528  1872 pts/0    S+   06:32   0:00 grep --color=auto sshd
```

然后在宿主机中测试：

```bash
ssh root@<container_IP>
```