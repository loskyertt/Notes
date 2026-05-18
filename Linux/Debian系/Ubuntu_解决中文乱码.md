---
title: 解决 Ubuntu 下中文乱码问题
date: 2024-11-30 21:46:27
excerpt: "解决 Ubuntu 下中文乱码的问题，主要是用于 docker 的 ubuntu 镜像中。"
categories: "Linux教程"
---

# 1.安装中文字体包

```bash
sudo apt-get install language-pack-zh-hans
```

---

# 2.修改配置文件

> 注：下列操作在 docker 的 ubuntu 容器中不需要加 sudo，默认就是 root 权限。

## 2.1 方式一：修改 `~/.bashrc` 文件

> 如果只会用到终端环境的话，推荐使用这种方法。

修改 `~/.bashrc` 文件：

```bash
nano ~/.bashrc
```

添加：

```bash
export LANG=zh_CN.UTF-8
export LANGUAGE=zh_CN:zh
export LC_ALL=zh_CN.UTF-8
```

然后执行：

```bash
source ~/.bashrc
```

## 2.2 方式二：修改 `/etc/locale.gen` 文件

此方式是所有 Linux 操作系统同用的：

```bash
sudo nano /etc/locale.gen
```

找到以下行并取消注释（删除行前的 `#`）：

```bash
zh_CN.UTF-8 UTF-8
```

如果没有找到`zh_CN.UTF-8`，则手动添加这一行。

保存文件后，运行 `locale-gen` 命令来生成新的语言环境：

```bash
sudo locale-gen
```

然后重启系统即可。

---

# 3.中文空格乱码解决

这是缺少相应字体的原因，按住字体就行：

```bash
sudo apt-get install fonts-droid-fallback ttf-wqy-zenhei ttf-wqy-microhei fonts-arphic-ukai fonts-arphic-uming
```