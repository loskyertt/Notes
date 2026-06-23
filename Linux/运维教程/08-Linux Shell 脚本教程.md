---
title: Linux Shell 脚本教程
date: 2026-06-23
tags:
  - Linux
  - 运维
  - Shell
  - Bash
  - 脚本
aliases:
  - Shell 脚本
  - Bash 脚本教程
  - Linux 自动化脚本
---

# 1. Shell 脚本概述

Shell 是用户与 Linux 系统交互的命令解释器，负责解析命令、启动程序、处理变量、管道、重定向和脚本控制逻辑。

**Shell 脚本**就是把一组命令和控制逻辑保存到文件中，让它们可以被重复执行、自动执行和稳定交付。

| Shell | 说明 | 常见场景 |
|---|---|---|
| `sh` | Bourne Shell 或 POSIX Shell | 追求最大兼容性的脚本 |
| `bash` | Bourne Again Shell | Linux 运维脚本最常用 |
| `zsh` | 交互体验强，配置生态丰富 | 个人终端环境 |
| `fish` | 语法友好，默认体验好 | 个人交互式 Shell |

> [!summary]
> 运维脚本优先使用 **Bash**：它兼顾 Linux 普及度、数组、`[[ ]]`、函数、错误处理和工程实践。

---

# 2. 第一个脚本

## 2.1 创建脚本

```bash
vim hello.sh
```

写入内容：

```bash
#!/usr/bin/env bash

echo "Hello, Shell"
```

## 2.2 运行脚本

方式一：赋予执行权限后运行。

```bash
chmod +x hello.sh
./hello.sh
```

方式二：直接指定解释器运行。

```bash
bash hello.sh
```

| 运行方式 | 是否需要执行权限 | 是否使用 shebang |
|---|---|---|
| `./hello.sh` | 需要 | 是 |
| `bash hello.sh` | 不需要 | 否，直接由 `bash` 执行 |

> [!tip]
> 日常脚本建议同时保留 shebang 并设置执行权限，便于脚本被 crontab、systemd、CI/CD 或其他脚本直接调用。

---

# 3. Shebang 与执行环境

## 3.1 Shebang 的作用

shebang（也叫 **hashbang**）是脚本文件第一行的：

```bash
#!/bin/bash
```

其中 `#!` 就是 shebang。它的作用是告诉操作系统：“这个脚本应该由哪个解释器来执行”。例如在 `test.sh` 中写入：

```bash
#!/bin/bash

echo "hello"
```

然后执行：

```bash
chmod +x test.sh
./test.sh
```

Linux 实际会执行：

```bash
/bin/bash ./test.sh
```

即 shebang 指定了解释器。

> [!question]
> 为什么需要 shebang？

假设 `test.sh` 脚本中只有下面的内容：

```bash
echo "hello"
```

然后执行该脚本，但是系统不知道：这个文件是 Bash？还是 Python？还是 Perl？还是 Ruby？因此可能出现这种问题：

```text
Exec format error
```

或者由当前 Shell 进行解释。

## 3.2 常见 Shebang

- Bash

```bash
#!/bin/bash
```

- POSIX sh

```bash
#!/bin/sh
```

- Python

```python
#!/usr/bin/python3

print("hello")
```

执行：

```bash
./test.py
```

相当于：

```bash
python3 test.py
```

- Perl

```perl
#!/usr/bin/perl
```

- Ruby

```ruby
#!/usr/bin/ruby
```

## 3.3 Shebang 写法

```bash
#!/usr/bin/env bash
```

它表示用当前环境中找到的 `bash` 解释脚本，比固定写死 `/bin/bash` 更灵活。

| 写法 | 特点 |
|---|---|
| `#!/bin/bash` | 路径明确，但依赖 bash 固定在 `/bin/bash` |
| `#!/usr/bin/env bash` | 通过 `PATH` 查找 bash，跨环境更灵活 |
| `#!/bin/sh` | 只应使用 POSIX 语法，不应使用 Bash 专属语法 |

> [!note]
> 如果明确指定了解释器来执行脚本，比如 `bash script.sh`，此时 shebang 会被忽略。

## 3.4 判断当前 Shell

```bash
# 当前用户默认 Shell
echo "$SHELL"

# 系统可用 Shell
cat /etc/shells

# 当前 Bash 版本
bash --version
```

> [!warning]
> 如果脚本使用数组、`[[ ]]`、`(( ))`、`local` 等 Bash 语法，shebang 应写 `bash`，不要写 `sh`。

---

# 4. 脚本基本结构

## 4.1 推荐模板

```bash
#!/usr/bin/env bash
set -euo pipefail

readonly SCRIPT_DIR=$(
    cd "$(dirname "${BASH_SOURCE[0]}")"
    pwd
)

log() {
    printf '[%s] %s\n' "$(date '+%F %T')" "$*"
}

main() {
    log "start"
    log "script_dir=${SCRIPT_DIR}"
    log "done"
}

main "$@"
```

