---
title: VirtualBox 实现主机与虚拟机（CachyOS）传文件
date: 2024-08-01 22:44:58
categories: "虚拟机"
---


# 1.VirtualBox 中实现主机与虚拟机之间传输文件

## 1.1 VirtualBox 中的设置

如图所示：

- 进入虚拟机的设置：
![图一](https://im.gurl.eu.org/file/b852b7e357c0db8eb549d.png)

- 创建主机挂载目录：
![图二](https://im.gurl.eu.org/file/a6030bd1051ef5e2c8ced.png)

## 1.2 虚拟机中的设置

在虚拟机中创建共享目录：
```bash
mkdir ~/share -p
```

将主机的那个共享目录挂载到`~/share`文件夹下：
```bash
sudo mount -t vboxsf CachyOS_share ~/share/
```
