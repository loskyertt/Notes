---
title: Linux 运维总览与排障速查
date: 2026-06-15
tags:
  - Linux
  - 运维
  - DevOps
  - 排障
  - 速查
aliases:
  - 运维速查
  - Linux 运维
  - 服务器排障
---

# 1. 笔记定位

这篇笔记是 Linux 运维教程目录的**入口页与排障速查**，用于快速定位主题、建立排障顺序、记录高危操作边界。

更细的基础命令请进入专题笔记：

| 主题 | 笔记 |
|---|---|
| 用户、用户组、sudo | [[01-Linux 用户与用户组管理]] |
| 文件查看、复制、删除、查找 | [[02-Linux 文件与目录基本操作]] |
| 文件权限、所有权、umask | [[03-Linux 文件权限与所有权]] |
| grep、正则、日志过滤 | [[04-Linux grep 文本搜索]] |
| tar、gzip、zip | [[05-Linux 文件打包与压缩]] |
| systemd、systemctl、journalctl | [[06-Linux systemd 与 systemctl 服务管理]] |

> [!summary]
> 运维的核心不是“记住所有命令”，而是按正确顺序缩小问题范围：**变更 → 状态 → 日志 → 资源 → 依赖 → 网络 → 回滚**。

---

# 2. 运维基本原则

| 原则 | 含义 | 落地动作 |
|---|---|---|
| 先观察后操作 | 避免重启或删除破坏现场 | 先看状态、日志、指标、变更 |
| 最小权限 | 降低误操作破坏范围 | 普通用户登录，需要时 `sudo` |
| 可回滚 | 出问题能恢复 | 修改前备份，发布前准备回滚 |
| 可复现 | 避免依赖手工记忆 | 沉淀脚本、Runbook、CI/CD |
| 保留证据 | 便于复盘和定位根因 | 保存日志、堆栈、指标、慢查询 |
| 区分环境 | 防止误操作生产 | 账号、域名、数据库、密钥隔离 |

> [!warning]
> 生产环境中，重启、删除、清库、改防火墙、改磁盘分区都属于高风险动作。执行前确认目标、影响范围、**备份**和回滚方案。

---

# 3. 常用目录速查

| 路径 | 用途 |
|---|---|
| `/etc` | 系统与服务配置 |
| `/var/log` | 系统与服务日志 |
| `/var/lib` | 服务持久化数据 |
| `/home` | 普通用户家目录 |
| `/root` | root 家目录 |
| `/opt` | 第三方应用或业务程序 |
| `/usr/local` | 本机手动安装的软件 |
| `/tmp` | 临时文件，通常会被清理 |
| `/proc` | 内核和进程运行时信息 |
| `/sys` | 内核设备模型信息 |
| `/dev` | 设备文件 |
| `/run` | 运行时状态文件 |

> [!tip]
> 记忆：`/etc` 放配置，`/var` 放变化数据，`/opt` 放业务或第三方应用，`/proc` 和 `/sys` 是运行时视图。

---

# 4. 基础状态检查

## 4.1 主机状态

| 命令 | 功能 | 核心关注点 |
| :--- | :--- | :--- |
| `hostnamectl` | 看机器名字和系统版本 | 我在哪台机器上？什么系统？ |
| `uptime` | 看机器活了多久和忙不忙 | 机器重启了吗？CPU 负载高不高？ |
| `date` | 看当前时间 | 现在几点了？ |
| `timedatectl` | 看时区和时间同步状态 | 时区对不对？NTP 同步开了没？ |
| `who` | 看现在谁在线 | 当前有几个用户登录？ |
| `w` | 看在线的人在干嘛 | 在线用户分别在敲什么命令？ |
| `last` | 看过去谁来过 | 历史上谁从哪个 IP 登录过？ |

## 4.2 资源状态

### 4.2.1 top

`top` 是 Linux 最常用的实时系统监控工具之一，可以查看 CPU、内存、进程等信息。

1. 顶部信息解释，例如：

```text
top - 10:30:12 up 2 days,  3:15,  1 user,  load average: 0.25, 0.30, 0.28
Tasks: 235 total,   1 running, 234 sleeping
%Cpu(s):  2.1 us,  0.5 sy,  0.0 ni, 97.0 id
MiB Mem : 15924 total,  4321 free,  5821 used,  5782 buff/cache
MiB Swap:  4096 total,  4096 free,     0 used
```

|字段|含义|
|---|---|
|load average|系统负载（1/5/15 分钟）|
|us|用户态 CPU|
|sy|内核态 CPU|
|id|CPU 空闲率|
|Mem|内存|
|Swap|交换分区|

2. 进程列表常用列：

