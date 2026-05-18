# 1. grep 概述

`grep`（Global Regular Expression Print，全局正则表达式打印）是 Unix / Linux 中最常用的文本搜索工具，用于在文件或标准输入中查找匹配指定模式的行并输出。

**基本语法**：

```bash
grep [选项] <模式> [文件...]
```

- `<模式>`：要搜索的文本字符串或正则表达式。
- `[文件...]`：要搜索的一个或多个文件；省略时从标准输入读取（常与管道配合使用）。

---

# 2. 常用选项

## 2.1 匹配控制选项

| 选项 | 说明 |
|---|---|
| `-i` | 忽略大小写（case-insensitive） |
| `-v` | 反向匹配，输出**不**匹配的行 |
| `-w` | 全词匹配，模式必须匹配完整单词（两侧为非单词字符） |
| `-x` | 全行匹配，模式必须匹配整行内容 |
| `-e <pattern>` | 指定一个匹配模式，可多次使用以支持多个模式（OR 关系） |
| `-f <file>` | 从文件中读取模式，每行一个 |

## 2.2 输出控制选项

| 选项 | 说明 |
|---|---|
| `-n` | 在输出行前显示行号 |
| `-c` | 只输出匹配行的数量，不显示具体内容 |
| `-l` | 只输出包含匹配行的**文件名**（多文件搜索时常用） |
| `-L` | 只输出**不包含**匹配内容的文件名 |
| `-o` | 只输出每行中实际匹配的部分，而非整行 |
| `-m <num>` | 找到指定数量的匹配行后停止搜索 |
| `-q` | 静默模式，不输出任何内容，只通过退出码表示是否匹配（0=匹配，1=不匹配） |
| `--color` | 将匹配的文本高亮显示（通常默认已开启） |

## 2.3 上下文选项

在排查日志时，常需要查看匹配行周围的上下文内容：

| 选项 | 说明 |
|---|---|
| `-A <num>` | 输出匹配行及其**后** num 行（After） |
| `-B <num>` | 输出匹配行及其**前** num 行（Before） |
| `-C <num>` | 输出匹配行及其**前后**各 num 行（Context），等价于 `-A <num> -B <num>` |

## 2.4 文件与目录选项

| 选项 | 说明 |
|---|---|
| `-r` | 递归搜索目录下的所有文件（不跟随符号链接） |
| `-R` | 递归搜索，同时跟随符号链接 |
| `--include=<pattern>` | 递归搜索时只处理匹配的文件名（如 `--include="*.log"`） |
| `--exclude=<pattern>` | 递归搜索时跳过匹配的文件名 |
| `--exclude-dir=<dir>` | 递归搜索时跳过匹配的目录名 |

## 2.5 正则表达式模式选项

| 选项 | 说明 |
|---|---|
| `-E` | 使用扩展正则表达式（ERE），等价于 `egrep` |
| `-F` | 将模式视为固定字符串（不解释正则元字符），等价于 `fgrep`，速度最快 |
| `-P` | 使用 Perl 兼容正则表达式（PCRE），支持 `\d`、`\s`、`?<=` 等语法 |

---

# 3. 基本用法示例

## 3.1 基础搜索

```bash
# 在文件中搜索包含指定文本的行
grep "error" /var/log/syslog

# 忽略大小写
grep -i "error" /var/log/syslog

# 显示匹配行及其行号
grep -n "error" /var/log/syslog

# 反向匹配：显示不包含 "debug" 的行
grep -v "debug" /var/log/syslog

# 全词匹配："err" 不会匹配 "error"，只匹配独立的单词 "err"
grep -w "err" /var/log/syslog
```

## 3.2 统计与文件名

```bash
# 统计匹配行的数量
grep -c "error" /var/log/syslog

# 在多个文件中搜索，只显示包含匹配内容的文件名
grep -l "TODO" *.py

# 只输出实际匹配的部分（而非整行）
grep -o "[0-9]\{1,3\}\.[0-9]\{1,3\}\.[0-9]\{1,3\}\.[0-9]\{1,3\}" access.log
# 提取日志中所有 IP 地址
```

## 3.3 上下文查看

```bash
# 显示匹配行及其后 3 行
grep -A 3 "Exception" app.log

# 显示匹配行及其前 3 行
grep -B 3 "Exception" app.log

# 显示匹配行及其前后各 3 行（查日志时最常用）
grep -C 3 "Exception" app.log
```

## 3.4 递归搜索

```bash
# 递归搜索目录下所有文件
grep -r "TODO" /path/to/project

# 只搜索 .py 文件
grep -r "TODO" /path/to/project --include="*.py"

# 排除 .git 目录
grep -r "TODO" /path/to/project --exclude-dir=".git"

# 递归搜索，只显示包含匹配内容的文件名
grep -rl "password" /etc
```

## 3.5 多模式搜索

```bash
# 使用 -e 指定多个模式（OR 关系）
grep -e "error" -e "warning" -e "critical" /var/log/syslog

# 使用 -f 从文件读取模式列表
echo -e "error\nwarning\ncritical" > patterns.txt
grep -f patterns.txt /var/log/syslog
```

## 3.6 静默模式（脚本中常用）

```bash
# -q 不输出内容，只返回退出码（0=找到，1=未找到）
if grep -q "error" /var/log/syslog; then
    echo "发现错误日志"
fi
```

---

# 4. 正则表达式

`grep` 默认使用**基本正则表达式（BRE）**，加 `-E` 选项后使用**扩展正则表达式（ERE）**，加 `-P` 后使用 **Perl 兼容正则表达式（PCRE）**。

## 4.1 基本元字符

