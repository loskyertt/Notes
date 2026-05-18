---
title: Docker 配置 MySQL 教程
date: 2024-09-05 20:31:01
excerpt: "如何通过 Docker 配置 MySQL 服务"
categories: "Docker教程"
---

# 1.Docker-MySQL 配置

## 1.1 拉取镜像与创建容器

1. **拉取 MySQL 镜像：**
   通过运行 `docker pull mysql[:版本号]`，版本号可选，默认是latest。

2. **创建并运行 MySQL 容器（推荐）：**
   使用以下命令来运行 MySQL 容器。这里假设想在主机上绑定3306端口，并设置一个root用户的密码：

```bash
docker run \
--name some-mysql \
-e MYSQL_ROOT_PASSWORD=1234 \
-p 3306:3306 \
-v ~/docker_volume/mysql/data:/var/lib/mysql \
-v ~/docker_volume/mysql/init:/docker-entrypoint-initdb.d \
-v ~/docker_volume/mysql/conf:/etc/mysql/conf.d \
-d mysql:<版本号>
```

**以上命令的说明：**
- `--name some-mysql`：给容器指定一个名称，比如 `some-mysql`。
- `-e MYSQL_ROOT_PASSWORD=1234`：设置root用户的密码为 `1234`。
- `mysql:latest`：使用最新版本的 MySQL 镜像。
- `p`：设置端口，`3306:3306`中，前者是宿主机的端口，可以自定义，后者是映射到容器的端口。默认是`TCP`协议。
- `-v`：挂载容器里的文件（`~/docker_volume/mysql/data`为本地目录）。**注意：** 要先创建好对应的文件路径！！同时本地文件夹会把容器内的文件夹完全覆盖！

## 1.2 连接到容器

1. **连接到 MySQL 容器：**

进入到容器内部然后进行连接：
```bash
docker exec -it some-mysql bash

mysql -uroot -p
```

直接连接到容器：
```bash
docker exec -it some-mysql mysql -uroot -p
```
这会启动一个终端会话，并要求输入上面设置的root用户的密码 (`1234`)。

以上命令的说明：
1. **docker exec：**
- **含义：** 在一个已经运行的容器内执行命令。
- **用途：** 允许你在运行的容器中执行新的命令。

2. **-it：**
- **`-i` (interactive)：** 保持标准输入（stdin）打开，使得你可以与容器中的进程进行交互。
- **`-t` (tty)：** 分配一个伪终端，提供一个终端会话环境。这两个选项一起使用可以进入容器并交互。

3. **some-mysql：**
- **含义：** 容器的名称或 ID。

4. **mysql：**
- **含义：** 这是在容器内执行的命令。在这个例子中，是启动 MySQL 客户端。
- **用途：** 连接到 MySQL 数据库。

5. **-uroot：**
- **`-u`：** 指定 MySQL 客户端的用户名。
- **root：** MySQL 数据库的用户名。在这个例子中，使用的是 `root` 用户。

6. **`-p`：**
- **含义：** 提示输入 MySQL 用户的密码。
- **用途：** 在执行命令后，你会被提示输入 `root` 用户的密码。这个密码是在运行容器时通过 `MYSQL_ROOT_PASSWORD` 环境变量设置的。

以上命令的 `-p 3306:3306` 会将主机的3306端口映射到容器的3306端口，这样可以从主机上的MySQL客户端连接到容器中的MySQL服务。

## 1.3 更改容器信息

### 1.3.1 方法一：停止并删除现有容器

首先，停止并删除现有的 `mysql-test` 容器：
```bash
docker start mysql-test        # 运行
docker stop mysql-test        # 停止
docker rm mysql-test        # 删除
```
然后再重新创建`mysql-test`即可。

### 1.3.2 方法二：不同的容器名称

可以使用不同的名称来运行容器，这样就不需要删除现有的容器。例如：
```bash
docker run --name mysql-test-2 -e MYSQL_ROOT_PASSWORD=0403 -p 3307:3306 -d mysql:latest
```
这样会启动一个新容器，名称为 `mysql-test-2`，并绑定主机的3307端口到容器的3306端口。

## 1.4 查看容器信息

查看容器或镜像内部信息（如端口，ip地址，挂载卷等）：
```
docker inspect <容器名/容器ID/镜像名/镜像ID>
```