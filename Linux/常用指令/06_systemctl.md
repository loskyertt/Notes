# 1. systemd 概述

`systemd` 是目前绝大多数主流 Linux 发行版（Ubuntu、Debian、CentOS、Fedora、Arch 等）采用的 **init 系统和服务管理器**，是系统启动后运行的第一个进程（PID = 1），负责启动和管理所有后续的系统服务与进程。

`systemctl`（system control）是与 `systemd` 交互的主要命令行工具。

**systemd 的核心概念——Unit（单元）**：

`systemd` 将所有被管理的资源抽象为 Unit，每种 Unit 对应一类系统资源：

| Unit 类型 | 扩展名 | 说明 |
|---|---|---|
| Service | `.service` | 系统服务（最常用），如 `nginx.service` |
| Socket | `.socket` | 进程间通信套接字 |
| Target | `.target` | 一组 Unit 的集合，类似运行级别（runlevel） |
| Timer | `.timer` | 定时触发，可替代 crontab |
| Mount | `.mount` | 文件系统挂载点 |
| Path | `.path` | 监控文件系统路径变化 |
| Slice | `.slice` | 资源控制组（cgroup） |

Unit 文件存放位置：

| 路径 | 说明 |
|---|---|
| `/lib/systemd/system/` | 系统默认 Unit 文件（软件包安装时提供，不建议修改） |
| `/etc/systemd/system/` | 管理员自定义 Unit 文件，**优先级高于上面**，修改和新建应在此处 |
| `~/.config/systemd/user/` | 当前用户的 Unit 文件（用户级服务） |

---

# 2. 服务管理

## 2.1 查看服务状态

```bash
# 查看指定服务的状态（含运行状态、最近日志）
systemctl status nginx

# 检查服务是否正在运行（返回 0=运行中，非 0=未运行）
systemctl is-active nginx

# 检查服务是否已设置开机自启
systemctl is-enabled nginx

# 检查服务是否加载失败
systemctl is-failed nginx
```

`systemctl status` 输出字段说明：

```
● nginx.service - A high performance web server
     Loaded: loaded (/lib/systemd/system/nginx.service; enabled; ...)
              │                                            └── 开机自启状态
              └── Unit 文件路径
     Active: active (running) since ...
              └── active(running)=运行中 | inactive(dead)=已停止 | failed=失败
    Process: ...
   Main PID: 1234 (nginx)
      Tasks: 3
     CGroup: /system.slice/nginx.service
```

## 2.2 启动、停止与重启服务

```bash
# 启动服务
sudo systemctl start nginx

# 停止服务
sudo systemctl stop nginx

# 重启服务（先 stop 再 start）
sudo systemctl restart nginx

# 重新加载配置文件（不中断服务，不是所有服务都支持）
sudo systemctl reload nginx

# 若支持 reload 则 reload，否则自动 restart（推荐用于生产环境）
sudo systemctl reload-or-restart nginx
```

**`restart` 与 `reload` 的区别**：

| 操作 | 行为 | 对连接的影响 |
|---|---|---|
| `restart` | 停止进程再重新启动 | 会短暂中断现有连接 |
| `reload` | 向进程发送信号，令其重读配置文件 | 不中断现有连接（需服务支持） |

## 2.3 设置开机自启

```bash
# 启用开机自启（在 /etc/systemd/system/ 中创建符号链接）
sudo systemctl enable nginx

# 启用开机自启，并立即启动服务（--now 等价于 enable + start）
sudo systemctl enable --now nginx

# 禁用开机自启
sudo systemctl disable nginx

# 禁用开机自启，并立即停止服务
sudo systemctl disable --now nginx

# 屏蔽服务（彻底禁止，无法被手动启动或被其他服务触发）
sudo systemctl mask nginx

# 取消屏蔽
sudo systemctl unmask nginx
```

**`disable` 与 `mask` 的区别**：

| 操作 | 开机不自启 | 能否手动 start |
|---|---|---|
| `disable` | ✅ | ✅ 可以 |
| `mask` | ✅ | ❌ 完全禁止 |

## 2.4 重新加载 systemd 配置

当修改或新增了 Unit 文件后，需要通知 systemd 重新读取配置：

```bash
# 重新加载所有 Unit 文件（修改 .service 文件后必须执行）
sudo systemctl daemon-reload
```

---

# 3. 查看服务列表

## 3.1 查看运行中的服务

```bash
# 查看所有当前运行的 service 类型 Unit
systemctl list-units --type=service --state=running

# 查看所有已加载的 Unit（不论状态）
systemctl list-units --type=service

# 查看所有 Unit（包括未加载的），不加 --type 则显示全部类型
systemctl list-units --all
```

