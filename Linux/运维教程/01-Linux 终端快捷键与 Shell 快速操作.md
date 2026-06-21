---
title: Linux 终端快捷键与 Shell 快速操作
date: 2026-06-16
tags:
  - Linux
  - 运维
  - 终端
  - Shell
  - 快捷键
aliases:
  - Linux 终端快捷键
  - Shell 快捷操作
  - 命令行快捷键
---

# 1. 终端操作概述

Linux 运维大量工作发生在终端中。熟悉终端快捷键后，可以减少重复输入、快速修改长命令、恢复历史命令，并安全控制前台和后台任务。

> [!summary]
> 终端快速操作的核心：**命令行编辑靠 Readline，历史复用靠 history，任务控制靠 job control，长会话托管靠 tmux**。

---

# 2. 命令行编辑快捷键

多数 Bash、Zsh 和交互式命令行工具默认支持 GNU Readline 风格快捷键。

## 2.1 光标移动

| 快捷键 | 作用 | 记忆 |
|---|---|---|
| `Ctrl+A` | 移动到行首 | Ahead |
| `Ctrl+E` | 移动到行尾 | End |
| `Ctrl+B` / `left` | 向左移动一个字符 | Backward |
| `Ctrl+F` / `right` | 向右移动一个字符 | Forward |
| `Alt+B` / `ctrl+left` | 向左移动一个单词 | Backward word |
| `Alt+F` / `ctrl + right` | 向右移动一个单词 | Forward word |

> [!tip]
> 长命令改参数时，`Ctrl+A`、`Ctrl+E`、`Alt+B`、`Alt+F` 最实用，可以避免按方向键按到手软。

## 2.2 删除与修改

| 快捷键 | 作用 |
|---|---|
| `Ctrl+U` | 删除光标前所有内容 |
| `Ctrl+K` | 删除光标后所有内容 |
| `Ctrl+_` | 撤销上一次编辑，部分终端也支持 `Ctrl+/` |
| `Ctrl+W` | 删除光标前一个单词 |
| `Alt+D` | 删除光标后一个单词 |
| `Ctrl+H` | 删除光标前一个字符，等同 Backspace |
| `Ctrl+D` | 删除光标处字符，等同于 Delete；空行时退出 shell |
| `Ctrl+Y` | 粘贴刚删除的内容 |
| `Ctrl+T` | 交换光标前两个字符 |
| `Alt+T` | 交换光标附近两个单词 |

## 2.3 清屏与输入控制

| 快捷键 | 作用 |
|---|---|
| `Ctrl+L` | 清屏，等同 `clear` |
| `Ctrl+C` | 发送 `SIGINT`，中断当前前台程序 |
| `Ctrl+D` | 发送 EOF，常用于退出 shell 或结束输入 |
| `Ctrl+S` | 暂停终端输出，部分终端会表现为“卡住” |
| `Ctrl+Q` | 恢复被 `Ctrl+S` 暂停的输出 |

> [!warning]
> 终端突然“没反应”但进程未退出时，先试 `Ctrl+Q`。很多时候是误按了 `Ctrl+S` 触发终端流控。

---

# 3. 历史命令复用

## 3.1 历史搜索

| 操作 | 作用 |
|---|---|
| `history` | 查看历史命令 |
| `↑` / `↓` | 上一条 / 下一条历史命令 |
| `Ctrl+R` | 反向搜索历史命令 |
| `Ctrl+S` | 正向搜索历史命令，可能受终端流控影响 |
| `Ctrl+G` | 退出历史搜索 |

`Ctrl+R` 使用流程：

```text
Ctrl+R → 输入关键词 → 继续 Ctrl+R 切换更早匹配 → Enter 执行
```

## 3.2 历史展开

| 写法 | 作用 |
|---|---|
| `!!` | 执行上一条命令 |
| `sudo !!` | 用 sudo 重新执行上一条命令 |
| `!123` | 执行历史编号为 123 的命令 |
| `!ssh` | 执行最近一条以 `ssh` 开头的命令 |
| `!$` | 引用上一条命令的最后一个参数 |
| `Alt+.` | 插入上一条命令的最后一个参数 |

示例：

```bash
vim /etc/nginx/nginx.conf

# 忘记 sudo 后
sudo !!

# 复用上一条命令最后一个参数
ls -l !$
```

> [!warning]
> 历史展开会在执行前替换为真实命令。对删除、覆盖、生产变更命令使用 `!!`、`!123` 前，务必确认展开后的内容。

---

# 4. 任务控制

## 4.1 前台与后台

| 命令或快捷键 | 作用 |
|---|---|
| `command &` | 在后台启动命令 |
| `Ctrl+Z` | 挂起当前前台任务 |
| `jobs` | 查看当前 shell 的后台任务 |
| `fg` | 将最近任务切回前台 |
| `fg %1` | 将编号为 1 的任务切回前台 |
| `bg` | 让最近挂起任务在后台继续运行 |
| `bg %1` | 让编号为 1 的任务在后台继续运行 |
| `disown` | 将任务从当前 shell 的 job 表中移除 |

