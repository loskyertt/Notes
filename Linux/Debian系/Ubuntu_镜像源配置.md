---
title: 解决 Ubuntu 系统下，软件包更新和软件下载速度慢的问题
date: 2024-12-07 21:05:25
excerpt: false
categories: "Linux教程"
---

# 1.问题复现

在安装好 Ubuntu 系统后，一般第一步都是执行 `sudo apt update`，此时用的是自带的官方镜像源，速度很慢，所以我们会选择切换为国内的镜像源，这里以清华镜像源为例，默认已经配置好了清华镜像源。

在执行更新时：

```bash
root@5395055d0589:/# apt update
Ign:1 https://mirrors.tuna.tsinghua.edu.cn/ubuntu noble InRelease
Ign:2 https://mirrors.tuna.tsinghua.edu.cn/ubuntu noble-updates InRelease                                
Ign:3 https://mirrors.tuna.tsinghua.edu.cn/ubuntu noble-backports InRelease                              
Get:4 http://security.ubuntu.com/ubuntu noble-security InRelease [126 kB]                                
Get:5 http://archive.ubuntu.com/ubuntu noble InRelease [256 kB]                                          
Ign:1 https://mirrors.tuna.tsinghua.edu.cn/ubuntu noble InRelease 
Ign:2 https://mirrors.tuna.tsinghua.edu.cn/ubuntu noble-updates InRelease
Ign:3 https://mirrors.tuna.tsinghua.edu.cn/ubuntu noble-backports InRelease
Get:6 http://security.ubuntu.com/ubuntu noble-security/multiverse amd64 Packages [15.3 kB]
Get:7 http://security.ubuntu.com/ubuntu noble-security/universe amd64 Packages [726 kB]
Get:8 http://security.ubuntu.com/ubuntu noble-security/main amd64 Packages [627 kB]
Get:9 http://security.ubuntu.com/ubuntu noble-security/restricted amd64 Packages [607 kB]
Ign:1 https://mirrors.tuna.tsinghua.edu.cn/ubuntu noble InRelease                       
Ign:2 https://mirrors.tuna.tsinghua.edu.cn/ubuntu noble-updates InRelease
Ign:3 https://mirrors.tuna.tsinghua.edu.cn/ubuntu noble-backports InRelease
Get:10 http://archive.ubuntu.com/ubuntu noble-updates InRelease [126 kB]                                                                                                                          
Err:1 https://mirrors.tuna.tsinghua.edu.cn/ubuntu noble InRelease                                                                                                                                 
  Certificate verification failed: The certificate is NOT trusted. The certificate issuer is unknown.  Could not handshake: Error in the certificate verification. [IP: 101.6.15.130 443]
Err:2 https://mirrors.tuna.tsinghua.edu.cn/ubuntu noble-updates InRelease                                                                                                                         
  Certificate verification failed: The certificate is NOT trusted. The certificate issuer is unknown.  Could not handshake: Error in the certificate verification. [IP: 101.6.15.130 443]
Err:3 https://mirrors.tuna.tsinghua.edu.cn/ubuntu noble-backports InRelease                                                                                                                       
  Certificate verification failed: The certificate is NOT trusted. The certificate issuer is unknown.  Could not handshake: Error in the certificate verification. [IP: 101.6.15.130 443]
Get:11 http://archive.ubuntu.com/ubuntu noble-backports InRelease [126 kB]                                                                                                                        
Get:12 http://archive.ubuntu.com/ubuntu noble/main amd64 Packages [1808 kB]                                                                                                                       
Get:13 http://archive.ubuntu.com/ubuntu noble/restricted amd64 Packages [117 kB]                                                                                                                  
Get:14 http://archive.ubuntu.com/ubuntu noble/universe amd64 Packages [19.3 MB]                                                                                                                   
38% [14 Packages 3024 kB/19.3 MB 16%]                                                                                                                                           75.6 kB/s 4min 12s^C
```

# 2.解决办法

可以看到，当清华镜像源证书验证失败时，会尝试从 `/etc/apt/sources.list.d/ubuntu.sources` 这里的官方镜像源下载更新，但是下载速度特别慢，所以我通常会禁用这个。

```bash
mv /etc/apt/sources.list.d/ubuntu.sources /etc/apt/sources.list.d/ubuntu.sources.bak
```

然后更新的时候临时禁用证书验证：

```bash
apt-get -o Acquire::https::Verify-Peer=false -o Acquire::https::Verify-Host=false update
```

更新完毕后，可能在安装 `ca-certificates` 还会出现这样的问题：

