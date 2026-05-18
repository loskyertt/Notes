---
title: 解决 Debian 下中文乱码问题
date: 2025-01-03 19:16:06
excerpt: "解决 Debian 下中文乱码的问题，主要是用于 docker 的 debian 镜像中。"
categories: "Linux教程"
---


# 1.安装语言包

首先需要安装 `locales` 这个软件包：

```bash
apt install locales
```

然后执行下面命令并配置语言环境：

```bash
dpkg-reconfigure locales
```

---

# 2.修改配置

修改 `~/.bashrc` 文件：

```bash
nano ~/.bashrc
```

添加：

```txt
export LANG=zh_CN.UTF-8
export LANGUAGE=zh_CN:zh
export LC_ALL=zh_CN.UTF-8
```

然后执行：

```bash
source ~/.bashrc
```

# 3.参考链接

- [Debian配置系统中文语言及环境](https://developer.aliyun.com/article/1143167)