常见流程：

```bash
# 运行一个长任务
long_running_command

# 暂停任务
Ctrl+Z

# 放到后台继续运行
bg

# 查看任务
jobs

# 回到前台
fg
```

## 4.2 nohup 与后台保活

```bash
# shell 退出后仍继续运行
nohup command > app.log 2>&1 &

# 从当前 shell job 管理中移除
disown
```

> [!info]
> `nohup` 适合简单临时任务；长期服务应使用 systemd 管理，便于自动重启、日志收集和开机自启。

---

# 5. 常用 Shell 快速语法

> [!note]
> 下面的 `cmd` 都是 `command`（命令）的简称。

## 5.1 重定向

| 写法 | 含义 |
|---|---|
| `cmd > file` | 标准输出**覆盖**写入文件 |
| `cmd >> file` | 标准输出**追加**写入文件 |
| `cmd 2> err.log` | 标准**错误**写入文件 |
| `cmd > all.log 2>&1` | 标准输出和标准错误都写入文件 |
| `cmd < file` | 从文件读取标准输入 |

> [!warning]
> 重定向顺序很重要：`cmd > all.log 2>&1` 会把 stdout 和 stderr 都写入文件；`cmd 2>&1 > all.log` 会让 stderr 仍输出到终端。

## 5.2 管道与条件执行

| 写法 | 含义 |
|---|---|
| `cmd1 \| cmd2` | 将 `cmd1` 输出交给 `cmd2` |
| `cmd1 && cmd2` | `cmd1` 成功后执行 `cmd2` |
| `cmd1 \|\| cmd2` | `cmd1` 失败后执行 `cmd2` |
| `echo "$?"` | 查看上一条命令退出码 |

示例：

```bash
# 查看最近错误
grep -i "error" app.log | tail -n 50

# 创建目录成功后进入
mkdir -p /opt/myapp && cd /opt/myapp

# 失败时输出提示
systemctl is-active nginx || echo "nginx not running"
```

---

# 6. tmux 会话管理

`tmux` 用于在远程服务器上托管长时间终端会话，即使 SSH 断开，会话也能保留。

## 6.1 会话命令

| 命令 | 作用 |
|---|---|
| `tmux new -s name` | 创建命名会话 |
| `tmux ls` | 列出会话 |
| `tmux attach -t name` | 进入指定会话 |
| `tmux detach` | 脱离当前会话 |
| `tmux kill-session -t name` | 关闭指定会话 |

## 6.2 常用快捷键

`tmux` 默认前缀键是 `Ctrl+B`。先按前缀键，再按后续键。

| 快捷键 | 作用 |
|---|---|
| `Ctrl+B D` | 脱离当前会话 |
| `Ctrl+B C` | 新建窗口 |
| `Ctrl+B N` | 下一个窗口 |
| `Ctrl+B P` | 上一个窗口 |
| `Ctrl+B ,` | 重命名窗口 |
| `Ctrl+B %` | 左右分屏 |
| `Ctrl+B "` | 上下分屏 |
| `Ctrl+B 方向键` | 切换 pane |
| `Ctrl+B X` | 关闭 pane |
| `Ctrl+B [` | 进入复制/滚动模式，按 `q` 退出 |

> [!tip]
> 远程排障、日志跟踪、发布操作建议放在 tmux 中进行，避免 SSH 断开导致上下文丢失。

---

# 7. 常见场景速查

| 场景 | 推荐操作 |
|---|---|
| 长命令跳到行首 | `Ctrl+A` |
| 长命令跳到行尾 | `Ctrl+E` |
| 删除前一个参数 | `Ctrl+W` |
| 找历史命令 | `Ctrl+R` |
| 忘记 sudo | `sudo !!` |
| 终端疑似卡住 | `Ctrl+Q` |
| 中断当前命令 | `Ctrl+C` |
| 暂停当前命令 | `Ctrl+Z` |
| 后台继续运行 | `bg` |
| 回到前台 | `fg` |
| SSH 长会话 | `tmux new -s name` |
| 退出 tmux 但保留会话 | `Ctrl+B D` |

---

# 8. 易错点总结

> [!summary]
> 本篇核心：终端效率来自快捷键和历史复用，但生产环境中任何历史展开、后台任务和重定向都要先确认影响范围。

- `Ctrl+C` 是中断，`Ctrl+Z` 是挂起，二者含义不同。
- `Ctrl+S` 可能暂停终端输出，恢复用 `Ctrl+Q`。
- `sudo !!` 很方便，但执行的是上一条命令，生产环境必须先确认。
- 临时后台任务可用 `nohup`，长期服务应使用 systemd。
- tmux 是远程服务器长时间操作的安全垫，排障和发布都值得默认使用。
