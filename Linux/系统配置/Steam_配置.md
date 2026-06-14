---
title: Linux 下给 Steam 设置代理
date: 2024-07-24 15:17:15
tags:
  - linux/系统配置
  - steam
  - 代理
---

# 1. 设置系统代理

1. 打开 Steam ==大屏模式==（一定是大屏模式！）。

![大屏模式](https://nyalog.oss-cn-hongkong.aliyuncs.com/NyaLog/articles/Steam%20Linux%20%E8%AE%BE%E7%BD%AE%E4%BB%A3%E7%90%86%E7%9A%84%E5%B0%8F%E6%8A%80%E5%B7%A7/Steam-BigScreen-min.png)

2. 在设置里进入网络设置界面，填入自己的代理地址和端口。

![设置界面](https://nyalog.oss-cn-hongkong.aliyuncs.com/NyaLog/articles/Steam%20Linux%20%E8%AE%BE%E7%BD%AE%E4%BB%A3%E7%90%86%E7%9A%84%E5%B0%8F%E6%8A%80%E5%B7%A7/Steam-proxy-min.png)

3. 设置成功后退出重启 Steam 即可。

---

# 2. TUN 模式（推荐）

> [!tip] 推荐
> 建议直接开启代理软件的 TUN 模式，自动接管所有流量，无需单独配置。

> [!note] 相关笔记
> 终端代理配置参见 [[zsh_配置#1. 终端代理设置]]，Docker 代理配置参见 [[Docker_配置#1.3 代理配置]]。