1. `set -euo pipefail`：这是 Bash 的**安全模式**（Strict Mode）：
    - `-e`：任何命令出错（返回值非 0），脚本立即停止执行。
    - `-u`：使用未定义的变量时直接报错，防止因空变量导致误删文件。
    - `-o pipefail`：管道命令中只要有一个子命令失败，整个管道就视为失败。

2. 获取脚本绝对路径
	- `readonly`：将变量设为只读，后续无法被修改。
	- `${BASH_SOURCE[0]}`：获取当前正在执行的脚本文件的路径。
	- `dirname`：提取出该脚本所在的目录。
	- `cd ... && pwd`：进入该目录并打印出绝对路径。
	- **总结**：无论你在哪个目录下执行这个脚本，`SCRIPT_DIR` 都能精准拿到该脚本文件**所在的绝对路径**。

3. 日志打印函数
	- 定义了一个名为 `log` 的函数。
	- `$(date '+%F %T')`：生成当前的时间戳，格式为 `年-月-日 时:分:秒`（例如 `2026-06-23 23:54:00`）。
	- `$*`：获取传给函数的所有参数（即日志内容）。
	- **效果**：调用 `log "hello"` 会输出 `[2026-06-23 23:54:00] hello`。

4. 主逻辑与执行
	- `main()`：将核心业务逻辑封装在 `main` 函数中，这是良好的编程习惯。
	- `main "$@"`：脚本真正的入口。`"$@"` 代表透传执行脚本时输入的所有外部参数。

> [!summary]
> 推荐把脚本写成“配置区 → 函数区 → main 入口”的结构，而不是从头到尾堆命令。

---

# 5. 变量与引用

## 5.1 定义变量

```bash
name="Tom"
age=18
readonly APP_HOME="/opt/myapp"
```

> [!error]
> **变量赋值时等号两侧不能有空格**。
>
> ```bash
> # 错误
> name = "Tom"
> ```

## 5.2 使用变量

```bash
echo "$name"

# 和 echo "$name" 没区别，但是更推荐，这种方式更安全、更规范
echo "${name}"

echo "name=${name}, age=${age}"
```

推荐始终使用双引号包裹变量：

```bash
rm -rf "${target_dir}"
```

> [!warning]
> 未加双引号的变量会发生单词分割和通配符展开。路径中包含空格、换行或 `*` 时，可能造成严重误操作。

## 5.3 变量作用域

```bash
global_name="app"

show_name() {
    local local_name="worker"
    echo "${global_name}:${local_name}"
}
```

| 类型 | 写法 | 作用范围 |
|---|---|---|
| 普通变量 | `name=value` | 当前 Shell |
| 只读变量 | `readonly name=value` | 当前 Shell，不可修改 |
| 局部变量 | `local name=value` | 函数内部 |
| 环境变量 | `export name=value` | 当前 Shell 及其子进程 |

---

# 6. 环境变量

## 6.1 查看环境变量

```bash
env
printenv
echo "$PATH"
echo "$HOME"
echo "$USER"
echo "$PWD"
```

## 6.2 导出环境变量

```bash
export APP_ENV="prod"
export APP_HOME="/opt/myapp"
```

子进程可读取已导出的变量：

```bash
bash -c 'echo "$APP_ENV"'
```

> [!info]
> `name=value` 只对当前 Shell 可见；`export name=value` 才能被子进程继承。

---

# 7. 字符串与命令替换

## 7.1 字符串操作

```bash
str="abcdef"

# 长度
echo "${#str}"

# 截取，从下标 1 开始取 3 个字符
echo "${str:1:3}"

# 拼接
prefix="app"
name="${prefix}-server"
```

## 7.2 命令替换

推荐使用 `$(...)`：

```bash
current_time=$(date '+%F %T')
current_user=$(whoami)
```

不推荐旧式反引号：

```bash
today=`date`
```

> [!tip]
> `$(...)` 可读性更好，也更适合嵌套命令替换。

---

# 8. 数学运算

## 8.1 算术展开

```bash
a=10
b=20
sum=$((a + b))
echo "${sum}"
```

## 8.2 自增与条件计算

```bash
i=0
((i++))

if (( i > 0 )); then
    echo "positive"
fi
```

| 写法 | 说明 |
|---|---|
| `$(( expression ))` | 返回计算结果 |
| `(( expression ))` | 执行算术判断或赋值 |
| `expr 1 + 2` | 旧写法，空格要求严格，不推荐新脚本使用 |

---

# 9. 数组

## 9.1 定义与访问

```bash
servers=("web01" "web02" "db01")

echo "${servers[0]}"
echo "${#servers[@]}"
```

