---
title: Debian 系镜像源配置与证书问题
date: 2024-12-03 14:53:04
tags:
  - linux/debian系
  - apt
  - 镜像源
  - 证书
aliases:
  - Debian 镜像源
  - Ubuntu 镜像源
---

# 1. 问题描述

在安装好 Debian/Ubuntu 系统后，一般第一步都是执行 `sudo apt update`。此时如果用的是自带的官方镜像源，速度很慢，所以我们会选择切换为国内的镜像源（如清华镜像源）。

然而在执行更新时，可能会遇到 **HTTPS 证书验证失败**的问题：

```
Err:1 https://mirrors.tuna.tsinghua.edu.cn/debian bookworm InRelease
  Certificate verification failed: The certificate is NOT trusted. The certificate issuer is unknown.  Could not handshake: Error in the certificate verification. [IP: 101.6.15.130 443]
W: No system certificates available. Try installing ca-certificates.
```

> [!bug] 死循环问题
> 问题的核心是系统无法验证 HTTPS 源的证书，需要安装 `ca-certificates`，但安装 `ca-certificates` 本身又需要通过 HTTPS 下载——形成了一个==死循环==。

---

# 2. 解决办法

## 2.1 方式一：临时禁用 HTTPS 验证（推荐）

> [!tip] 推荐方式
> 这是最快捷的解决方式，临时绕过证书验证，安装好 `ca-certificates` 后再恢复正常验证。

**Debian：**

编辑 `/etc/apt/apt.conf.d/99disable-https-check`：

```bash
sudo nano /etc/apt/apt.conf.d/99disable-https-check
```

添加以下内容：

```plaintext
Acquire::https::Verify-Peer "false";
Acquire::https::Verify-Host "false";
```

保存后运行：

```bash
sudo apt update
sudo apt install ca-certificates
```

> [!warning] 安装完成后务必恢复
> 安装完 `ca-certificates` 后，删除或注释掉该配置以恢复安全性。

也可以直接在命令行临时禁用（无需修改配置文件）：

```bash
apt -o Acquire::https::Verify-Peer=false -o Acquire::https::Verify-Host=false update
```

**Ubuntu：**

同样使用命令行临时禁用：

```bash
apt-get -o Acquire::https::Verify-Peer=false -o Acquire::https::Verify-Host=false update
```

安装 `ca-certificates` 和 `openssl`：

```bash
apt-get -o Acquire::https::Verify-Peer=false -o Acquire::https::Verify-Host=false install ca-certificates openssl
```

> [!info] Ubuntu 额外步骤
> Ubuntu 中，当清华镜像源证书验证失败时，会尝试从 `/etc/apt/sources.list.d/ubuntu.sources` 中的官方镜像源下载，但速度特别慢。可以临时禁用：
>
> ```bash
> mv /etc/apt/sources.list.d/ubuntu.sources /etc/apt/sources.list.d/ubuntu.sources.bak
> ```

## 2.2 方式二：手动安装 `ca-certificates` 包

如果方式一不可行，可以手动下载并安装。

### 2.2.1 下载 DEB 包

1. 在另一台可以正常联网的机器上，访问 [Debian Packages](https://packages.debian.org/) 或 [Ubuntu Packages](https://packages.ubuntu.com/)。
2. 下载与系统版本匹配的 `ca-certificates` 包。例如，适用于 Debian Bookworm 的链接：
   ```
   https://packages.debian.org/bookworm/all/ca-certificates/download
   ```

3. 使用 USB 或其他方法将下载的 `.deb` 文件复制到目标系统。

### 2.2.2 手动安装

```bash
sudo dpkg -i /path/to/ca-certificates*.deb
sudo apt update
```

---

# 3. 常见国内镜像源

> [!abstract]- 清华大学镜像源（TUNA）
> - **Debian**：`https://mirrors.tuna.tsinghua.edu.cn/debian/`
> - **Ubuntu**：`https://mirrors.tuna.tsinghua.edu.cn/ubuntu/`
> - 官网：https://mirrors.tuna.tsinghua.edu.cn/

> [!abstract]- 阿里云镜像源
> - **Debian**：`https://mirrors.aliyun.com/debian/`
> - **Ubuntu**：`https://mirrors.aliyun.com/ubuntu/`
> - 官网：https://developer.aliyun.com/mirror/

> [!abstract]- 中科大镜像源（USTC）
> - **Debian**：`https://mirrors.ustc.edu.cn/debian/`
> - **Ubuntu**：`https://mirrors.ustc.edu.cn/ubuntu/`
> - 官网：https://mirrors.ustc.edu.cn/

> [!note] 相关笔记
> 中文乱码问题参见 [[Debian系_中文乱码]]，Docker 环境配置参见 [[Docker_配置]]。
