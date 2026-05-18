---
title: Zerotier 使用教程
date: 2024-08-18 13:25:39
excerpt: "主要记录平时用到的`ssh`指令以及使用方法。"
categories: "计算机网络"
---

# 1.登录或注册 Zerotier 官网帐号

[官网地址](https://www.zerotier.com/)

## 1.1 创建组网

点击`Create A NetWork`

[![创建网络](https://s21.ax1x.com/2024/08/18/pACzRbV.md.png)](https://imgse.com/i/pACzRbV)

蓝色框里的是我已经创建好了的。创建好后可以点击进入并管理。

---

# 2.安装 Zerotier

## 2.1 常规安装

请参考官网下载链接：[Download](https://www.zerotier.com/download/)

## 2.2 Arch 系 Linux 安装

通过`pacman`安装：
```bash
sudo pacman -Sy zerotier-one
```

设置开机自启动，并启动`zerotier-one`服务：
```bash
sudo systemctl enable --now zerotier-one
```

---

# 3.加入创建的组网

红色框：ID （是一个 16 字节的十六进制字符串）要复制下来
紫色框处：要勾选上
黑色框：用户可以自定义命名加入的设备
绿色框：分配的 IP 地址，用于连接使用
[![管理](https://s21.ax1x.com/2024/08/18/pACzhUU.png)](https://imgse.com/i/pACzhUU)

## 3.1 加入组网

其它设备直接在软件里输入组网 ID 就能直接加入了，但是要确定防火墙不能组织 Zerotier 流量。对于 Linux 设备按如下方式：

- 加入组网：
```bash
sudo zerotier-cli join <network-id>
```

- 验证加入状态:
```bash
sudo zerotier-cli listnetworks
```

## 3.2 在 ZeroTier 管理控制台上授权设备
登录到 ZeroTier 的管理控制台（[ZeroTier Central](https://my.zerotier.com/)），找到你创建的网络。你会看到一个待授权的设备列表。你需要在控制台中授权你的设备：

1. 在控制台的网络管理页面中，找到你的网络。
2. 在“成员”部分，你会看到一个类似 `e3918db483f0c385` 的设备 ID（这是你的设备）。
3. 勾选该设备，然后点击“授权”或“允许”按钮以授权设备加入网络。

### 3.3 验证网络连接

可以使用 `ping` 命令来测试与网络中其他设备的连接：
```bash
ping <zerotier-ip-address>
```
替换 `<zerotier-ip-address>` 为网络中其他设备的 ZeroTier IP 地址。