|列|含义|
|---|---|
|PID|进程 ID|
|USER|用户|
|PR|优先级|
|NI|nice 值|
|VIRT|虚拟内存|
|RES|实际物理内存|
|SHR|共享内存|
|S|状态|
|%CPU|CPU 占用|
|%MEM|内存占用|
|TIME+|累计 CPU 时间|
|COMMAND|命令名|

3. **`top` 运行时交互快捷键**

在 `top` 运行界面中按下以下按键实现对应功能：

| 快捷键 | 功能说明 | 备注 / 操作示例 |
| :--- | :--- | :--- |
| **`Shift + P`** (大写 P) | 按 CPU 使用率排序 | 按 `%CPU` 从高到低排序 |
| **`Shift + M`** (大写 M) | 按内存使用率排序 | 按 `%MEM` 排序，查内存泄漏时常用 |
| **`Shift + T`** (大写 T) | 按运行时间排序 | 按 `TIME+` 排序 |
| **`Shift + R`** (大写 R) | 反转排序 | 实现升序/降序切换 |
| **`H`** | 进程/线程视图切换 | 进程视图 ↔ 线程视图。调试 Java, MySQL, Redis 等多线程程序很有用 |
| **`u`** | 查看特定用户 | 按下后输入用户名（如 `sky`），只显示该用户进程 |
| **`L`** | 搜索进程 | 按下后输入关键字搜索 PID |
| **`k`** | 杀死进程 | 按下后输入 `PID`，再输入信号（如 `15` 表示 SIGTERM，`9` 表示 SIGKILL）。等价于 `kill PID` |
| **`r`** | 修改 Nice 值 | 按下后输入 `PID`，再输入优先级（如 `-5` 或 `10`）。等价于 `renice` |
| **`c`** | 切换命令行显示 | COMMAND 列在程序名与完整参数间切换。<br>例：`python` ↔ `python train.py --batch-size 64 --lr 0.001` |
| **`V`** | 树状显示进程 | 显示父子进程树状关系，类似 `pstree`（部分版本支持） |
| **`i`** | 仅显示活跃进程 | 隐藏 `%CPU = 0` 的空闲进程 |
| **`d`** | 修改刷新间隔 | 按下后输入数字（如 `1` 表示 1 秒刷新一次，默认通常为 3 秒） |
| **`f`** | 添加/删除显示列 | 进入字段配置界面，可勾选显示 PPID, UID, WCHAN, CGROUP 等字段 |

4. **`top` 常用启动参数**

在终端中输入 `top` 命令时直接附加的参数：

| 启动命令 | 功能说明 | 示例 |
| :--- | :--- | :--- |
| **`-p <PID>`** | 查看单个指定进程 | `top -p 1234` |
| **`-p <PID1,PID2>`**| 查看多个指定进程 | `top -p 1234,5678` |
| **`-d <秒数>`** | 指定刷新时间间隔 | `top -d 1` （1 秒刷新一次） |
| **`-u <用户名>`** | 仅查看指定用户的进程 | `top -u sky` |

### 4.2.2 其它

| 命令 | 功能 | 典型使用场景 |
| :--- | :--- | :--- |
| `free -h` | 看内存还剩多少 | 确认内存有没有爆 |
| `vmstat 1` | 每秒看一次系统状态趋势 | 排查是 CPU 瓶颈还是磁盘 IO 瓶颈 |
| `df -h` | 看磁盘空间剩多少 | 报错“磁盘满”时，先看这个确认 |
| `df -i` | 看文件数量(inode)上限 | 磁盘没满但报错，排查是不是小文件太多 |
| `du -sh /var/*` | 看某个目录占了多大空间 | 磁盘满了，顺藤摸瓜找大文件在哪 |
| `ps aux --sort=-%cpu \| head` | 找最吃 CPU 的前 10 个进程 | CPU 飙升到 100%，抓元凶 |
| `ps aux --sort=-%mem \| head` | 找最吃内存的前 10 个进程 | 内存快满了，抓元凶 |
| `pgrep -af nginx` | 找指定程序的进程号 | 确认 Nginx 是否在跑，PID 是多少 |

## 4.3 网络状态

| 命令 | 一句话总结 | 典型使用场景 |
| :--- | :--- | :--- |
| `ip addr` | 看本机有哪些网卡和 IP | 确认服务器 IP 配好没有 |
| `ip route` | 看网关和路由表怎么走 | 有 IP 但连不上外网，查网关 |
| `ss -tulpen` | 看哪些端口在提供服务 | 确认 Nginx 到底在没在 80 端口蹲守 |
| `ss -antp` | 看当前的 TCP 连接情况 | 查看有没有客户端连进来，有没有僵尸连接 |
| `dig` | 查域名解析到的 IP | 排查域名配没配对 |
| `curl -v` | 模拟浏览器发 HTTP 请求 | 排查网站打不开，看状态码或证书是否报错 |