```bash
root@5395055d0589:/# apt install ca-certificates
Reading package lists... Done
Building dependency tree... Done
Reading state information... Done
The following additional packages will be installed:
  openssl
The following NEW packages will be installed:
  ca-certificates openssl
0 upgraded, 2 newly installed, 0 to remove and 0 not upgraded.
Need to get 1162 kB of archives.
After this operation, 2326 kB of additional disk space will be used.
Do you want to continue? [Y/n] y
Ign:1 https://mirrors.tuna.tsinghua.edu.cn/ubuntu noble-updates/main amd64 openssl amd64 3.0.13-0ubuntu3.4
Ign:2 https://mirrors.tuna.tsinghua.edu.cn/ubuntu noble/main amd64 ca-certificates all 20240203
Ign:1 https://mirrors.tuna.tsinghua.edu.cn/ubuntu noble-updates/main amd64 openssl amd64 3.0.13-0ubuntu3.4
Ign:2 https://mirrors.tuna.tsinghua.edu.cn/ubuntu noble/main amd64 ca-certificates all 20240203
Ign:1 https://mirrors.tuna.tsinghua.edu.cn/ubuntu noble-updates/main amd64 openssl amd64 3.0.13-0ubuntu3.4
Ign:2 https://mirrors.tuna.tsinghua.edu.cn/ubuntu noble/main amd64 ca-certificates all 20240203
Err:1 https://mirrors.tuna.tsinghua.edu.cn/ubuntu noble-updates/main amd64 openssl amd64 3.0.13-0ubuntu3.4
  Certificate verification failed: The certificate is NOT trusted. The certificate issuer is unknown.  Could not handshake: Error in the certificate verification. [IP: 101.6.15.130 443]
Err:2 https://mirrors.tuna.tsinghua.edu.cn/ubuntu noble/main amd64 ca-certificates all 20240203
  Certificate verification failed: The certificate is NOT trusted. The certificate issuer is unknown.  Could not handshake: Error in the certificate verification. [IP: 101.6.15.130 443]
W: https://mirrors.tuna.tsinghua.edu.cn/ubuntu/pool/main/o/openssl/openssl_3.0.13-0ubuntu3.4_amd64.deb: No system certificates available. Try installing ca-certificates.
W: https://mirrors.tuna.tsinghua.edu.cn/ubuntu/pool/main/c/ca-certificates/ca-certificates_20240203_all.deb: No system certificates available. Try installing ca-certificates.
W: https://mirrors.tuna.tsinghua.edu.cn/ubuntu/pool/main/o/openssl/openssl_3.0.13-0ubuntu3.4_amd64.deb: No system certificates available. Try installing ca-certificates.
W: https://mirrors.tuna.tsinghua.edu.cn/ubuntu/pool/main/c/ca-certificates/ca-certificates_20240203_all.deb: No system certificates available. Try installing ca-certificates.
W: https://mirrors.tuna.tsinghua.edu.cn/ubuntu/pool/main/o/openssl/openssl_3.0.13-0ubuntu3.4_amd64.deb: No system certificates available. Try installing ca-certificates.
W: https://mirrors.tuna.tsinghua.edu.cn/ubuntu/pool/main/c/ca-certificates/ca-certificates_20240203_all.deb: No system certificates available. Try installing ca-certificates.
W: https://mirrors.tuna.tsinghua.edu.cn/ubuntu/pool/main/o/openssl/openssl_3.0.13-0ubuntu3.4_amd64.deb: No system certificates available. Try installing ca-certificates.
W: https://mirrors.tuna.tsinghua.edu.cn/ubuntu/pool/main/c/ca-certificates/ca-certificates_20240203_all.deb: No system certificates available. Try installing ca-certificates.
E: Failed to fetch https://mirrors.tuna.tsinghua.edu.cn/ubuntu/pool/main/o/openssl/openssl_3.0.13-0ubuntu3.4_amd64.deb  Certificate verification failed: The certificate is NOT trusted. The certificate issuer is unknown.  Could not handshake: Error in the certificate verification. [IP: 101.6.15.130 443]
E: Failed to fetch https://mirrors.tuna.tsinghua.edu.cn/ubuntu/pool/main/c/ca-certificates/ca-certificates_20240203_all.deb  Certificate verification failed: The certificate is NOT trusted. The certificate issuer is unknown.  Could not handshake: Error in the certificate verification. [IP: 101.6.15.130 443]
E: Unable to fetch some archives, maybe run apt-get update or try with --fix-missing?
```

从错误信息来看，`apt` 在尝试从清华镜像站下载 `ca-certificates` 和 `openssl` 时仍然遇到了证书验证失败的问题。这可能是由于现有的 `ca-certificates` 软件包本身损坏或缺失导致的。

因此可以在下载时临时禁用证书验证：

```bash
apt-get -o Acquire::https::Verify-Peer=false -o Acquire::https::Verify-Host=false install ca-certificates openssl
```