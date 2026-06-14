---
title: Firewall 常用指令
date: 2024-09-11 10:32:38
tags:
  - linux/系统配置
  - firewall
  - 网络
  - 防火墙
---

# 1. 检查 firewall

是否安装：

```bash
which firewalld
```

是否启动：

```bash
sudo systemctl status firewalld
```

---

# 2. TCP 与 UDP 简介

> [!abstract]- TCP 与 UDP 协议对比
> ## TCP（传输控制协议）
> - **定义**：TCP（Transmission Control Protocol）是一个面向连接的、可靠的传输协议，位于传输层（OSI 模型的第四层）。
> - **用途**：用于在网络上可靠地传输数据，确保数据包的顺序、完整性和无错误传输。常见应用包括网页浏览（HTTP/HTTPS）、电子邮件（SMTP/IMAP/POP3）、文件传输（FTP）等。
> - **特点**：
>   - 需要建立连接（三次握手）
>   - 提供数据完整性、顺序保证和错误检测
>   - 支持流量控制和拥塞控制
>
> ## UDP（用户数据报协议）
> - **定义**：UDP（User Datagram Protocol）是一个无连接的、轻量级的传输协议，也位于传输层。
> - **用途**：用于快速传输数据，但不保证数据的可靠性和顺序。常见应用包括视频流、实时语音通信（VoIP）、在线游戏等。
> - **特点**：
>   - 不需要建立连接
>   - 不保证数据包的顺序和完整性
>   - 速度快，开销小

---

# 3. 常用指令

## 3.1 基本指令

- 列出所有配置：

```bash
sudo firewall-cmd --list-all
```

- 重载设置（在添加新配置后需要做，以使配置生效）：

```bash
sudo firewall-cmd --reload
```

- 列出所有开放的端口：

```bash
sudo firewall-cmd --zone=public --list-ports
```

## 3.2 端口设置

- 开放端口：

```bash
firewall-cmd --add-port=80/tcp [--zone=public] [--permanent]
```

> [!info] 参数说明
> - `--zone=public`：指定区域
> - `--permanent`：永久开放该端口，重启后仍然生效。不加则为临时端口

- 查看所有可用区域：

```bash
firewall-cmd --get-zones
```

- 查看当前默认区域：

```bash
firewall-cmd --get-default-zone
```

- 删除端口：

```bash
sudo firewall-cmd --remove-port=80/tcp [--zone=public] [--permanent]
```
