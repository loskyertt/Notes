---
title: Linux systemd 与 systemctl 服务管理
date: 2026-06-15
tags:
  - Linux
  - 运维
  - systemd
  - systemctl
aliases:
  - systemctl
  - systemd 服务管理
---

# 1. systemd 概述

`systemd` 是主流 Linux 发行版常用的 init 系统和服务管理器，是系统启动后的第一个进程，PID 为 `1`。

`systemctl` 是与 `systemd` 交互的主要命令，用于管理服务、开机自启、运行目标和 Unit 配置。

> [!summary]
> systemd 把服务、挂载点、定时器、套接字等资源统一抽象为 **Unit**，再通过依赖关系组织系统启动和运行。

# 2. Unit 基础

## 2.1 常见 Unit 类型

| 类型 | 扩展名 | 说明 |
|---|---|---|
| Service | `.service` | 系统服务，最常用 |
| Socket | `.socket` | 套接字激活 |
| Target | `.target` | 一组 Unit 的集合 |
| Timer | `.timer` | 定时触发任务 |
| Mount | `.mount` | 文件系统挂载 |
| Path | `.path` | 文件路径变化触发 |
| Slice | `.slice` | cgroup 资源分组 |

## 2.2 Unit 文件位置

| 路径 | 说明 |
|---|---|
| `/lib/systemd/system/` | 软件包提供的系统默认 Unit，不建议直接改 |
| `/usr/lib/systemd/system/` | 部分发行版的软件包 Unit 目录 |
| `/etc/systemd/system/` | 管理员自定义或覆盖配置，优先级高 |
| `~/.config/systemd/user/` | 当前用户的用户级 Unit |

> [!tip]
> 自定义服务通常放在 `/etc/systemd/system/`，不要直接修改软件包安装在 `/lib/systemd/system/` 中的文件。

# 3. 服务状态查看

```bash
# 查看状态、PID、最近日志
systemctl status nginx

# 是否正在运行
systemctl is-active nginx

# 是否设置开机自启
systemctl is-enabled nginx

# 是否处于失败状态
systemctl is-failed nginx
```

`systemctl status` 关键字段：

```text
● nginx.service - A high performance web server
     Loaded: loaded (/lib/systemd/system/nginx.service; enabled; ...)
              │                                            └── 开机自启状态
              └── Unit 文件路径
     Active: active (running) since ...
              └── 当前运行状态
   Main PID: 1234 (nginx)
      Tasks: 3
     CGroup: /system.slice/nginx.service
```

# 4. 启动、停止与重载

```bash
# 启动
sudo systemctl start nginx

# 停止
sudo systemctl stop nginx

# 重启，先 stop 再 start
sudo systemctl restart nginx

# 重读配置，不中断进程，需服务支持
sudo systemctl reload nginx

# 支持 reload 则 reload，否则 restart
sudo systemctl reload-or-restart nginx
```

| 操作 | 行为 | 影响 |
|---|---|---|
| `restart` | 停止进程再启动 | 可能短暂中断连接 |
| `reload` | 让进程重读配置 | 通常不中断连接，但需服务支持 |
| `reload-or-restart` | 优先 reload，失败再 restart | 更适合生产变更 |

> [!tip]
> 修改 Nginx、MySQL、应用服务等配置后，优先确认服务是否支持 reload。生产环境避免无脑 restart。

# 5. 开机自启与屏蔽

```bash
# 启用开机自启
sudo systemctl enable nginx

# 启用开机自启并立即启动
sudo systemctl enable --now nginx

# 禁用开机自启
sudo systemctl disable nginx

# 禁用开机自启并立即停止
sudo systemctl disable --now nginx

# 屏蔽服务，禁止被手动或依赖启动
sudo systemctl mask nginx

# 取消屏蔽
sudo systemctl unmask nginx
```

| 操作 | 开机自启 | 能否手动启动 | 适用场景 |
|---|---|---|---|
| `disable` | 否 | 可以 | 不希望开机自动运行 |
| `mask` | 否 | 不可以 | 明确禁止该服务启动 |

# 6. 重新加载配置

修改或新增 Unit 文件后，需要让 systemd 重新读取配置。

```bash
sudo systemctl daemon-reload
```

> [!warning]
> 修改 `.service` 文件后如果不执行 `daemon-reload`，systemd 仍可能使用旧配置。

# 7. 查看服务列表

## 7.1 运行状态列表

```bash
# 当前运行中的 service
systemctl list-units --type=service --state=running

# 所有已加载 service
systemctl list-units --type=service

# 所有 Unit，包括未激活
systemctl list-units --all
```

## 7.2 自启状态列表

```bash
# 已启用的 service
systemctl list-unit-files --type=service --state=enabled

# 所有 service 及启用状态
systemctl list-unit-files --type=service

# 所有已启用 Unit
systemctl list-unit-files --state=enabled
```

| 状态 | 含义 |
|---|---|
| `enabled` | 已设置开机自启 |
| `disabled` | 未设置开机自启 |
| `masked` | 已屏蔽，无法启动 |
| `static` | 不能直接 enable，通常由依赖触发 |
| `generated` | systemd 自动生成 |

# 8. 用户级服务

用户级服务属于当前用户，不需要 `sudo`。

```bash
# 查看用户级 service
systemctl --user list-unit-files --type=service

# 查看状态
systemctl --user status <service>

# 启动与停止
systemctl --user start <service>
systemctl --user stop <service>

# 启用自启
systemctl --user enable <service>
```

如需用户登出后仍运行用户级服务，通常还要启用 lingering：