## 9.2 遍历数组

```bash
for server in "${servers[@]}"; do
    echo "deploy ${server}"
done
```

> [!warning]
> 遍历数组时使用 `"${array[@]}"`，不要使用 `${array[*]}` 或未加引号的 `${array[@]}`，否则包含空格的元素会被拆开。

---

# 10. 位置参数与参数解析

## 10.1 特殊参数

| 参数 | 含义 |
|---|---|
| `$0` | 脚本名 |
| `$1`、`$2` | 第 1、2 个参数 |
| `$#` | 参数个数 |
| `$@` | 所有参数，推荐用 `"$@"` 保留参数边界 |
| `$*` | 所有参数，不推荐用于遍历 |
| `$?` | 上一条命令退出码 |
| `$$` | 当前 Shell 进程 PID |

## 10.2 参数检查

```bash
if [[ $# -lt 1 ]]; then
    echo "用法: $0 <env>"
    exit 2
fi

env_name="$1"
```

## 10.3 `case` 解析子命令

```bash
case "${1:-}" in
    start)
        echo "start"
        ;;
    stop)
        echo "stop"
        ;;
    restart)
        echo "restart"
        ;;
    *)
        echo "用法: $0 {start|stop|restart}"
        exit 2
        ;;
esac
```

> [!tip]
> 简单脚本用 `case` 足够；复杂参数如 `--env prod --force` 可再学习 `getopts`。

---

# 11. 条件判断

条件判断是 Shell 脚本逻辑控制的核心，Shell 通过 **命令的退出码**（`$?`）来判断条件真伪：`0` 为真（成功），非 `0` 为假（失败）。

## 11.1 三种判断写法

| 写法 | 说明 | 推荐程度 |
|---|---|---|
| `test expression` | 传统写法，本质是一个外部命令 | 可读性一般，不推荐新脚本使用 |
| `[ expression ]` | `test` 的别名，**`[` 后面和 `]` 前面必须有空格** | 兼容性好，POSIX 脚本中使用 |
| `[[ expression ]]` | Bash 内置关键字，功能更强 | **Bash 脚本推荐** |

### 11.1.1 `test` 写法

`test 条件` 会返回一个退出码：

- 0 条件成立（true）
- 1 条件不成立（false）

```bash
# 判断文件是否存在
if test -f /etc/passwd; then
    echo "文件存在"
fi
```

> `-f` 表示判断 `/etc/passwd` 是否为普通文件（regular file），等价于 `stat("/etc/passwd")`。

```bash
# 判断数值是否相等
if test "${a}" -eq "${b}"; then
    echo "a 等于 b"
fi
```

### 11.1.2 `[ ]` 单方括号写法

```bash
# 注意：方括号内侧必须有空格，否则报语法错误
if [ "${name}" = "admin" ]; then
    echo "管理员登录"
fi

# 判断目录是否存在
if [ -d /var/log ]; then
    echo "/var/log 目录存在"
fi

# 常见错误：缺少空格会导致语法错误
# [ "${name}" = "admin"]  ← 错误，缺少 ] 前的空格
```

### 11.1.3 `[[ ]]` 双方括号写法（推荐）

```bash
# [[ ]] 是 Bash 内置关键字，支持 &&、||、正则匹配等高级特性
if [[ "${name}" == "admin" ]]; then
    echo "管理员登录"
fi

# 支持通配符模式匹配（[ ] 不支持）
if [[ "${filename}" == *.log ]]; then
    echo "这是一个日志文件: ${filename}"
fi

# 支持正则匹配（使用 =~ 运算符）
if [[ "${email}" =~ ^[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\.[a-zA-Z]{2,}$ ]]; then
    echo "邮箱格式正确: ${email}"
fi
```

> [!info]
> `[[ ]]` 是 Bash 内置关键字，不需要对变量加双引号也能正确处理空值，且支持 `&&` / `||` 和模式匹配。**编写 Bash 脚本时优先使用 `[[ ]]`**。

## 11.2 数值比较

| 运算符 | 含义 | 英文全称 |
|---|---|---|
| `-eq` | 等于 | **eq**ual |
| `-ne` | 不等于 | **n**ot **e**qual |
| `-gt` | 大于 | **g**reater **t**han |
| `-lt` | 小于 | **l**ess **t**han |
| `-ge` | 大于等于 | **g**reater or **e**qual |
| `-le` | 小于等于 | **l**ess or **e**qual |

> [!warning]
> 数值比较只能用 `-eq` / `-ne` / `-gt` / `-lt` / `-ge` / `-le`，不能用 `==` / `!=` / `>` / `<`。字符串比较才用 `==` / `!=`。

### 11.2.1 各运算符示例

