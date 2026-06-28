---
title: Linux grep 文本搜索
date: 2026-06-15
tags:
  - Linux
  - 运维
  - grep
  - 文本处理
aliases:
  - grep
  - Linux 文本搜索
---

# 1. grep 概述

`grep`（Global Regular Expression Print）用于在文件或标准输入中查找匹配模式的行，是日志排查和文本过滤中最高频的命令之一。

```bash
grep [选项] <模式> [文件...]
```

| 参数 | 含义 |
|---|---|
| `<模式>` | 普通字符串或正则表达式 |
| `[文件...]` | 一个或多个文件；省略时读取标准输入 |

> [!summary]
> grep 的核心能力：**匹配行、过滤行、递归搜文件、结合管道处理命令输出**。

# 2. 常用选项

## 2.1 匹配控制

| 选项 | 说明 |
|---|---|
| `-i` | 忽略大小写 |
| `-v` | 反向匹配，输出不匹配的行 |
| `-w` | 全词匹配 |
| `-x` | 全行匹配 |
| `-e <pattern>` | 指定一个模式，可多次使用 |
| `-f <file>` | 从文件读取模式，每行一个 |

## 2.2 输出控制

| 选项 | 说明 |
|---|---|
| `-n` | 显示行号 |
| `-c` | 只输出匹配行数量 |
| `-l` | 只输出包含匹配内容的文件名 |
| `-L` | 只输出不包含匹配内容的文件名 |
| `-o` | 只输出每行实际匹配部分 |
| `-m <num>` | 匹配到指定行数后停止 |
| `-q` | 静默模式，只通过退出码表示结果 |
| `--color=auto` | 高亮匹配内容 |

## 2.3 上下文控制

| 选项 | 说明 |
|---|---|
| `-A <num>` | 输出匹配行及其后 `num` 行 |
| `-B <num>` | 输出匹配行及其前 `num` 行 |
| `-C <num>` | 输出匹配行前后各 `num` 行 |

## 2.4 文件与目录

| 选项 | 说明 |
|---|---|
| `-r` | 递归搜索目录，不跟随符号链接 |
| `-R` | 递归搜索目录，并跟随符号链接 |
| `--include=<pattern>` | 只搜索匹配文件名 |
| `--exclude=<pattern>` | 排除匹配文件名 |
| `--exclude-dir=<dir>` | 排除目录 |

## 2.5 正则模式

| 选项 | 说明 |
|---|---|
| `-E` | 使用扩展正则表达式 |
| `-F` | 固定字符串匹配，不解释正则元字符 |
| `-P` | 使用 Perl 兼容正则表达式 |

> [!tip]
> 搜索普通固定文本时优先考虑 `grep -F`，语义清晰且通常更快。

# 3. 基础用法

## 3.1 搜索文本

```bash
# 搜索包含 error 的行
grep "error" /var/log/syslog

# 忽略大小写
grep -i "error" /var/log/syslog

# 显示行号
grep -n "error" /var/log/syslog

# 排除 debug 行
grep -v "debug" /var/log/syslog

# 全词匹配 err
grep -w "err" /var/log/syslog
```

## 3.2 统计与文件名输出

```bash
# 统计匹配行数量
grep -c "error" app.log

# 多文件搜索，只显示包含匹配内容的文件名
grep -l "TODO" *.py

# 只输出匹配内容
grep -o "[0-9]\{1,3\}\.[0-9]\{1,3\}\.[0-9]\{1,3\}\.[0-9]\{1,3\}" access.log
```

## 3.3 查看上下文

```bash
# 匹配行后 3 行
grep -A 3 "Exception" app.log

# 匹配行前 3 行
grep -B 3 "Exception" app.log

# 匹配行前后各 3 行
grep -C 3 "Exception" app.log
```

## 3.4 递归搜索

```bash
# 递归搜索目录
grep -r "TODO" /path/to/project

# 只搜索 .py 文件
grep -r "TODO" /path/to/project --include="*.py"

# 排除 .git 目录
grep -r "TODO" /path/to/project --exclude-dir=".git"

# 只显示包含 password 的文件名
grep -rl "password" /etc
```

## 3.5 多模式搜索

```bash
# 多个模式是 OR 关系
grep -e "error" -e "warning" -e "critical" app.log

# 从文件读取模式
grep -f patterns.txt app.log
```

## 3.6 脚本判断

```bash
if grep -q "error" /var/log/syslog; then
    echo "发现错误日志"
fi
```

| 退出码 | 含义 |
|---:|---|
| 0 | 找到匹配 |
| 1 | 未找到匹配 |
| 2 | 发生错误 |

# 4. 正则表达式

## 4.1 基本正则表达式

`grep` 默认使用基本正则表达式（BRE）。

