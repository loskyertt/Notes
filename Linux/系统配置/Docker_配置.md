---
title: Linux 环境下配置 Docker
date: 2024-07-20 16:21:15
excerpt: "这是一篇 Docker 在 Linux 下的一些配置教程。"
categories: "Linux教程"
---


# 1. Docker 配置

## 1.1 安装 Docker

- **安装方式一（推荐）：**

通过包管理器直接安装（全局安装）：

```bash
sudo pacman -Sy docker docker-compose
```

`docker-compose` 可选安装，它可以通过配置文件 Dockerfile 来构建镜像。

- **安装方式二：**

通过 conda 安装（局部安装，可全局使用）：

首先需要创建虚拟环境用于存储安装的 docker 和 docker-compose。

```bash
conda create --name docker python=3.12
```

Python版本可自己选择。

```bash
conda install docker docker-compose -c conda-forge
```

这里的 `-c conda-forge` 一定要加，默认的 `conda.org` 源没有 `docker`。

安装好的`docker`和`docker-compose`是在该路径下：
- ~/.conda/envs/docker/bin/docker
- ~/.conda/envs/docker/bin/docker-compose

## 1.2 docker 基本操作

### 1.2.1 docker 服务

- 启动 docker 服务

```bash
sudo systemctl start docker
```

- 关闭 docker 服务

```bash
sudo systemctl stop docker.service docker.socket
```

- 检查 docker 服务状态

```bash
sudo systemctl status docker
```

- 开机自启动设置

```bash
sudo systemctl enable --now docker
```

### 1.2.2 添加用户到 docker 组

为了避免每次运行 docker 命令时都需要使用 sudo，可以将当前用户添加到 docker 组：

```sh
sudo usermod -aG docker $USER
```

然后，重新登录以使更改生效，或者重新加载用户组信息：

登录：

```sh
newgrp docker
```

退出：

```bash
exit
```

可能需要重启系统来让配置生效。

## 1.3 代理配置

> 如果使用代理软件的 Tun 模式，就不需要配置。

1.创建 docker 相关的 systemd 目录，这个目录下的配置将覆盖 docker 的默认配置：

```bash
sudo mkdir -p /etc/systemd/system/docker.service.d
```

2.新建配置文件：

```bash
kate /etc/systemd/system/docker.service.d/proxy.conf
```

将以下内容复制到 proxy.conf 中：

```txt
[Service]
Environment="HTTP_PROXY=http://127.0.0.1:7890"
Environment="HTTPS_PROXY=http://127.0.0.1:7890"
```

这里需要根据自己实际的代理端口填写。

3.如果自己建了私有的镜像仓库，需要 docker 绕过代理服务器直连，那么配置 NO_PROXY 变量：

```
[Service]
Environment="HTTP_PROXY=http://127.0.0.1:7890"
Environment="HTTPS_PROXY=http://127.0.0.1:7890"
Environment="NO_PROXY=your-registry.com,10.10.10.10,*.example.com"
```

多个 `NO_PROXY` 变量的值用逗号分隔，而且可以使用通配符（`*`），极端情况下，如果 `NO_PROXY=*`，那么所有请求都将不通过代理服务器。

4.重新加载配置文件，重启 docker：

```bash
sudo systemctl daemon-reload
```

```bash
sudo systemctl restart docker
```

5.检查确认环境变量已经正确配置：

```bash
sudo systemctl show --property=Environment docker
```

像输出以下内容就成功了：

```
Environment=HTTP_PROXY=http://127.0.0.1:7890 HTTPS_PROXY=http://127.0.0.1:7890
```

## 1.4 命令简化配置

**以下推荐加到 .bashrc 或者 .zshrc 中。**

1. 查看容器信息：

```bash
docker ps --format "table {{.ID}}\t{{.Image}}\t{{.Ports}}\t{{.Status}}\t{{.Names}}"
```

命令简化：

```txt
alias dps='docker ps --format "table {{.ID}}\t{{.Image}}\t{{.Ports}}\t{{.Status}}\t{{.Names}}"'
```

2. 查看网络配置：

```bash
docker inspect --format='{{.HostConfig.NetworkMode}}' <容器名/容器ID>
```

命令简化：

```txt
alias din='docker inspect --format='{{.HostConfig.NetworkMode}}''
```