```bash
age=25

# -eq：等于
if [[ "${age}" -eq 25 ]]; then
    echo "年龄是 25 岁"
fi

# -ne：不等于
if [[ "${age}" -ne 30 ]]; then
    echo "年龄不是 30 岁"
fi

# -gt：大于
if [[ "${age}" -gt 18 ]]; then
    echo "已成年（大于 18 岁）"
fi

# -lt：小于
if [[ "${age}" -lt 60 ]]; then
    echo "未到退休年龄（小于 60 岁）"
fi

# -ge：大于等于
if [[ "${age}" -ge 18 ]]; then
    echo "已成年（大于等于 18 岁）"
fi

# -le：小于等于
if [[ "${age}" -le 65 ]]; then
    echo "未到退休年龄（小于等于 65 岁）"
fi
```

### 11.2.2 结合 (( )) 算术判断

```bash
# (( )) 支持 C 语言风格的数值比较，可读性更好
count=10

if (( count > 5 )); then
    echo "count 大于 5"
fi

if (( count <= 10 && count >= 0 )); then
    echo "count 在 0~10 之间"
fi

# 适合计算型比较
if (( count % 2 == 0 )); then
    echo "count 是偶数"
fi
```

> [!tip]
> 纯数值比较用 `(( ))` 更直观，字符串 / 文件判断用 `[[ ]]`。两者可混用。

## 11.3 字符串比较

| 运算符 | 含义 | 说明 |
|---|---|---|
| `==` 或 `=` | 等于 | `[[ ]]` 中推荐 `==`，`[ ]` 中只能用 `=` |
| `!=` | 不等于 | |
| `-z` | 长度为 0（空字符串） | **z**ero length |
| `-n` | 长度非 0（非空字符串） | **n**on-zero length |
| `>` | 字典序大于 | `[[ ]]` 中直接使用，`[ ]` 中需转义为 `\>` |
| `<` | 字典序小于 | `[[ ]]` 中直接使用，`[ ]` 中需转义为 `\<` |
| `=~` | 正则匹配 | **仅 `[[ ]]` 支持** |

### 11.3.1 相等与不等

```bash
str1="hello"
str2="world"

# 等于
if [[ "${str1}" == "hello" ]]; then
    echo "str1 是 hello"
fi

# 不等于
if [[ "${str1}" != "${str2}" ]]; then
    echo "str1 和 str2 不相等"
fi

# 在 [ ] 中只能用 =（单等号）
if [ "${str1}" = "hello" ]; then
    echo "POSIX 兼容写法"
fi
```

### 11.3.2 空值判断

```bash
empty_var=""
non_empty="hello"

# -z：判断是否为空
if [[ -z "${empty_var}" ]]; then
    echo "变量为空或未定义"
fi

# -n：判断是否非空
if [[ -n "${non_empty}" ]]; then
    echo "变量非空，值为: ${non_empty}"
fi

# 常见用途：检查函数参数是否传递
check_param() {
    if [[ -z "${1:-}" ]]; then
        echo "错误：缺少参数" >&2
        return 1
    fi
    echo "参数: $1"
}
```

### 11.3.3 字典序比较

```bash
a="apple"
b="banana"

# apple < banana（字典序）
if [[ "${a}" < "${b}" ]]; then
    echo "${a} 在 ${b} 前面"
fi

# 在 [ ] 中需要转义
if [ "${a}" \< "${b}" ]; then
    echo "POSIX 兼容写法"
fi
```

### 11.3.4 正则匹配（=~）

```bash
phone="13812345678"

# 匹配中国大陆手机号
if [[ "${phone}" =~ ^1[3-9][0-9]{9}$ ]]; then
    echo "手机号格式正确: ${phone}"
fi

# 捕获匹配组（通过 BASH_REMATCH 数组）
version="v3.2.1"
if [[ "${version}" =~ ^v([0-9]+)\.([0-9]+)\.([0-9]+)$ ]]; then
    echo "主版本: ${BASH_REMATCH[1]}"
    echo "次版本: ${BASH_REMATCH[2]}"
    echo "补丁版本: ${BASH_REMATCH[3]}"
fi

# 判断字符串是否以指定前缀开头
if [[ "${filename}" =~ ^backup_ ]]; then
    echo "这是一个备份文件"
fi
```

### 11.3.5 通配符模式匹配

```bash
filename="app.log"

# 匹配后缀
if [[ "${filename}" == *.log ]]; then
    echo "日志文件"
fi

# 匹配前缀
if [[ "${filename}" == app.* ]]; then
    echo "app 相关文件"
fi

# 匹配中间
if [[ "${path}" == */logs/* ]]; then
    echo "路径包含 logs 目录"
fi
```

## 11.4 文件判断

