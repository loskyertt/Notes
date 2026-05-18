---
title: 解决 Debian 系统下，软件包更新和软件下载速度慢的问题
date: 2024-12-03 14:53:04
excerpt: false
categories: "Linux教程"
---

# 1.问题复现

在安装后 Debian 系统后，一般第一步都是执行 `sudo apt update`，此时用的是自带的官方镜像源，速度很慢，所以我们会选择切换为国内的镜像源，这里以清华镜像源为例，默认已经配置好了清华镜像源。

在执行更新时：

```bash
sky@DESKTOP-IU948AR:~$ sudo apt update
[sudo] password for sky: 
Err:1 https://security.debian.org/debian-security bookworm-security InRelease
  Certificate verification failed: The certificate is NOT trusted. The certificate issuer is unknown.  Could not handshake: Error in the certificate verification. [IP: 151.101.2.132 443]
Err:2 https://mirrors.tuna.tsinghua.edu.cn/debian bookworm InRelease
  Certificate verification failed: The certificate is NOT trusted. The certificate issuer is unknown.  Could not handshake: Error in the certificate verification. [IP: 101.6.15.130 443]
Err:3 https://mirrors.tuna.tsinghua.edu.cn/debian bookworm-updates InRelease
  Certificate verification failed: The certificate is NOT trusted. The certificate issuer is unknown.  Could not handshake: Error in the certificate verification. [IP: 101.6.15.130 443]
Err:4 https://mirrors.tuna.tsinghua.edu.cn/debian bookworm-backports InRelease
  Certificate verification failed: The certificate is NOT trusted. The certificate issuer is unknown.  Could not handshake: Error in the certificate verification. [IP: 101.6.15.130 443]
Reading package lists... Done
Building dependency tree... Done
Reading state information... Done
All packages are up to date.
W: https://mirrors.tuna.tsinghua.edu.cn/debian/dists/bookworm/InRelease: No system certificates available. Try installing ca-certificates.    
W: https://security.debian.org/debian-security/dists/bookworm-security/InRelease: No system certificates available. Try installing ca-certificates.
W: https://mirrors.tuna.tsinghua.edu.cn/debian/dists/bookworm-updates/InRelease: No system certificates available. Try installing ca-certificates.
W: https://mirrors.tuna.tsinghua.edu.cn/debian/dists/bookworm-backports/InRelease: No system certificates available. Try installing ca-certificates.
W: Failed to fetch https://mirrors.tuna.tsinghua.edu.cn/debian/dists/bookworm/InRelease  Certificate verification failed: The certificate is NOT trusted. The certificate issuer is unknown.  Could not handshake: Error in the certificate verification. [IP: 101.6.15.130 443]
W: Failed to fetch https://mirrors.tuna.tsinghua.edu.cn/debian/dists/bookworm-updates/InRelease  Certificate verification failed: The certificate is NOT trusted. The certificate issuer is unknown.  Could not handshake: Error in the certificate verification. [IP: 101.6.15.130 443]    
W: Failed to fetch https://mirrors.tuna.tsinghua.edu.cn/debian/dists/bookworm-backports/InRelease  Certificate verification failed: The certificate is NOT trusted. The certificate issuer is unknown.  Could not handshake: Error in the certificate verification. [IP: 101.6.15.130 443]  
W: Failed to fetch https://security.debian.org/debian-security/dists/bookworm-security/InRelease  Certificate verification failed: The certificate is NOT trusted. The certificate issuer is unknown.  Could not handshake: Error in the certificate verification. [IP: 151.101.2.132 443]  
W: Some index files failed to download. They have been ignored, or old ones used instead.

sky@DESKTOP-IU948AR:~$ sudo apt install ca-certificates
Reading package lists... Done
Building dependency tree... Done
Reading state information... Done
Package ca-certificates is not available, but is referred to by another package.
This may mean that the package is missing, has been obsoleted, or
is only available from another source

E: Package 'ca-certificates' has no installation candidate
```

可以看出更新失败，需要我们安装 `ca-certificates`，但是又无法成功安装 `ca-certificates`。

---

# 2.解决办法

> 这个问题的核心是系统无法验证 HTTPS 源的证书，导致无法更新包管理器的索引或安装新软件包。这可能是因为系统缺少必要的 CA 证书或某些配置问题。

## 2.1 方式一：手动安装 `ca-certificates` 包

### 2.1.1 下载 DEB 包

1. 在另一台可以正常联网的机器上，访问 [Debian Packages](https://packages.debian.org/)。
2. 下载与系统版本匹配的 `ca-certificates` 包。例如，适用于 Bookworm 的链接可能是：
   ```
   https://packages.debian.org/bookworm/all/ca-certificates/download
   ```

3. 使用 USB 或其他方法将下载的 `.deb` 文件复制到你的系统。

### 2.1.2 手动安装包

在目标机器上运行以下命令：

```bash
sudo dpkg -i /path/to/ca-certificates*.deb
sudo apt update
```

### 2.2 方式二：临时禁用 HTTPS 验证（推荐）

可以尝试临时禁用 HTTPS 验证以更新软件源和安装 `ca-certificates`：

编辑 `/etc/apt/apt.conf.d/99disable-https-check` 文件：

```bash
sudo nano /etc/apt/apt.conf.d/99disable-https-check
```

添加以下内容：

```plaintext
Acquire::https::Verify-Peer "false";
Acquire::https::Verify-Host "false";
```

保存后运行以下命令：

```bash
sudo apt update
sudo apt install ca-certificates
```

安装完 `ca-certificates` 后，删除或注释掉该配置以恢复安全性。

也可以直接在命令行临时禁用：

```bash
apt -o Acquire::https::Verify-Peer=false -o Acquire::https::Verify-Host=false update
```