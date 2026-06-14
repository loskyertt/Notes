---
title: Debian 系解决中文乱码问题
date: 2024-11-30 21:46:27
tags:
  - linux/debian系
  - 中文
  - locale
  - docker
aliases:
  - Debian 中文乱码
  - Ubuntu 中文乱码
---

# 1. 安装语言包

> [!info] 适用范围
> 以下方法适用于 Debian 和 Ubuntu 系统，==特别是 Docker 容器中的最小化镜像==（默认不含中文语言包）。

- **Debian：**

```bash
sudo apt install locales
sudo dpkg-reconfigure locales
```

- **Ubuntu：**

```bash
sudo apt-get install language-pack-zh-hans
```

> [!tip] Docker 容器中无需 sudo
> 在 Docker 的 Debian/Ubuntu 容器中默认是 root 权限，不需要加 `sudo`。

---

# 2. 配置语言环境

两种方式任选其一。

## 2.1 方式一：修改 `~/.bashrc`（推荐终端用户）

> [!tip] 推荐场景
> 如果只会用到终端环境，推荐使用这种方法，仅对当前用户生效。

```bash
nano ~/.bashrc
```

在文件末尾添加：

```bash
export LANG=zh_CN.UTF-8
export LANGUAGE=zh_CN:zh
export LC_ALL=zh_CN.UTF-8
```

然后执行：

```bash
source ~/.bashrc
```

## 2.2 方式二：修改 `/etc/locale.gen`（全局生效）

> [!info] 通用方法
> 此方式适用于所有 Linux 操作系统，对所有用户生效。

```bash
sudo nano /etc/locale.gen
```

找到以下行并取消注释（删除行前的 `#`）：

```bash
zh_CN.UTF-8 UTF-8
```

如果没有找到 `zh_CN.UTF-8`，则手动添加这一行。

保存文件后，运行 `locale-gen` 命令来生成新的语言环境：

```bash
sudo locale-gen
```

然后重启系统即可。

---

# 3. 中文空格乱码

> [!bug] 症状
> 中文字符能显示，但空格位置出现乱码或方块，这是因为缺少相应的中文字体。

安装中文字体包：

- **Debian：**

```bash
sudo apt-get install fonts-droid-fallback ttf-wqy-zenhei ttf-wqy-microhei fonts-arphic-ukai fonts-arphic-uming
```

- **Ubuntu：**

```bash
sudo apt-get install fonts-droid-fallback ttf-wqy-zenhei ttf-wqy-microhei fonts-arphic-ukai fonts-arphic-uming
```

> [!note] 相关笔记
> Linux 下安装字体的通用方法参见 [[安装字体]]。