| 表达式 | 含义 | 记忆技巧 |
|---|---|---|
| `-e file` | 文件/目录 **存在** | **e**xists |
| `-f file` | 存在且为 **普通文件** | **f**ile |
| `-d dir` | 存在且为 **目录** | **d**irectory |
| `-L path` | 存在且为 **符号链接** | **L**ink |
| `-r file` | 存在且 **可读** | **r**eadable |
| `-w file` | 存在且 **可写** | **w**ritable |
| `-x file` | 存在且 **可执行** | e**x**ecutable |
| `-s file` | 存在且 **大小大于 0** | **s**ize > 0 |
| `-h file` | 同 `-L`，符号链接 | symlink |
| `-p file` | 命名管道（FIFO） | **p**ipe |
| `-S file` | Socket 文件 | **S**ocket |
| `-b file` | 块设备文件 | **b**lock device |
| `-c file` | 字符设备文件 | **c**haracter device |
| `-g file` | 设置了 SGID 位 | set**g**id |
| `-u file` | 设置了 SUID 位 | set**u**id |
| `-k file` | 设置了 sticky 位 | stic**k**y bit |
| `file1 -nt file2` | file1 比 file2 **新** | **n**ewer **t**han |
| `file1 -ot file2` | file1 比 file2 **旧** | **o**lder **t**han |
| `file1 -ef file2` | 两个文件指向同一 inode（硬链接/同一文件） | same **e**ntry **f**ile |

### 11.4.1 常用文件判断示例

```bash
config_file="/etc/myapp/config.conf"

# -e：文件或目录是否存在
if [[ -e "${config_file}" ]]; then
    echo "配置文件存在"
else
    echo "配置文件不存在"
fi

# -f：是否为普通文件
if [[ -f "${config_file}" ]]; then
    echo "这是一个普通文件"
fi

# -d：是否为目录
if [[ -d /var/log ]]; then
    echo "/var/log 是一个目录"
fi

# -L：是否为符号链接
if [[ -L /usr/bin/python ]]; then
    target=$(readlink -f /usr/bin/python)
    echo "python 是符号链接，指向: ${target}"
fi

# -s：文件存在且非空
if [[ -s "${config_file}" ]]; then
    echo "配置文件非空"
else
    echo "配置文件为空或不存在"
fi
```

### 11.4.2 权限判断示例

```bash
script="deploy.sh"

# -r：可读
if [[ -r "${script}" ]]; then
    echo "脚本可读"
fi

# -w：可写
if [[ -w "${script}" ]]; then
    echo "脚本可写"
fi

# -x：可执行
if [[ -x "${script}" ]]; then
    echo "脚本可执行"
else
    echo "脚本不可执行，尝试添加执行权限"
    chmod +x "${script}"
fi
```

### 11.4.3 文件比较示例

```bash
old_file="/tmp/backup_old.tar.gz"
new_file="/tmp/backup_new.tar.gz"

# -nt：newer than，判断是否更新
if [[ "${new_file}" -nt "${old_file}" ]]; then
    echo "新备份比旧备份更新"
fi

# -ot：older than，判断是否更旧
if [[ "${old_file}" -ot "${new_file}" ]]; then
    echo "旧备份比新备份更旧"
fi

# -ef：检查是否为同一文件（硬链接或同一 inode）
if [[ "${new_file}" -ef "${old_file}" ]]; then
    echo "两个文件是同一个文件"
fi
```

### 11.4.4 文件检查完整示例

```bash
# 典型的配置文件检查流程
check_config() {
    local config_file="$1"

    if [[ ! -e "${config_file}" ]]; then
        echo "错误：配置文件 ${config_file} 不存在" >&2
        return 1
    fi

    if [[ ! -f "${config_file}" ]]; then
        echo "错误：${config_file} 不是普通文件" >&2
        return 1
    fi

    if [[ ! -r "${config_file}" ]]; then
        echo "错误：${config_file} 不可读" >&2
        return 1
    fi

    if [[ ! -s "${config_file}" ]]; then
        echo "警告：${config_file} 是空文件" >&2
    fi

    echo "配置文件检查通过: ${config_file}"
    return 0
}

check_config "/etc/myapp/config.conf"
```

## 11.5 逻辑运算符

| 写法 | `[[ ]]` 中 | `[ ]` / `test` 中 | 说明 |
|---|---|---|---|
| 与（AND） | `&&` | `-a` | 所有条件为真 |
| 或（OR） | `\|\|` | `-o` | 任一条件为真 |
| 非（NOT） | `!` | `!` | 取反 |

### 11.5.1 `[[ ]]` 中组合条件（推荐）