| 元字符 | 含义 | 示例 |
|---|---|---|
| `.` | 匹配任意单个字符 | `gr.p` |
| `*` | 前一个字符出现零次或多次 | `ab*c` |
| `^` | 行开头 | `^root` |
| `$` | 行末尾 | `sh$` |
| `[]` | 字符类 | `[aeiou]` |
| `[^]` | 否定字符类 | `[^0-9]` |
| `\` | 转义 | `\.` |

> [!info]
> BRE 中 `{}`、`()`、`+`、`?` 需要写成 `\{}`、`\(\)`、`\+`、`\?` 才表示特殊含义。

## 4.2 扩展正则表达式

加 `-E` 后使用扩展正则表达式（ERE），分组、次数和 OR 写法更自然。

| 元字符 | 含义 | 示例 |
|---|---|---|
| `+` | 前一个字符一次或多次 | `ab+c` |
| `?` | 前一个字符零次或一次 | `colou?r` |
| `{n}` | 精确 n 次 | `[0-9]{4}` |
| `{n,m}` | n 到 m 次 | `[0-9]{2,4}` |
| `()` | 分组 | `(ab)+` |
| `|` | 或 | `cat|dog` |

## 4.3 POSIX 字符类

| 写法 | 含义 |
|---|---|
| `[[:alpha:]]` | 字母 |
| `[[:digit:]]` | 数字 |
| `[[:alnum:]]` | 字母或数字 |
| `[[:space:]]` | 空白字符 |
| `[[:upper:]]` | 大写字母 |
| `[[:lower:]]` | 小写字母 |
| `[[:punct:]]` | 标点符号 |

## 4.4 常见正则示例

```bash
# 以数字开头
grep "^[0-9]" file.txt

# 空行
grep "^$" file.txt

# 只包含空白或为空的行
grep -E "^[[:space:]]*$" file.txt

# IPv4 简化匹配
grep -E "([0-9]{1,3}\.){3}[0-9]{1,3}" access.log

# 邮箱简化匹配
grep -E "[A-Za-z0-9._%+-]+@[A-Za-z0-9.-]+\.[A-Za-z]{2,}" file.txt

# 匹配 .sh 或 .py 结尾
grep -E "\.(sh|py)$" file.txt

# 使用 PCRE 提取 HTTP 状态码
grep -oP "HTTP/\d\.\d \K\d{3}" access.log
```

# 5. 管道组合

`grep` 常作为管道中的过滤器。

```bash
# 查找进程
ps aux | grep "nginx"

# 排除 grep 自身
ps aux | grep "nginx" | grep -v "grep"

# 更简洁的排除自身写法
ps aux | grep "[n]ginx"

# 实时过滤日志
tail -F /var/log/syslog | grep -i "error"

# 搜索历史命令
history | grep "git commit"

# 统计日志级别分布
grep -oE "(ERROR|WARN|INFO)" app.log | sort | uniq -c | sort -rn

# 查看 80 端口连接
ss -antp | grep ":80"

# 在 gzip 日志中搜索
zgrep "error" /var/log/syslog.gz
```

> [!tip]
> `grep "[n]ginx"` 能匹配 `nginx`，但 grep 进程自己的命令行是 `[n]ginx`，因此不会匹配自己。

# 6. grep 变体

| 命令 | 等价写法 | 说明 |
|---|---|---|
| `egrep` | `grep -E` | 扩展正则，旧写法 |
| `fgrep` | `grep -F` | 固定字符串，旧写法 |
| `rgrep` | `grep -r` | 递归搜索 |
| `zgrep` | 无完全等价 | 搜索 `.gz` 压缩文件 |

> [!info]
> 新脚本建议写 `grep -E` 和 `grep -F`，不要继续依赖 `egrep`、`fgrep`。

# 7. 日志排查模板

```bash
# 查看最近错误上下文
grep -iC 3 "error" app.log | tail -n 100

# 查看一段时间的日志
grep "2026-06-15 10:" app.log

# 查看多类异常
grep -iE "error|exception|panic|fatal|timeout|refused" app.log

# 找出出现 5xx 的访问日志
grep -E ' "5[0-9]{2} ' access.log

# 统计错误类型
grep -oE "ERROR|WARN|FATAL" app.log | sort | uniq -c | sort -nr
```

# 8. 易错点总结

> [!summary]
> 本篇核心：普通文本用 `grep -F`，复杂模式用 `grep -E`，脚本判断用 `grep -q`，日志排查多用 `-n`、`-C`、`tail -F`。

- `grep` 默认按行匹配，不跨行匹配。
- BRE 与 ERE 的元字符规则不同，复杂正则优先使用 `grep -E`。
- 搜索变量内容时要注意 shell 引号，避免空格或特殊字符被提前解释。
- 递归搜索大目录时使用 `--exclude-dir` 排除 `.git`、`node_modules`、日志归档目录。
- `grep -P` 并非所有系统都完整支持，脚本可移植性弱于 `-E`。
