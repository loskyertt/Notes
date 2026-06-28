---
title: Linux 用户与用户组管理
date: 2026-06-15
tags:
  - Linux
  - 运维
  - 用户管理
  - 权限
aliases:
  - Linux 用户操作
  - 用户与用户组管理
---

# 1. 核心概念

Linux 使用 **用户（user）**、**用户组（group）** 和 **权限（permission）** 控制系统资源访问。

| 对象 | 作用 | 常见配置文件 |
|---|---|---|
| 用户 | 标识一个登录或运行进程的身份 | `/etc/passwd`、`/etc/shadow` |
| 用户组 | 将多个用户组织起来统一授权 | `/etc/group`、`/etc/gshadow` |
| root | UID 为 `0` 的超级用户 | 全系统最高权限 |
| sudo | 授权普通用户临时执行特权命令 | `/etc/sudoers` |

> [!summary]
> 日常运维优先使用普通用户登录，需要特权时用 `sudo` 执行单条命令，避免长期停留在 root shell。

---

# 2. 用户身份切换

## 2.1 `su` 命令

`su` 用于切换到其他用户身份，默认目标用户是 `root`，需要输入**目标用户密码**。

```bash
# 切换到 root，并加载 root 的完整登录环境
su -

# 切换到 root，但保留当前环境变量
su

# 切换到指定用户，并加载其登录环境
su - <username>
```

| 命令 | 环境变量 | 工作目录 | 适用场景 |
|---|---|---|---|
| `su` | 保留当前用户环境 | 不变 | 临时切换身份，不推荐长期使用 |
| `su -` | 加载目标用户登录环境 | 目标用户 `HOME` | 模拟目标用户完整登录 |

> [!warning]
> 绝大多数场景应使用 `su -`，否则 `PATH`、`HOME` 等环境变量可能仍来自原用户，导致命令查找或配置读取异常。

## 2.2 `sudo` 命令

`sudo` 允许已授权用户以 `root` 或其他用户身份执行命令，通常输入的是**当前用户自己的密码**。

```bash
# 以 root 身份执行单条命令
sudo <command>

# 示例：查看 root 目录
sudo ls /root

# 进入 root 登录环境
sudo -i

# 以指定用户身份执行命令
sudo -u <username> <command>
```

## 2.3 常见切换方式对比

| 命令 | 输入密码 | 结果 | 适用场景 |
|---|---|---|---|
| `su -` | root 密码 | 进入 root 登录环境 | 知道 root 密码 |
| `sudo <command>` | 当前用户密码 | 执行单条特权命令 | 最推荐的日常方式 |
| `sudo -i` | 当前用户密码 | 进入 root 登录环境 | Ubuntu 默认 root 被锁定 |
| `sudo su -` | 当前用户密码 | 通过 sudo 再进入 root 环境 | 兼容旧习惯或旧系统 |

> [!tip]
> Ubuntu / Debian 安装后常默认锁定 root 密码，此时不能直接 `su -`，应使用 `sudo -i` 或 `sudo su -`。

# 3. 用户信息查看

```bash
# 当前用户名
whoami

# 当前用户 UID、GID 与所属组
id

# 指定用户信息
id <username>

# 当前登录用户
who
w

# 最近登录记录
last

# 所有用户记录
cat /etc/passwd

# 查看密码过期策略
sudo chage -l <username>
```

`/etc/passwd` 每行格式：

```text
用户名:密码占位符:UID:GID:注释:主目录:登录Shell
```

> [!info]
> `/etc/passwd` 中的 `x` 不是明文密码，真实密码哈希保存在权限更严格的 `/etc/shadow`。

# 4. 用户创建与修改

## 4.1 创建用户

```bash
# 创建用户并生成主目录
sudo useradd -m <username>

# 指定登录 Shell
sudo useradd -m -s /bin/bash <username>

# 指定主目录
sudo useradd -m -d /home/custom_dir <username>

# 指定 UID
sudo useradd -m -u 1100 <username>

# 设置密码
sudo passwd <username>
```