## 3.2 查看开机自启动服务

```bash
# 查看所有已启用（开机自启）的 service
systemctl list-unit-files --type=service --state=enabled

# 查看所有 service 及其启用状态
systemctl list-unit-files --type=service

# 查看所有已启用的 Unit（包含 service、socket、target 等）
systemctl list-unit-files --state=enabled
```

`list-unit-files` 输出的状态说明：

| 状态 | 含义 |
|---|---|
| `enabled` | 已设置开机自启 |
| `disabled` | 未设置开机自启 |
| `masked` | 已屏蔽，无法启动 |
| `static` | 无 `[Install]` 段，不能被直接 enable，通常由其他服务依赖触发 |
| `generated` | 由 systemd 自动生成 |

## 3.3 查看用户级服务

用户级服务在当前用户登录后启动，登出后停止，不需要 `sudo`：

```bash
# 查看当前用户的所有 service
systemctl --user list-unit-files --type=service

# 查看用户服务状态
systemctl --user status <service_name>

# 启动/停止用户服务
systemctl --user start <service_name>
systemctl --user stop <service_name>

# 启用用户服务开机自启（需先启用 loginctl lingering）
systemctl --user enable <service_name>
```

---

# 4. 自定义 Service Unit 文件

当需要将一个自定义脚本或程序注册为系统服务时，需要手动编写 `.service` 文件。

## 4.1 Unit 文件结构

`.service` 文件由三个 Section 组成：

```ini
[Unit]
Description=My Custom Service          # 服务描述
After=network.target                   # 在 network.target 启动后再启动本服务
Wants=network.target                   # 软依赖（推荐存在，但不强制）
Requires=mysql.service                 # 强依赖（依赖必须启动成功）

[Service]
Type=simple                            # 服务类型（见下表）
ExecStart=/usr/bin/python3 /opt/app/main.py   # 启动命令（必须用绝对路径）
ExecStop=/bin/kill -s QUIT $MAINPID    # 停止命令（可选）
ExecReload=/bin/kill -s HUP $MAINPID   # 重载命令（可选）
WorkingDirectory=/opt/app              # 工作目录
User=www-data                          # 以指定用户身份运行
Group=www-data                         # 以指定组身份运行
Restart=on-failure                     # 异常退出时自动重启
RestartSec=5                           # 重启前等待 5 秒
Environment="ENV=production"           # 设置环境变量
StandardOutput=journal                 # 标准输出写入 journal
StandardError=journal                  # 标准错误写入 journal

[Install]
WantedBy=multi-user.target             # 在 multi-user.target 启动时被拉起
```

**`[Service]` 中的 `Type` 类型**：

| Type | 说明 |
|---|---|
| `simple` | 默认值，`ExecStart` 启动的进程即为主进程 |
| `forking` | 进程启动后会 fork 并退出父进程（传统 daemon 模式） |
| `oneshot` | 进程执行完即退出，适合一次性脚本 |
| `notify` | 进程启动后通过 `sd_notify` 通知 systemd 已就绪 |
| `idle` | 等待所有任务完成后再启动 |

**`Restart` 策略**：

| 值 | 触发重启的条件 |
|---|---|
| `no` | 永不重启（默认） |
| `always` | 总是重启 |
| `on-failure` | 仅在非正常退出时重启（退出码非 0、被信号终止等） |
| `on-abnormal` | 信号终止、超时等异常情况时重启 |

## 4.2 创建并启用自定义服务

```bash
# 1. 创建 Unit 文件
sudo vim /etc/systemd/system/myapp.service

# 2. 重新加载 systemd 配置（每次修改 .service 文件后都需要执行）
sudo systemctl daemon-reload

# 3. 启动服务并设置开机自启
sudo systemctl enable --now myapp.service

# 4. 查看状态确认正常运行
systemctl status myapp.service
```

---

# 5. 查看系统日志（journalctl）

`journalctl` 是 `systemd` 配套的日志查询工具，用于查看由 `journald` 收集的系统和服务日志。

```bash
# 查看所有日志（从最旧到最新）
journalctl

# 查看指定服务的日志
journalctl -u nginx

# 实时追踪服务日志（类似 tail -f）
journalctl -u nginx -f

# 查看指定服务最近 50 行日志
journalctl -u nginx -n 50

# 查看本次启动以来的日志
journalctl -b

# 查看上一次启动的日志
journalctl -b -1

# 按时间范围过滤
journalctl --since "2024-01-01 00:00:00" --until "2024-01-02 00:00:00"
journalctl --since "1 hour ago"
journalctl --since "yesterday"

# 只显示错误及以上级别的日志（emerg/alert/crit/err/warning/notice/info/debug）
journalctl -p err

# 以 JSON 格式输出（便于脚本解析）
journalctl -u nginx -o json-pretty

# 查看内核日志（相当于 dmesg）
journalctl -k

# 清理旧日志（保留最近 2 周的日志）
sudo journalctl --vacuum-time=2weeks

# 清理日志，使其不超过 500MB
sudo journalctl --vacuum-size=500M
```