```bash
sudo loginctl enable-linger <username>
```

# 9. 自定义 Service Unit

## 9.1 文件结构

```ini
[Unit]
Description=My Custom Service
After=network.target
Wants=network.target

[Service]
Type=simple
ExecStart=/usr/bin/python3 /opt/app/main.py
ExecReload=/bin/kill -HUP $MAINPID
WorkingDirectory=/opt/app
User=www-data
Group=www-data
Restart=on-failure
RestartSec=5
Environment="ENV=production"
StandardOutput=journal
StandardError=journal

[Install]
WantedBy=multi-user.target
```

| Section | 作用 |
|---|---|
| `[Unit]` | 描述服务和依赖关系 |
| `[Service]` | 定义如何启动、停止、重启服务 |
| `[Install]` | 定义 enable 时挂到哪个 target |

## 9.2 常见 Service 类型

| Type | 说明 |
|---|---|
| `simple` | 默认，`ExecStart` 进程就是主进程 |
| `forking` | 传统 daemon 模式，父进程退出，子进程常驻 |
| `oneshot` | 执行完即退出，适合一次性任务 |
| `notify` | 服务通过 `sd_notify` 告诉 systemd 已就绪 |
| `idle` | 等系统空闲后再启动 |

## 9.3 Restart 策略

| 值 | 触发条件 |
|---|---|
| `no` | 永不自动重启 |
| `always` | 总是重启 |
| `on-failure` | 非正常退出时重启 |
| `on-abnormal` | 信号终止、超时等异常时重启 |

## 9.4 创建并启用服务

```bash
# 1. 创建 Unit 文件
sudo vim /etc/systemd/system/myapp.service

# 2. 重新加载 systemd 配置
sudo systemctl daemon-reload

# 3. 启动并设置开机自启
sudo systemctl enable --now myapp.service

# 4. 查看状态
systemctl status myapp.service
```

> [!warning]
> `ExecStart` 建议使用绝对路径。环境变量、工作目录、运行用户不要依赖交互式 shell 的默认状态。

# 10. journalctl 日志

```bash
# 查看所有日志
journalctl

# 查看指定服务日志
journalctl -u nginx

# 实时追踪服务日志
journalctl -u nginx -f

# 最近 50 行
journalctl -u nginx -n 50

# 本次启动以来的日志
journalctl -b

# 上一次启动的日志
journalctl -b -1

# 时间范围过滤
journalctl --since "2026-06-15 10:00:00" --until "2026-06-15 11:00:00"
journalctl --since "1 hour ago"

# 只看错误及以上级别
journalctl -p err

# JSON 输出
journalctl -u nginx -o json-pretty

# 内核日志
journalctl -k
```

清理日志：

```bash
# 保留最近 2 周
sudo journalctl --vacuum-time=2weeks

# 限制总大小为 500MB
sudo journalctl --vacuum-size=500M
```

# 11. Target 与运行目标

`target` 是一组 Unit 的集合，类似传统 SysV init 的运行级别。

| systemd Target | SysV Runlevel | 说明 |
|---|---:|---|
| `poweroff.target` | 0 | 关机 |
| `rescue.target` | 1 | 单用户救援模式 |
| `multi-user.target` | 3 | 多用户命令行模式 |
| `graphical.target` | 5 | 图形界面模式 |
| `reboot.target` | 6 | 重启 |

```bash
# 当前默认 target
systemctl get-default

# 设置默认命令行模式
sudo systemctl set-default multi-user.target

# 设置默认图形界面模式
sudo systemctl set-default graphical.target

# 立即切换 target，不改变默认值
sudo systemctl isolate multi-user.target

# 关机与重启
sudo systemctl poweroff
sudo systemctl reboot
```

# 12. 开机自启动的其他方式

## 12.1 XDG autostart

用于桌面环境登录后自动启动图形应用。

```bash
ls ~/.config/autostart
ls /etc/xdg/autostart
```

## 12.2 crontab

```bash
# 编辑当前用户 crontab
crontab -e

# 查看当前用户 crontab
crontab -l

# 系统启动时执行
@reboot /opt/scripts/startup.sh
```

## 12.3 rc.local

`/etc/rc.local` 是传统兼容机制，现代系统中应优先使用 systemd service。

```bash
cat /etc/rc.local
sudo chmod +x /etc/rc.local
```

> [!summary]
> 新项目开机自启动优先使用 systemd service，只有桌面应用考虑 XDG autostart，简单定时任务考虑 crontab。

# 13. 常见排障流程

```bash
# 1. 看服务状态
systemctl status myapp

# 2. 看最近日志
journalctl -u myapp -n 100

# 3. 实时追踪
journalctl -u myapp -f

# 4. 修改 Unit 后重载
sudo systemctl daemon-reload

# 5. 重启或 reload
sudo systemctl reload-or-restart myapp

# 6. 确认是否运行和自启
systemctl is-active myapp
systemctl is-enabled myapp
```

# 14. 易错点总结

> [!summary]
> 本篇核心：服务状态看 `status`，日志看 `journalctl`，自启看 `enable`，Unit 修改后必须 `daemon-reload`。

- `enable` 只设置开机自启，不等于立即启动；需要立即启动用 `enable --now`。
- `disable` 不会阻止手动启动；彻底禁止用 `mask`。
- `restart` 会中断服务，生产配置更新优先考虑 `reload` 或 `reload-or-restart`。
- 自定义 Unit 的命令路径尽量写绝对路径。
- 用户级服务与系统级服务是两套命名空间，命令是否加 `--user` 很关键。