```bash
age=25
role="admin"

# AND：年龄大于 18 且角色是 admin
if [[ "${age}" -gt 18 && "${role}" == "admin" ]]; then
    echo "成年管理员"
fi

# OR：角色是 admin 或 root
if [[ "${role}" == "admin" || "${role}" == "root" ]]; then
    echo "拥有管理权限"
fi

# NOT：不是游客
if [[ ! "${role}" == "guest" ]]; then
    echo "非游客用户"
fi

# 复杂组合：使用括号分组
if [[ ("${role}" == "admin" || "${role}" == "root") && "${age}" -ge 18 ]]; then
    echo "成年管理员"
fi
```

### 11.5.2 `[ ]` 中组合条件

```bash
# [ ] 中用 -a 表示 AND，-o 表示 OR
if [ "${age}" -gt 18 -a "${role}" = "admin" ]; then
    echo "成年管理员"
fi

if [ "${role}" = "admin" -o "${role}" = "root" ]; then
    echo "拥有管理权限"
fi

# 也可以用多个 [ ] 配合 && / ||（在命令层面）
if [ "${age}" -gt 18 ] && [ "${role}" = "admin" ]; then
    echo "成年管理员"
fi
```

> [!warning]
> `[ ]` 中 `-a` / `-o` 的可读性较差，且存在一些边界情况。**在 Bash 中优先使用 `[[ ]]` 配合 `&&` / `||`**。

## 11.6 实战组合示例

### 11.6.1 输入验证

```bash
validate_port() {
    local port="$1"

    # 非空检查
    if [[ -z "${port}" ]]; then
        echo "错误：端口号不能为空" >&2
        return 1
    fi

    # 数值范围检查
    if [[ ! "${port}" =~ ^[0-9]+$ ]]; then
        echo "错误：端口号必须是数字" >&2
        return 1
    fi

    if (( port < 1 || port > 65535 )); then
        echo "错误：端口号范围 1-65535" >&2
        return 1
    fi

    echo "端口号 ${port} 验证通过"
    return 0
}

validate_port "8080"
validate_port "99999"
```

### 11.6.2 环境前置检查

```bash
# 典型的脚本启动前环境检查
preflight_check() {
    local errors=0

    # 检查必需命令是否存在
    for cmd in docker kubectl jq; do
        if ! command -v "${cmd}" &>/dev/null; then
            echo "错误：缺少命令 ${cmd}" >&2
            ((errors++))
        fi
    done

    # 检查必需目录是否存在
    for dir in /var/log/app /etc/app; do
        if [[ ! -d "${dir}" ]]; then
            echo "错误：目录 ${dir} 不存在" >&2
            ((errors++))
        fi
    done

    # 检查必需环境变量
    if [[ -z "${DATABASE_URL:-}" ]]; then
        echo "错误：环境变量 DATABASE_URL 未设置" >&2
        ((errors++))
    fi

    if (( errors > 0 )); then
        echo "前置检查失败，共 ${errors} 个错误" >&2
        return 1
    fi

    echo "前置检查通过"
    return 0
}
```

### 11.6.3 多分支条件判断

```bash
# 根据文件类型执行不同操作
process_path() {
    local path="$1"

    if [[ ! -e "${path}" ]]; then
        echo "错误：路径不存在: ${path}" >&2
        return 1
    fi

    if [[ -L "${path}" ]]; then
        echo "符号链接 → $(readlink -f "${path}")"
    elif [[ -d "${path}" ]]; then
        echo "目录，包含 $(ls -1 "${path}" | wc -l) 个项目"
    elif [[ -f "${path}" && -x "${path}" ]]; then
        echo "可执行文件，大小 $(stat -c%s "${path}" 2>/dev/null || stat -f%z "${path}") 字节"
    elif [[ -f "${path}" ]]; then
        echo "普通文件"
    else
        echo "其他类型"
    fi
}
```

> [!summary]
> - **条件判断的本质**：Shell 通过退出码 `0`（真）/ `非0`（假）判断条件
> - **写法选择**：Bash 脚本优先使用 `[[ ]]`，需要 POSIX 兼容时使用 `[ ]`
> - **数值比较**：用 `-eq`/`-gt` 等运算符或 `(( ))` 算术判断
> - **字符串比较**：`==`/`!=`/`-z`/`-n`，`[[ ]]` 额外支持 `=~` 正则匹配和通配符
> - **文件判断**：`-e`/`-f`/`-d` 是最高频的三个，`-s` 检查非空文件很实用
> - **逻辑组合**：`[[ ]]` 用 `&&`/`||`/`!`，`[ ]` 用 `-a`/`-o`/`!`
> - **记忆技巧**：运算符名即英文缩写（eq=equal, gt=greater than, d=directory, etc.）

---

# 12. 分支与循环

## 12.1 `if`

```bash
if [[ "${env_name}" == "prod" ]]; then
    echo "production"
elif [[ "${env_name}" == "test" ]]; then
    echo "test"
else
    echo "unknown"
fi
```

## 12.2 `for`