| 元字符 | 含义 | 示例 |
|---|---|---|
| `.` | 匹配任意**单个**字符（换行符除外） | `gr.p` 匹配 `grep`、`grap` |
| `*` | 匹配前一个字符**零次或多次** | `ab*c` 匹配 `ac`、`abc`、`abbc` |
| `^` | 匹配行**开头** | `^root` 匹配以 root 开头的行 |
| `$` | 匹配行**末尾** | `sh$` 匹配以 sh 结尾的行 |
| `[]` | 字符类，匹配括号内任意一个字符 | `[aeiou]` 匹配任意元音字母 |
| `[^]` | 否定字符类，匹配**不在**括号内的字符 | `[^0-9]` 匹配非数字字符 |
| `\` | 转义字符，使元字符失去特殊含义 | `\.` 匹配字面量的点 |

**在 BRE 中**，`{}`、`()`、`+`、`?` 需要加反斜杠转义才有特殊含义（`\{`、`\(`、`\+`、`\?`）；在 ERE（`-E`）中则直接使用。

## 4.2 扩展正则表达式（ERE，需加 `-E`）

| 元字符 | 含义 | 示例 |
|---|---|---|
| `+` | 匹配前一个字符**一次或多次** | `ab+c` 匹配 `abc`、`abbc`，不匹配 `ac` |
| `?` | 匹配前一个字符**零次或一次** | `colou?r` 匹配 `color` 和 `colour` |
| `{n}` | 精确匹配 n 次 | `[0-9]{4}` 匹配四位数字 |
| `{n,m}` | 匹配 n 到 m 次 | `[0-9]{2,4}` 匹配 2 到 4 位数字 |
| `{n,}` | 匹配至少 n 次 | `[0-9]{3,}` 匹配至少 3 位数字 |
| `()` | 分组 | `(ab)+` 匹配 `ab`、`abab` |
| `\|` | 交替（OR） | `cat\|dog` 匹配 `cat` 或 `dog` |

## 4.3 常用字符类简写

| 简写 | 等价字符类 | 含义 |
|---|---|---|
| `[[:alpha:]]` | `[a-zA-Z]` | 字母 |
| `[[:digit:]]` | `[0-9]` | 数字 |
| `[[:alnum:]]` | `[a-zA-Z0-9]` | 字母或数字 |
| `[[:space:]]` | `[ \t\n\r]` | 空白字符 |
| `[[:upper:]]` | `[A-Z]` | 大写字母 |
| `[[:lower:]]` | `[a-z]` | 小写字母 |
| `[[:punct:]]` | 标点符号 | 标点符号 |

在 PCRE（`-P`）模式下还可使用 `\d`（数字）、`\w`（单词字符）、`\s`（空白字符）等简写。

## 4.4 正则表达式示例

```bash
# 匹配以数字开头的行
grep "^[0-9]" filename.txt

# 匹配空行
grep "^$" filename.txt

# 匹配不含任何内容的行（包括只有空格的行）
grep -E "^\s*$" filename.txt

# 匹配有效的 IPv4 地址（简化版）
grep -E "([0-9]{1,3}\.){3}[0-9]{1,3}" filename.txt

# 匹配邮箱地址（简化版）
grep -E "[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\.[a-zA-Z]{2,}" filename.txt

# 匹配 color 或 colour（ERE）
grep -E "colou?r" filename.txt

# 匹配以 .sh 或 .py 结尾的行
grep -E "\.(sh|py)$" filename.txt

# 使用 PCRE 提取 HTTP 状态码
grep -oP "HTTP/\d\.\d \K\d{3}" access.log
```

---

# 5. 与管道结合使用

`grep` 最常见的使用场景是作为管道中的过滤器，对其他命令的输出进行筛选。

```bash
# 过滤进程列表，查找特定进程
ps aux | grep "nginx"

# 查找进程时排除 grep 自身（避免 grep 进程出现在结果中）
ps aux | grep "nginx" | grep -v "grep"

# 更简洁的写法：用方括号让模式不匹配自身
ps aux | grep "[n]ginx"

# 查看系统日志中最近的错误（结合 tail）
tail -f /var/log/syslog | grep "error"

# 在命令历史中搜索
history | grep "git commit"

# 统计日志中各类错误出现的次数
grep -oE "(ERROR|WARN|INFO)" app.log | sort | uniq -c | sort -rn

# 过滤 netstat 输出，查看指定端口的连接
netstat -an | grep ":80"

# 在压缩文件中搜索（zgrep 是 grep 处理 .gz 文件的包装器）
zgrep "error" /var/log/syslog.gz
```

---

# 6. grep 的变体命令

| 命令 | 等价于 | 说明 |
|---|---|---|
| `egrep` | `grep -E` | 使用扩展正则表达式，新脚本建议用 `grep -E` 替代 |
| `fgrep` | `grep -F` | 将模式视为固定字符串，不解释元字符，速度最快 |
| `rgrep` | `grep -r` | 递归搜索 |
| `zgrep` | — | 在 `.gz` 压缩文件中搜索，用法与 `grep` 相同 |

> `egrep` 和 `fgrep` 在较新的系统中已被标记为过时（deprecated），推荐直接使用 `grep -E` 和 `grep -F`。

---

# 7. 退出状态码

`grep` 的退出码在脚本判断中非常有用：

| 退出码 | 含义 |
|---|---|
| `0` | 找到至少一行匹配 |
| `1` | 未找到任何匹配 |
| `2` | 发生错误（如文件不存在、语法错误） |

```bash
grep -q "error" app.log
echo $?   # 输出 0（找到）或 1（未找到）
```