---

# 5. 服务排障流程

## 5.1 通用流程

1. 确认最近是否有发布、配置、扩容、证书、网络、安全组或数据库变更。
2. 查看服务状态：`systemctl status <service>`。
3. 查看服务最近 100 行的日志：`journalctl -u <service> -n 100`。
4. 查看端口监听：`ss -tulpen | grep <port>`。
5. 查看进程资源：`top`、`ps`、`pidstat`。
6. 查看依赖状态：数据库、Redis、消息队列、外部 HTTP/RPC。
7. 判断是否需要 reload、restart、扩容、回滚或降级。

```bash
systemctl status myapp
journalctl -u myapp -n 100
journalctl -u myapp -f  # 该命令类似于 tail -f <日志文件名>
ss -tulpen | grep 8080
curl -v http://127.0.0.1:8080/health
```

> [!summary]
> 服务不可用时，不要第一反应重启。先判断“进程是否在、端口是否听、日志报什么、依赖是否通、最近改过什么”。

## 5.2 HTTP 状态码定位

| 状态码 | 常见含义 | 检查方向 |
|---:|---|---|
| 400 | 请求格式错误 | 参数、Header、JSON 格式 |
| 401 | 未认证 | Token、Session、登录态 |
| 403 | 无权限 | 鉴权、文件权限、Nginx deny |
| 404 | 资源不存在 | 路由、路径、Nginx root/alias |
| 413 | 请求体过大 | `client_max_body_size` |
| 499 | 客户端断开 | 客户端超时、网络、服务慢 |
| 500 | 应用内部错误 | 应用日志、异常堆栈 |
| 502 | 网关错误 | upstream 进程、端口、协议 |
| 503 | 服务不可用 | 限流、维护、upstream 不可用 |
| 504 | 网关超时 | upstream 慢、超时配置 |

---

# 6. 日志排查速查

```bash
# systemd 服务日志
journalctl -u myapp -n 100
journalctl -u myapp -f
journalctl -u myapp --since "1 hour ago"

# 系统错误
journalctl -p err -b

# 应用日志关键词
grep -iE "error|exception|panic|fatal|timeout|refused" app.log

# 查看上下文
grep -iC 3 "exception" app.log

# gzip 历史日志
zgrep -i "error" app.log.1.gz

# Nginx 状态码分布
awk '{print $9}' access.log | sort | uniq -c | sort -nr

# 访问最多的 IP
awk '{print $1}' access.log | sort | uniq -c | sort -nr | head
```

> [!tip]
> 日志排查优先关注：错误时间、请求 ID、错误级别、堆栈、上游依赖、重试、超时、状态码和最近变更。

---

# 7. 网络排查路线

## 7.1 四步法

1. **DNS**：域名是否解析正确。
2. **连通性**：IP 是否可达。
3. **端口**：服务是否监听，防火墙是否放行。
4. **协议**：HTTP/TLS/应用协议是否正常。

```bash
dig example.com
ping 8.8.8.8
mtr example.com
nc -vz example.com 443
curl -Iv https://example.com
openssl s_client -connect example.com:443 -servername example.com
```

## 7.2 常见现象

| 现象 | 可能原因 | 检查命令 |
|---|---|---|
| 域名不通 | DNS 配置错误 | `dig`、`/etc/resolv.conf` |
| IP 通端口不通 | 服务未监听、防火墙、安全组 | `ss`、`nc`、`iptables` |
| 本机通外部不通 | 监听 `127.0.0.1`、防火墙 | `ss -tulpen` |
| HTTPS 异常 | 证书过期、SNI、链不完整 | `openssl s_client` |
| 请求慢 | DNS、网络、TLS、服务端慢 | `curl -w`、日志、指标 |

---

# 8. 磁盘与容量排查

```bash
# 文件系统容量
df -h

# inode
df -i

# 查大目录
du -xh /var | sort -h | tail -n 30

# 查大文件
find /var -type f -size +1G -ls

# 已删除但仍被进程占用的文件
lsof | grep deleted
```

> [!warning]
> `df` 显示磁盘满但 `du` 找不到大文件时，常见原因是文件已删除但进程仍持有句柄。用 `lsof | grep deleted` 定位后，重启对应进程释放空间。

---

# 9. CPU 与内存排查

## 9.1 CPU 高

```bash
top
ps -eo pid,ppid,cmd,%cpu,%mem --sort=-%cpu | head
pidstat -u -p <PID> 1
```

常见方向：