```bash
for file in ./*.log; do
    [[ -e "${file}" ]] || continue
    echo "处理 ${file}"
done
```

C 风格循环：

```bash
for ((i = 0; i < 10; i++)); do
    echo "${i}"
done
```

## 12.3 `while`

```bash
i=0
while [[ "${i}" -lt 5 ]]; do
    echo "${i}"
    ((i++))
done
```

逐行读取文件：

```bash
while IFS= read -r line; do
    echo "line=${line}"
done < input.txt
```

> [!warning]
> 读取文件行时使用 `IFS= read -r`，避免反斜杠被转义、前后空白被意外裁剪。

---

# 13. 函数

## 13.1 定义与调用

```bash
log_info() {
    printf '[INFO] %s\n' "$*"
}

log_info "deploy start"
```

## 13.2 参数与返回

```bash
sum() {
    local a="$1"
    local b="$2"
    echo $((a + b))
}

result=$(sum 10 20)
echo "${result}"
```

`return` 只能返回 `0` 到 `255` 的退出码，不适合返回字符串或大整数。

```bash
check_file() {
    local file="$1"
    [[ -f "${file}" ]]
}

if check_file "/etc/passwd"; then
    echo "exists"
fi
```

> [!summary]
> 函数用 `echo/printf` 输出业务结果，用 `return` 或最后一条命令退出码表示成功失败。

---

# 14. 输入、输出与重定向

## 14.1 读取输入

```bash
read -r name
read -r -p "Input name: " name
read -r -s -p "Password: " password
echo
```

## 14.2 标准输出与标准错误

```bash
echo "normal message"
echo "error message" >&2
```

## 14.3 重定向

| 写法 | 含义 |
|---|---|
| `cmd > file` | stdout 覆盖写入 |
| `cmd >> file` | stdout 追加写入 |
| `cmd 2> err.log` | stderr 写入 |
| `cmd > all.log 2>&1` | stdout 和 stderr 都写入 |
| `cmd &> all.log` | Bash 简写，stdout 和 stderr 都写入 |
| `cmd < input.txt` | 从文件读取 stdin |

> [!warning]
> 重定向顺序很重要。`cmd > all.log 2>&1` 和 `cmd 2>&1 > all.log` 的结果不同。

---

# 15. 管道与文本处理

## 15.1 管道

管道将左侧命令的标准输出作为右侧命令的标准输入。

```bash
ps aux | grep "[n]ginx"
```

## 15.2 grep、sed、awk

```bash
# 搜索错误日志
grep -iE "error|fail|timeout" app.log

# 替换文本，输出到 stdout
sed 's/old/new/g' config.txt

# 原地替换并备份
sed -i.bak 's/old/new/g' config.txt

# 打印第一列
awk '{print $1}' access.log

# 按第一列统计出现次数
awk '{print $1}' access.log | sort | uniq -c | sort -nr | head
```

> [!info]
> grep、sed、awk 的深入用法可分别放在专题笔记中；脚本中重点掌握“搜索、替换、抽列、统计”四类组合。

---

# 16. 错误处理

## 16.1 严格模式

```bash
set -euo pipefail
```

| 选项 | 含义 |
|---|---|
| `-e` | 命令失败时退出 |
| `-u` | 使用未定义变量时报错 |
| `-o pipefail` | 管道中任一命令失败，整个管道失败 |

## 16.2 主动失败

```bash
die() {
    echo "[ERROR] $*" >&2
    exit 1
}

[[ -f "${config_file}" ]] || die "配置文件不存在: ${config_file}"
```

## 16.3 可预期失败

`set -e` 下，可预期失败要显式处理：

```bash
if grep -q "ready" app.log; then
    echo "ready"
else
    echo "not ready"
fi
```

不要写成：

```bash
grep -q "ready" app.log
echo "ready"
```

---

# 17. trap 与清理

## 17.1 退出时清理临时文件

```bash
tmp_file=$(mktemp)

cleanup() {
    rm -f "${tmp_file}"
}

trap cleanup EXIT
```

## 17.2 捕获中断

```bash
on_interrupt() {
    echo "interrupted" >&2
    exit 130
}

trap on_interrupt INT TERM
```

> [!tip]
> `EXIT` 适合清理临时文件，`INT` 通常来自 `Ctrl+C`，`TERM` 常来自 systemd、Docker 或 Kubernetes 的终止信号。

---

# 18. 调试脚本

## 18.1 执行跟踪

```bash
bash -x script.sh
```

局部开启：

```bash
set -x
command_a
command_b
set +x
```

## 18.2 语法检查

```bash
bash -n script.sh
```

## 18.3 ShellCheck

```bash
shellcheck script.sh
```

> [!summary]
> 调试顺序：先 `bash -n` 查语法，再 `shellcheck` 查隐患，最后 `bash -x` 追踪运行路径。