---

# 6. 开机自启动的其他方式

除 `systemd` 外，Linux 还有以下几种配置开机自启动的方式。

## 6.1 桌面环境自启动（XDG autostart）

适用于带有桌面环境（GNOME、KDE 等）的图形界面系统，用于在用户登录桌面后自动启动应用程序。

```bash
# 查看用户级别的自启动应用（.desktop 文件）
ls ~/.config/autostart

# 查看系统级别的自启动应用
ls /etc/xdg/autostart
```

`.desktop` 文件示例：

```ini
[Desktop Entry]
Type=Application
Name=My App
Exec=/usr/bin/myapp
Hidden=false
X-GNOME-Autostart-enabled=true
```

## 6.2 crontab 定时任务

`cron` 是传统的定时任务工具，支持在特定时间或系统启动时执行命令。

```bash
# 编辑当前用户的 crontab
crontab -e

# 查看当前用户的 crontab
crontab -l

# 查看指定用户的 crontab（需要 root 权限）
sudo crontab -l -u <username>

# 删除当前用户的 crontab
crontab -r
```

**crontab 格式**：

```
# 分  时  日  月  周  命令
  *   *   *   *   *  /path/to/command

# 每天 3:30 执行备份脚本
30 3 * * * /opt/scripts/backup.sh

# 每周一 8:00 发送报告
0 8 * * 1 /opt/scripts/report.sh

# 系统启动时执行（@reboot 是特殊时间标志）
@reboot /opt/scripts/startup.sh
```

**时间字段说明**：

| 字段 | 取值范围 | 特殊值 |
|---|---|---|
| 分钟 | 0–59 | `*` 每分钟，`*/5` 每 5 分钟 |
| 小时 | 0–23 | `*` 每小时 |
| 日期 | 1–31 | `*` 每天 |
| 月份 | 1–12 | `*` 每月 |
| 星期 | 0–7（0 和 7 均为周日） | `*` 每天 |

系统级 crontab 文件：

```bash
# 系统 crontab（含用户字段）
cat /etc/crontab

# 按时间维度分类的脚本目录
ls /etc/cron.hourly/
ls /etc/cron.daily/
ls /etc/cron.weekly/
ls /etc/cron.monthly/
```

## 6.3 /etc/rc.local

`/etc/rc.local` 是一个传统的兼容机制，在 `systemd` 下由 `rc-local.service` 执行，**在现代系统中已基本被 systemd service 取代**，部分发行版默认不再提供此文件。

```bash
# 查看文件内容（若存在）
cat /etc/rc.local

# 若需要使用，确保文件有可执行权限
sudo chmod +x /etc/rc.local
```

文件内容示例（命令写在 `exit 0` 之前）：

```bash
#!/bin/bash
/opt/scripts/startup.sh &
exit 0
```

> 新项目建议统一使用 `systemd service` 代替 `rc.local`，管理更规范，且支持依赖控制、自动重启、日志收集等功能。

---

# 7. systemd Target（系统运行目标）

`target` 是一组 Unit 的集合，用于定义系统的运行状态，类似于传统 SysV init 的运行级别（runlevel）。

**常用 target 与 SysV runlevel 对应关系**：

| systemd Target | SysV Runlevel | 说明 |
|---|---|---|
| `poweroff.target` | 0 | 关机 |
| `rescue.target` | 1 | 单用户救援模式 |
| `multi-user.target` | 3 | 多用户命令行模式（无图形界面） |
| `graphical.target` | 5 | 多用户图形界面模式 |
| `reboot.target` | 6 | 重启 |

```bash
# 查看当前默认 target
systemctl get-default

# 设置默认 target（如切换为纯命令行模式）
sudo systemctl set-default multi-user.target

# 设置默认 target（切换为图形界面模式）
sudo systemctl set-default graphical.target

# 立即切换到指定 target（不改变默认值）
sudo systemctl isolate multi-user.target

# 关机
sudo systemctl poweroff

# 重启
sudo systemctl reboot

# 挂起（暂停到内存）
sudo systemctl suspend

# 休眠（暂停到磁盘）
sudo systemctl hibernate
```