- 热点接口或突增流量。
- 死循环、正则回溯、序列化、压缩、加密。
- 线程池打满。
- 日志过量。
- GC 频繁。

## 9.2 内存高或 OOM

```bash
free -h
ps aux --sort=-%mem | head
dmesg -T | grep -i oom
cat /proc/<PID>/status | grep -E 'VmRSS|VmSize|Threads'
```

常见方向：

- 内存泄漏。
- 缓存无上限。
- 大文件一次性加载。
- 大查询或大结果集。
- 连接泄漏。
- 容器内存限制过小。

---

# 10. 数据库与缓存速查

## 10.1 MySQL / MariaDB

```sql
SHOW PROCESSLIST;
SHOW ENGINE INNODB STATUS\G
EXPLAIN SELECT * FROM table_name WHERE id = 1;
SELECT * FROM information_schema.INNODB_TRX\G
```

重点关注：

- 慢查询。
- 锁等待。
- 连接数是否打满。
- 主从延迟。
- 大事务和长事务。

## 10.2 Redis

```bash
redis-cli -h host -p 6379
INFO
INFO memory
INFO replication
CLIENT LIST
SLOWLOG GET 10
SCAN 0 MATCH prefix:* COUNT 100
```

> [!warning]
> 生产环境避免执行 `KEYS *`，它可能阻塞 Redis。优先使用 `SCAN` 分批扫描。

---

# 11. 发布与变更检查

发布或配置变更前确认：

- 当前版本和目标版本。
- 配置差异和敏感配置。
- 数据库变更是否可回滚。
- 依赖服务是否兼容。
- 健康检查是否可用。
- 监控和告警是否正常。
- 回滚命令或回滚镜像是否准备好。

发布后检查：

```bash
systemctl status myapp
journalctl -u myapp -n 100
curl -f http://127.0.0.1:8080/health
ss -tulpen | grep 8080
```

---

# 12. 高危命令清单

> [!danger]
> 以下命令可能造成数据丢失、服务中断或远程失联。执行前必须确认环境、目标、备份、回滚和维护窗口。

```bash
rm -rf /
rm -rf /path
dd if=/dev/zero of=/dev/sdX
mkfs.ext4 /dev/sdX
fdisk /dev/sdX
iptables -F
ufw enable
docker system prune -a --volumes
kubectl delete ns namespace
kubectl delete -f app.yaml
```

```sql
DROP DATABASE db;
TRUNCATE table;
DELETE FROM table;
```

```redis
FLUSHDB
FLUSHALL
KEYS *
```

---

# 13. 事故复盘模板

## 13.1 基本信息

- 事故时间：
- 影响范围：
- 影响时长：
- 严重等级：
- 发现方式：
- 负责人：

## 13.2 时间线

| 时间 | 事件 |
|---|---|
|  |  |

## 13.3 根因分析

- 直接原因：
- 深层原因：
- 为什么监控没有提前发现：
- 为什么自动化没有阻止：
- 为什么流程没有覆盖：

## 13.4 处理过程

- 临时止血措施：
- 最终修复措施：
- 回滚或补偿动作：
- 验证方式：

## 13.5 改进项

- [ ] 增加监控或告警
- [ ] 增加自动化测试
- [ ] 优化发布流程
- [ ] 完善运行手册
- [ ] 修复代码或配置缺陷

> [!summary]
> 复盘重点是改进系统和流程，而不是追责个人。每个改进项都应有负责人、截止时间和验证方式。

---

# 14. 高频命令索引

| 场景 | 命令 |
|---|---|
| 当前目录 | `pwd` |
| 查看文件 | `less file` |
| 跟踪日志 | `tail -F app.log` |
| 查找文本 | `grep -R "keyword" .` |
| 查找文件 | `find /path -name "*.log"` |
| 磁盘容量 | `df -h` |
| 目录大小 | `du -sh *` |
| 内存 | `free -h` |
| CPU/进程 | `top` |
| 端口 | `ss -tulpen` |
| 服务状态 | `systemctl status service` |
| systemd 日志 | `journalctl -u service -f` |
| 网络测试 | `curl -v url` |
| DNS | `dig domain` |
| SSH | `ssh user@host` |
| 同步文件 | `rsync -av src/ dst/` |

---

# 15. 学习路线

1. 先掌握终端快捷键、历史命令和 tmux，会“高效且安全地操作命令行”。
2. 再学习用户、文件操作、文件权限，建立 Linux 基础操作模型。
3. 继续学习 grep、压缩备份和 Vim，覆盖日常查找、归档和编辑。
4. 然后学习 systemd、journalctl 与服务管理，进入真正的服务运维。
5. 最后结合网络、日志、磁盘、CPU、内存排障路径沉淀 Runbook 和自动化流程。