---

# 19. 脚本目录与路径安全

## 19.1 获取脚本目录

```bash
readonly SCRIPT_DIR=$(
    cd "$(dirname "${BASH_SOURCE[0]}")"
    pwd
)
```

无论从哪个目录执行脚本，都能得到脚本文件所在目录。

## 19.2 路径安全检查

删除、移动、覆盖前先确认目标不为空，并在预期目录内。

```bash
target_dir="${1:-}"

if [[ -z "${target_dir}" || "${target_dir}" == "/" ]]; then
    echo "危险路径: ${target_dir}" >&2
    exit 1
fi
```

> [!warning]
> 脚本中执行 `rm -rf "${var}"` 前必须检查变量是否为空、是否为根目录、是否在预期工作目录内。

---

# 20. 实战脚本：日志清理

```bash
#!/usr/bin/env bash
set -euo pipefail

readonly LOG_DIR="${1:-/var/log/myapp}"
readonly DAYS="${2:-14}"

log() {
    printf '[%s] %s\n' "$(date '+%F %T')" "$*"
}

main() {
    if [[ ! -d "${LOG_DIR}" ]]; then
        echo "日志目录不存在: ${LOG_DIR}" >&2
        exit 1
    fi

    log "preview files older than ${DAYS} days in ${LOG_DIR}"
    find "${LOG_DIR}" -type f -name '*.log' -mtime +"${DAYS}" -print

    read -r -p "确认删除以上文件？[y/N] " answer
    if [[ "${answer}" != "y" && "${answer}" != "Y" ]]; then
        log "cancelled"
        exit 0
    fi

    find "${LOG_DIR}" -type f -name '*.log' -mtime +"${DAYS}" -delete
    log "done"
}

main "$@"
```

> [!tip]
> 高风险脚本采用“先预览，再确认，再执行”的流程，比直接删除更适合生产环境。

---

# 21. 实战脚本：服务健康检查

```bash
#!/usr/bin/env bash
set -euo pipefail

readonly SERVICE_NAME="${1:-nginx}"
readonly HEALTH_URL="${2:-http://127.0.0.1/}"

die() {
    echo "[ERROR] $*" >&2
    exit 1
}

main() {
    systemctl is-active --quiet "${SERVICE_NAME}" \
        || die "service not active: ${SERVICE_NAME}"

    curl -fsS --max-time 5 "${HEALTH_URL}" >/dev/null \
        || die "health check failed: ${HEALTH_URL}"

    echo "[OK] ${SERVICE_NAME} healthy"
}

main "$@"
```

适合接入：

- 发布后验证。
- systemd `ExecStartPost`。
- CI/CD 部署流水线。
- crontab 定时巡检。

---

# 22. 脚本规范

| 规则 | 推荐写法 |
|---|---|
| 解释器 | `#!/usr/bin/env bash` |
| 严格模式 | `set -euo pipefail` |
| 变量引用 | `"${var}"` |
| 常量 | `readonly APP_HOME="/opt/app"` |
| 函数名 | `snake_case` |
| 入口 | `main "$@"` |
| 日志 | 统一 `log_info`、`log_warn`、`log_error` |
| 错误 | 使用 `die "message"` |
| 临时文件 | `mktemp` + `trap cleanup EXIT` |
| 删除操作 | 先预览，再确认，再执行 |

> [!warning]
> 不要在脚本中默认假设当前目录。使用绝对路径、`SCRIPT_DIR` 或显式 `cd` 到工作目录。

---

# 23. 学习路线

## 23.1 生存期

先掌握：

- shebang 与执行方式。
- 变量和双引号。
- `if`、`case`、`for`、`while`。
- 函数和 `main "$@"`。
- `set -euo pipefail`。

## 23.2 效率期

继续学习：

- grep、sed、awk。
- find、xargs。
- rsync、ssh、scp。
- crontab、systemd timer。
- ShellCheck。

## 23.3 工程期

开始沉淀：

- 部署脚本。
- 健康检查脚本。
- 日志清理脚本。
- 备份与恢复脚本。
- 巡检脚本。
- 事故处理 Runbook。

---

# 24. 易错点总结

> [!summary]
> 本篇核心：Shell 脚本不是把命令简单堆起来，而是把命令组织成可重复、可检查、可回滚、可维护的自动化流程。

- 变量赋值等号两边不能有空格。
- 变量引用默认使用 `"${var}"`。
- Bash 专属语法不要放在 `#!/bin/sh` 脚本里。
- 遍历参数用 `"$@"`，遍历数组用 `"${array[@]}"`。
- `return` 返回的是退出码，不是业务结果。
- `set -e` 不能替代错误设计，可预期失败要显式处理。
- 删除、覆盖、迁移前要校验路径并尽量先预览。