| 选项 | 说明 |
|---|---|
| `-m` | 创建主目录 |
| `-s <shell>` | 指定登录 Shell |
| `-d <dir>` | 指定主目录路径 |
| `-u <uid>` | 指定 UID |
| `-g <group>` | 指定主组 |
| `-G <groups>` | 指定附加组，多个组用逗号分隔 |
| `-c <comment>` | 添加备注 |

## 4.2 修改用户

```bash
# 修改用户名
sudo usermod -l <new_username> <old_username>

# 修改主目录，并移动原目录内容
sudo usermod -d /new/home/path -m <username>

# 修改登录 Shell
sudo usermod -s /bin/zsh <username>

# 修改备注
sudo usermod -c "Full Name" <username>

# 锁定账号
sudo usermod -L <username>

# 解锁账号
sudo usermod -U <username>
```

## 4.3 删除用户

```bash
# 删除用户，保留主目录
sudo userdel <username>

# 删除用户及主目录
sudo userdel -r <username>
```

> [!warning]
> `userdel -r` 会删除用户主目录和邮件目录。执行前确认数据已备份，尤其不要误删仍在运行服务使用的账号。

# 5. 用户组管理

## 5.1 查看组信息

```bash
# 当前用户所属组
groups

# 指定用户所属组
groups <username>

# 所有组记录
cat /etc/group
```

`/etc/group` 每行格式：

```text
组名:密码占位符:GID:成员列表
```

## 5.2 创建、修改与删除组

```bash
# 创建组
sudo groupadd <groupname>

# 创建组并指定 GID
sudo groupadd -g 1100 <groupname>

# 修改组名
sudo groupmod -n <new_groupname> <old_groupname>

# 删除组
sudo groupdel <groupname>
```

## 5.3 管理组成员

```bash
# 将用户追加到附加组
sudo usermod -aG <groupname> <username>

# Debian / Ubuntu 授予 sudo 权限
sudo usermod -aG sudo <username>

# RHEL / CentOS / Fedora / Arch 授予 sudo 类权限
sudo usermod -aG wheel <username>

# 从某个组移除用户
sudo gpasswd -d <username> <groupname>
```

> [!warning]
> `usermod -aG` 中的 `-a` 不能省略。`usermod -G <group> <user>` 会覆盖用户原有附加组，可能导致用户失去 sudo、docker 等权限。

# 6. sudo 授权

## 6.1 编辑 sudoers

```bash
sudo visudo
```

`visudo` 会在保存时检查语法，避免配置错误导致所有用户无法提权。

## 6.2 常见授权示例

```sudoers
# 允许用户执行所有命令
alice ALL=(ALL) ALL

# 允许 deploy 用户无密码重启指定服务
deploy ALL=(ALL) NOPASSWD: /bin/systemctl restart myapp.service

# 允许 sudo 组成员执行所有命令
%sudo ALL=(ALL:ALL) ALL
```

> [!warning]
> `NOPASSWD` 应只授予明确命令，不要对 `ALL` 滥用。生产环境要保留审计记录，并定期清理离职人员账号和密钥。

# 7. 常用操作速查

| 场景 | 命令 |
|---|---|
| 查看当前身份 | `whoami` |
| 查看 UID/GID/组 | `id` |
| 创建用户 | `sudo useradd -m -s /bin/bash user` |
| 设置密码 | `sudo passwd user` |
| 添加 sudo 权限 | `sudo usermod -aG sudo user` |
| 锁定账号 | `sudo usermod -L user` |
| 删除用户并保留家目录 | `sudo userdel user` |
| 删除用户及家目录 | `sudo userdel -r user` |
| 切换到 root | `sudo -i` |
| 以其他用户执行命令 | `sudo -u user command` |

# 8. 易错点总结

> [!summary]
> 本篇核心：用户决定“谁在操作”，用户组决定“如何批量授权”，`sudo` 决定“普通用户如何临时提权”。

- `su -` 与 `su` 的关键差异是是否加载目标用户登录环境。
- Ubuntu 默认 root 账号通常被锁定，优先使用 `sudo -i`。
- `useradd` 创建用户后通常还需要 `passwd` 设置密码。
- `usermod -aG` 必须保留 `-a`，否则会覆盖附加组。
- sudoers 应使用 `visudo` 修改，不要直接编辑文件。
