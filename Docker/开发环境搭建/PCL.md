---
title: 通过 Docker 搭建 PCL 环境
date: 2024-11-30 22:10:05
excerpt: "这篇教程是关于如何通过 Docker 的 Ubuntu 容器来搭建 C++ 的 PCL 环境。"
categories: "Docker教程"
---

# 1.在容器内部构建

> ~~个人推荐使用这种方式，用 `Dockerfile` 总会出奇奇怪怪的问题。~~

## 1.1 运行容器

```bash
docker run -it --name=pcl ubuntu:latest
```

## 1.2 初始准备

在宿主机中创建一个 `sources.list` 文件，里面添加下面内容：

```txt
# 默认注释了源码镜像以提高 apt update 速度，如有需要可自行取消注释
deb https://mirrors.tuna.tsinghua.edu.cn/ubuntu/ noble main restricted universe multiverse
# deb-src https://mirrors.tuna.tsinghua.edu.cn/ubuntu/ noble main restricted universe multiverse
deb https://mirrors.tuna.tsinghua.edu.cn/ubuntu/ noble-updates main restricted universe multiverse
# deb-src https://mirrors.tuna.tsinghua.edu.cn/ubuntu/ noble-updates main restricted universe multiverse
deb https://mirrors.tuna.tsinghua.edu.cn/ubuntu/ noble-backports main restricted universe multiverse
# deb-src https://mirrors.tuna.tsinghua.edu.cn/ubuntu/ noble-backports main restricted universe multiverse

# 以下安全更新软件源包含了官方源与镜像站配置，如有需要可自行修改注释切换
# deb http://security.ubuntu.com/ubuntu/ noble-security main restricted universe multiverse
# deb-src http://security.ubuntu.com/ubuntu/ noble-security main restricted universe multiverse

# 预发布软件源，不建议启用
# deb https://mirrors.tuna.tsinghua.edu.cn/ubuntu/ noble-proposed main restricted universe multiverse
# # deb-src https://mirrors.tuna.tsinghua.edu.cn/ubuntu/ noble-proposed main restricted universe multiverse
```

详细配置可以见：[Ubuntu 清华镜像源](https://mirrors.tuna.tsinghua.edu.cn/help/ubuntu/)

然后把该文件复制`pcl`到容器中：

```bash
docker cp sources.list pcl:/etc/apt
```

在容器内执行：

```bash
apt update
```

如果更新失败，大概率是会报找不到 `certificates` 这样的错误。直接安装好 `ca-certificates` 即可：

```bash
apt install ca-certificates
```

然后重新执行`apt update`即可更新成功。

最后再执行一遍：

```bash
apt upgrade
```

## 1.3 配置 PCL 和 C++ 环境

`PCL` 可以直接通过 Ubuntu 的软件包管理器安装：

```bash
sudo apt install -y libpcl-dev
```

这会自动安装 `PCL` 的核心库和常见的依赖项，如 `Eigen`、`FLANN` 和 `VTK`。

验证 `PCL` 是否正确安装：

```bash
pkg-config --modversion pcl_common
```

如果安装成功，会显示PCL的版本号，例如 `1.12.1`。

如果需要 `PCL` 的可视化模块（基于 `VTK`），可以确保安装以下依赖：

```bash
apt install libvtk7-dev
```

然后，使用 `pcl_visualization` 模块来实现点云的 3D 可视化。

安装其它工具：

```bash
apt install -y cmake clang clangd libomp-dev
```

如果不想 `clangd` 的语法提示出幺蛾子（避免报找不到 `omp.h` 的错误），最好把 `libomp-dev` 安装好。

安装 `x11` 服务（为了显示程序的 `ui`）：

```bash
apt install x11-apps
```

最后通过 VSCode 连接到容器内部即可。

## 1.4 libomp-dev 是什么？（补充，可以跳过）

`libomp-dev` 是 OpenMP 运行时库和开发文件的 Debian/Ubuntu 软件包。OpenMP（Open Multi-Processing）是一个用于共享内存并行编程的 API，主要用于 C、C++ 和 Fortran 语言。它允许程序员使用编译器指令和库函数来创建并行代码，充分利用多核处理器的计算能力。

`libomp-dev` 包含：
1. OpenMP 头文件（如 `omp.h`）
2. OpenMP 运行时库的开发文件
3. 编译 OpenMP 程序所需的工具和配置

安装这个包的主要目的是：
- 允许编译器（如 GCC、Clang）支持 OpenMP 并行编程
- 提供编译 OpenMP 程序所需的头文件和库
- 启用 `-fopenmp` 编译选项

典型的使用场景：

```cpp
#include <omp.h>

int main() {
    #pragma omp parallel
    {
        // 这段代码将并行执行
        printf("Hello from thread %d\n", omp_get_thread_num());
    }
    return 0;
}
```

编译这样的代码需要添加 `-fopenmp` 选项：

```bash
g++ -fopenmp your_file.cpp -o your_program
```

---

# 2.通过 Dockerfile 创建

还是先创建镜像源文件 `sources.list`。

然后创建 `Dockerfile`，在里面加入下面的内容：

```Dockerfile
FROM ubuntu:latest

# 镜像源方式
# COPY sources.list /etc/apt/sources.list
# RUN apt-get update && apt-get install -y ca-certificates && apt-get update && apt-get -yq upgrade

# 代理方式
ENV http_proxy=http://your-proxy-address:port/
ENV https_proxy=http://your-proxy-address:port/
RUN apt-get update && apt-get -yq upgrade

RUN apt-get -yq install \
    libpcl-dev \
    cmake \
    clang \
    clangd \
    libomp-dev \
    x11-apps \
    language-pack-zh-hans \
    nano && \
    mkdir /root/workspace && \
    touch /var/lib/locales/supported.d/local

RUN echo 'LANG="zh_CN.UTF-8"' >> /etc/environment && \
    echo 'LANGUAGE="zh_CN:zh:en_US:en"' >> /etc/environment && \
    echo 'en_US.UTF-8 UTF-8' >> /var/lib/locales/supported.d/local && \
    echo 'zh_CN.UTF-8 UTF-8' >> /var/lib/locales/supported.d/local && \
    echo 'zh_CN.GBK GBK' >> /var/lib/locales/supported.d/local && \
    echo 'zh_CN GB2312' >> /var/lib/locales/supported.d/local && \
    locale-gen zh_CN.UTF-8

# 设置工作目录
WORKDIR /root/workspace
```

关于注释部分`ENV`说明：如果用镜像源不成功，就把设置镜像源那一行注释掉，然后设置代理来更新下载。`your-proxy-address`是宿主机的`IP`地址。

构建镜像：

```bash
docker build --no-cache -t ubuntu-pcl:latest .
```

> 如果在 `/etc/apt/sources.list` 中更改了 APT 源列表，而没有使用 `--no-cache`，Docker 可能会使用旧的缓存层，导致源列表的更改不会反映在最终的镜像中。当使用 `--no-cache` 时，Docker 会忽略缓存，确保每个命令都重新执行，特别是在修改了基础文件（如 `/etc/apt/sources.list` ）时，确保镜像构建时使用的是新的 APT 源列表。