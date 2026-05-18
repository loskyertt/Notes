---
title: 通过Docker搭建Hexo博客
date: 2024-07-24 10:57:40
excerpt: "通过 Docker 搭建 Hexo 博客挺方便的，主要是之前在 Windows 电脑上搭建过一个博客项目，但是环境是直接部署在本地，导致后来重装系统把配置文件全弄丢了。如果用 Docker 搭建，可以把配置环境和文件等放到镜像里存储到 Docker Hub 或者打包备份到本地，就不用担心弄丢。"
categories: "Docker教程"
---


# 1.创建项目文件夹

创建博客的工作

```bash
mkdir -p ~/Hexo && cd ~/Hexo

# 创建存放 Dockerfile 的文件夹
mkdir hexo_docker && cd hexo_docker

# 创建 Dockerfile
touch Dockerfile
```

---

# 2.配置 Dockerfile

```bash
# 基础镜像
FROM node:latest

# 维护者信息
MAINTAINER yourname<youremail@gmail.com>

# 工作目录
WORKDIR /hexo

# 设置 npm 使用淘宝镜像源
RUN npm config set registry https://registry.npmmirror.com

# 安装 Hexo
RUN npm install hexo-cli -g
RUN hexo init blog
RUN cd blog
RUN npm install

# 设置git
RUN git config --global user.name "loskyertt"
RUN git config --global user.email "loskyertt0403@gmail.com"

# 映射端口
EXPOSE 4000

# 运行命令
CMD ["/bin/bash"]
```

然后构建镜像（和 Dockerfile 同目录下）：

```bash
docker build -t hexo:latest .
```

Docker 的 BuildKit 可以加速构建过程并启用更多的优化选项，在构建镜像时启用 BuildKit：

```bash
DOCKER_BUILDKIT=1 docker build -t hexo:latest .
```

---

# 3.更新镜像

要在已经构建的基础镜像更新镜像，可以使用以下两种方法：

## 3.1 方法一：更新现有 Dockerfile 并重建镜像

更新`Dockerfile`，然后重新构建镜像。

## 3.2 方法二：从已有镜像启动容器并手动添加 Hexo

1. **启动一个容器**

从已有的基础镜像启动一个交互式容器：
```bash
docker run -it hexo:latest /bin/bash
```

2. **在容器内进行操作**

3. **提交容器为新镜像**

退出容器（`exit` 命令），然后提交容器为新镜像：

```bash
docker commit <container_id> hexo:with-hexo
```

`with-hexo`是镜像标签，可以自定义。这里的 `<container_id>` 是刚刚启动的容器的 ID。可以使用 `docker ps -a` 命令找到它。
**注意：** 如果要把挂载的宿主机的文件提交到镜像里，需要提前做一个`cp`操作，即通过`docker cp`把宿主计内的文件复制到容器内，然后再提交。

---

# 4.推送与备份镜像

## 4.1 推送镜像到 Docker Hub

1. **登录 Docker Hub**

```bash
docker login
```

2. **标记镜像**

将镜像标记为你的 Docker Hub 存储库。例如，将 `hexo:latest` 标记为 `yourusername/hexo:latest`：

```bash
docker tag hexo:latest yourusername/hexo:latest
```

3. **推送镜像**

```bash
 docker push yourusername/hexo:latest
```

## 4.1 备份镜像到本地

1. **保存镜像**

使用 `docker save` 命令将镜像保存到一个 tar 文件。例如，将 `hexo:latest` 保存到 `hexo_latest.tar`：

```bash
docker save -o hexo_latest.tar hexo:latest
```

2. **加载镜像**

以后你可以使用 `docker load` 命令从 tar 文件加载镜像。例如，从 `hexo_latest.tar` 加载镜像：

```bash
docker load -i hexo_latest.tar
```

---

# 5.挂载容器文件的注意项

在构建镜像时，已经生成了 `/hexo/blog` 目录，如果直接执行：

```bash
docker run -it --name="my-blog" -p 4000:4000 -v ~/Hexo/hexo:/hexo hexo:latest /bin/bash
```

这回在主机的`~/Hexo/hexo`目录和容器内的`/hexo`目录之间创建一个卷映射。这意味着容器内的`/hexo`目录会被主机上的`~/Hexo`目录的内容覆盖。如果主机上的`~/Hexo`目录是空的或不存在，那么容器内的`/hexo`目录也会是空的。

## 5.1 解决方法

### 5.1.1 方式一：确保主机目录包含内容

在主机上确保 `~/Hexo` 目录存在并包含`Hexo`项目文件。如果目录不存在或为空，可以先在主机上初始化`hexo`项目：

```bash
mkdir -p ~/Hexo/hexo
cd ~/Hexo/hexo
npm install hexo-cli -g
hexo init .
cd blog
npm install
```

### 5.1.2 方式二：在容器内初始化 Hexo 项目

如果希望在容器内初始化 Hexo 项目而不是依赖主机目录，可以先启动一个临时容器，初始化项目，然后将其复制到主机目录：

```bash
docker run -it hexo:latest /bin/bash
```

在容器内执行以下命令：

```bash
npm install hexo-cli -g
hexo init /hexo
cd /hexo
npm install
```

退出容器（`exit`），然后将容器内的 `/hexo` 目录复制到主机：

```bash
docker cp <container_id>:/hexo ~/Hexo
```

这里的 `<container_id>` 是你刚刚启动的容器的 ID。可以使用 `docker ps -a` 命令找到它。

---

# 6.最终步骤

确保主机上的 `~/Hexo` 目录存在并包含 Hexo 项目文件，然后再次运行 Docker 容器：

```bash
docker run -it --name="my-blog" -p 4000:4000 -v ~/Hexo/hexo:/hexo hexo:latest /bin/bash
```

这时，容器内的 `/hexo` 目录将包含主机上 `~/Hexo/hexo` 目录的内容。
