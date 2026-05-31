---
title: Win10 SSH 服务配置
date: 2024-07-30 01:37:03
excerpt: "如何在 Win10 下安装 SSH 服务，以及远程主机应该进行什么样的配置。"
categories: "ssh教程"
---


# 1.Windows 10 下安装 SSH 服务（Openssh）

## 1.1 下载

[ **Openssh 下载地址** ](https://github.com/PowerShell/Win32-OpenSSH)

## 1.2 解压

解压到如下目录（建议加压到`C:\Program Files`下，不然会出现未知错误）：

```bash
C:\Program Files\OpenSSH_Win64\
```

>> 也可直接下载安装后缀是 `.msi` 的文件，如 `OpenSSH-Win64-v10.0.0.0.msi`。下载好后，双击安装即可。
## 1.3 安装

**注意：** 一定要使用`powershell`并且要以管理员的身份打开。
- Windows 下开启运行执行`*.sp1`文件类型：
```bash
set-executionpolicy remotesigned
```

- 安装`sshd`
```bash
cd C:\Program Files\OpenSSH_Win64\

./install-sshd.ps1
```

## 1.4 测试连接

- 验证是否安装成功：
```bash
ssh -V
```

- 测试是否能与远程主机进行连接：
在与远程主机连接之前确保能`ping`通，然后通过`ssh <username>@<PCIPAddr>`与远程主机进行连接。
