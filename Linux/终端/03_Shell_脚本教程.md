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

## 1.1 Shell 与 Shell 脚本

**Shell** 是用户与 Linux 系统交互的命令解释器，负责解析命令、展开变量、处理管道与重定向，并启动外部程序。

**Shell 脚本** 是把一组命令、变量、判断、循环和函数保存到文件中，使操作可以被重复执行、自动执行和稳定交付。

| 类型 | 主要用途 | 典型例子 |
|---|---|---|
| 交互式 Shell | 人手动输入命令 | `cd`、`grep`、`systemctl status nginx` |
| Shell 脚本 | 自动化一组操作 | 部署、备份、巡检、日志清理、健康检查 |
| Shell 配置 | 定制终端行为 | `~/.bashrc`、`~/.zshrc`、alias、prompt |

> [!summary]
> 交互式 Shell 追求输入效率；Shell 脚本追求**可重复、可检查、可维护、可失败恢复**。

## 1.2 常见 Shell 的选择

| Shell | 说明 | 适用场景 |
|---|---|---|
| `sh` | POSIX Shell，语法最小集 | 跨发行版、跨 Unix 的兼容脚本 |
| `bash` | Linux 最常见的增强 Shell | 运维自动化、部署脚本、巡检脚本 |
| `zsh` | 交互体验强，插件生态丰富 | 个人终端环境 |
| `fish` | 默认体验好，语法更现代 | 个人交互式 Shell |

本文默认讨论 **Bash 脚本**。如果脚本使用数组、`[[ ]]`、`(( ))`、`local`、`${BASH_SOURCE[0]}` 等语法，shebang 必须指向 Bash，而不是 `sh`。

## 1.3 脚本的工程边界

Shell 适合把已有命令编排起来，不适合承担复杂业务系统。

| 适合 Shell | 不适合 Shell |
|---|---|
| 调用系统命令 | 复杂数据结构 |
| 文件批处理 | 大型业务逻辑 |
| 启停服务 | 高并发网络服务 |
| 部署、备份、巡检 | 复杂 JSON/XML 转换 |
| 简单文本处理 | 需要严格单元测试的大型程序 |

> [!tip]
> 判断标准：如果脚本主要是在调用 `find`、`grep`、`sed`、`awk`、`ssh`、`rsync`、`systemctl`，Shell 很合适；如果开始写大量嵌套条件、复杂状态机和字符串解析，应考虑 Python、Go 或专用工具。

---

# 2. 执行环境与脚本入口

## 2.1 第一个脚本

创建 `hello.sh`：

```bash
#!/usr/bin/env bash

echo "Hello, Shell"
```

执行方式：

```bash
chmod +x hello.sh
./hello.sh

bash hello.sh
```

| 运行方式 | 是否需要执行权限 | 是否读取 shebang |
|---|---|---|
| `./hello.sh` | 需要 | 是 |
| `bash hello.sh` | 不需要 | 否，由当前命令指定解释器 |
| `source hello.sh` | 不需要 | 否，在当前 Shell 中执行 |

> [!warning]
> `source script.sh` 会在当前 Shell 中执行脚本，脚本里的变量、`cd`、函数定义会影响当前终端。普通脚本通常使用 `./script.sh` 或 `bash script.sh`。

## 2.2 Shebang 的作用

shebang 是脚本第一行的解释器声明：

```bash
#!/usr/bin/env bash
```

当执行 `./script.sh` 时，Linux 内核会根据 shebang 找到解释器，相当于执行：

```bash
/usr/bin/env bash ./script.sh
```

常见写法：

| 写法 | 特点 |
|---|---|
| `#!/usr/bin/env bash` | 从 `PATH` 中查找 Bash，跨环境更灵活 |
| `#!/bin/bash` | 路径明确，但依赖 Bash 位于 `/bin/bash` |
| `#!/bin/sh` | 只能使用 POSIX Shell 语法 |

> [!warning]
> `#!/bin/sh` 在不同系统上可能指向 `dash`、`bash` 或其他 POSIX Shell。不要在 `sh` 脚本中使用 Bash 专属语法。

## 2.3 推荐脚本模板

```bash
#!/usr/bin/env bash
set -euo pipefail

readonly SCRIPT_DIR="$(
    cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd -P
)"

log_info() {
    printf '[%s] [INFO] %s\n' "$(date '+%F %T')" "$*"
}

die() {
    printf '[%s] [ERROR] %s\n' "$(date '+%F %T')" "$*" >&2
    exit 1
}

main() {
    log_info "script_dir=${SCRIPT_DIR}"
    log_info "start"
    log_info "done"
}

main "$@"
```

这份模板包含四个关键设计：

| 设计 | 目的 |
|---|---|
| `set -euo pipefail` | 尽早暴露失败、未定义变量和管道错误 |
| `SCRIPT_DIR` | 让脚本能稳定找到自身所在目录 |
| `log_info` / `die` | 统一正常日志与错误日志 |
| `main "$@"` | 明确入口，并保留所有命令行参数边界 |

## 2.4 严格模式

`set -euo pipefail` 用来把 Bash 默认的宽松行为改成更适合自动化脚本的严格行为。

| 选项 | 含义 | 解决的问题 |
|---|---|---|
| `-e` | 普通命令返回非 0 时退出 | 防止失败后继续执行 |
| `-u` | 使用未定义变量时报错 | 防止空变量参与路径拼接 |
| `-o pipefail` | 管道中任意命令失败则整个管道失败 | 防止只看最后一个命令的退出码 |

示例：

```bash
set -euo pipefail

cd /not/exist
rm -rf ./*
```

`cd` 失败后脚本会立即退出，不会继续执行后面的删除命令。

管道失败示例：

```bash
set -o pipefail

cat missing.log | sort > sorted.log
```

没有 `pipefail` 时，`sort` 可能成功退出，整个管道被视为成功；开启后，`cat missing.log` 失败会使整个管道失败。

> [!warning]
> `set -e` 不是错误处理设计的替代品。放在 `if`、`while`、`until`、`&&`、`||` 等条件位置的命令，失败通常不会直接退出；可预期失败必须显式处理。

## 2.5 获取脚本所在目录

```bash
readonly SCRIPT_DIR="$(
    cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd -P
)"
```

拆解如下：

| 片段 | 含义 |
|---|---|
| `${BASH_SOURCE[0]}` | 当前脚本文件路径，可能是相对路径 |
| `dirname -- ...` | 取出脚本所在目录 |
| `cd -- ... && pwd -P` | 进入该目录并输出物理绝对路径 |
| `$()` | 将命令输出赋值给变量 |
| `readonly` | 防止后续代码意外修改 |

用途示例：

```bash
readonly PROJECT_DIR="$(dirname -- "${SCRIPT_DIR}")"
readonly CONFIG_FILE="${PROJECT_DIR}/config/app.conf"
```

这样无论从哪个目录执行脚本，都能稳定定位项目内的配置文件。

---

# 3. 变量、引用与数据结构

## 3.1 变量定义

```bash
name="Tom"
age=18
readonly APP_HOME="/opt/myapp"
```

规则：

| 规则 | 正确写法 | 错误写法 |
|---|---|---|
| 等号两侧不能有空格 | `name="Tom"` | `name = "Tom"` |
| 变量名区分大小写 | `APP_HOME` | `app_home` 不是同一个变量 |
| 常量用 `readonly` | `readonly ENV="prod"` | 后续随意覆盖 |

## 3.2 变量引用

推荐写法：

```bash
echo "${name}"
echo "name=${name}, age=${age}"
rm -rf -- "${target_dir}"
```

`"${var}"` 同时解决两个问题：

| 写法 | 作用 |
|---|---|
| `"..."` | 防止单词分割和通配符展开 |
| `${...}` | 明确变量名边界 |

变量边界示例：

```bash
name="app"

echo "$name_log"      # 读取 name_log 变量
echo "${name}_log"    # 输出 app_log
```

> [!warning]
> 在脚本中使用路径变量时，默认写成 `"${var}"`。未加引号的变量遇到空格、换行、`*`、`?` 时可能被拆分或展开，导致误删、误移动、误覆盖。

## 3.3 单引号、双引号与不加引号

| 写法 | 是否展开变量 | 是否执行命令替换 | 典型用途 |
|---|---|---|---|
| `'text $HOME'` | 否 | 否 | 原样字符串 |
| `"text $HOME"` | 是 | 是 | 含变量的字符串 |
| `text` | 是 | 是 | 简单字面量，不建议放变量 |

```bash
name="Tom"

echo 'hello ${name}'   # hello ${name}
echo "hello ${name}"   # hello Tom
```

## 3.4 环境变量

普通变量只在当前 Shell 中可见；环境变量会传递给子进程。

```bash
APP_ENV="prod"
export APP_ENV

bash -c 'echo "${APP_ENV}"'
```

常见环境变量：

| 变量 | 含义 |
|---|---|
| `PATH` | 命令搜索路径 |
| `HOME` | 当前用户主目录 |
| `USER` | 当前用户名 |
| `PWD` | 当前工作目录 |
| `SHELL` | 用户默认 Shell |

## 3.5 字符串操作

```bash
str="abcdef"

echo "${#str}"       # 长度：6
echo "${str:1:3}"    # 从下标 1 开始取 3 个字符：bcd
echo "${str/abc/ABC}" # 替换第一个匹配：ABCdef
```

常用场景：

```bash
file="/var/log/app.log"

echo "${file##*/}"   # app.log，删除最长路径前缀
echo "${file%.*}"    # /var/log/app，删除最短扩展名后缀
echo "${file##*.}"   # log，提取扩展名
```

## 3.6 命令替换

推荐使用 `$(...)`：

```bash
current_time="$(date '+%F %T')"
current_user="$(whoami)"
```

不推荐旧式反引号：

```bash
today=`date`
```

> [!tip]
> `$(...)` 更易读，也更适合嵌套。命令替换结果通常也要放在双引号中，避免输出被二次拆分。

## 3.7 算术运算

```bash
a=10
b=20

sum=$((a + b))
echo "${sum}"

if (( sum > 20 )); then
    echo "large"
fi
```

| 写法 | 作用 |
|---|---|
| `$(( expression ))` | 返回计算结果 |
| `(( expression ))` | 执行算术判断或赋值 |
| `expr 1 + 2` | 旧写法，不推荐新脚本使用 |

## 3.8 数组

Bash 数组适合保存一组参数或目标。

```bash
servers=("web01" "web02" "db01")

echo "${servers[0]}"
echo "${#servers[@]}"

for server in "${servers[@]}"; do
    echo "deploy ${server}"
done
```

> [!warning]
> 遍历数组用 `"${array[@]}"`。不要用未加引号的 `${array[@]}`，也不要把 `"${array[*]}"` 当成逐项遍历，它们会破坏元素边界。

---

# 4. 参数、函数与返回值

## 4.1 位置参数

| 参数 | 含义 |
|---|---|
| `$0` | 脚本名 |
| `$1`、`$2` | 第 1、2 个参数 |
| `$#` | 参数个数 |
| `"$@"` | 所有参数，保留每个参数边界 |
| `"$*"` | 所有参数合并为一个字符串 |
| `$?` | 上一条命令退出码 |
| `$$` | 当前 Shell 进程 PID |

参数检查：

```bash
if [[ $# -lt 1 ]]; then
    echo "用法: $0 <env>" >&2
    exit 2
fi

env_name="$1"
```

## 4.2 默认值与必填参数

```bash
env_name="${1:-dev}"                 # 没传则使用 dev
config_file="${CONFIG_FILE:-app.conf}" # 环境变量未设置则使用 app.conf
```

必填参数：

```bash
: "${DEPLOY_ENV:?必须设置 DEPLOY_ENV}"
```

> [!info]
> 在开启 `set -u` 后，访问未定义变量会报错。读取可选变量时应写成 `${var:-}` 或 `${var:-default}`。

## 4.3 `case` 解析子命令

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
        echo "用法: $0 {start|stop|restart}" >&2
        exit 2
        ;;
esac
```

`case` 适合解析固定子命令，比多层 `if/elif` 更清晰。

## 4.4 `getopts` 解析短选项

```bash
force=0
env_name="dev"

while getopts ":e:f" opt; do
    case "${opt}" in
        e)
            env_name="${OPTARG}"
            ;;
        f)
            force=1
            ;;
        :)
            echo "选项 -${OPTARG} 缺少参数" >&2
            exit 2
            ;;
        \?)
            echo "未知选项: -${OPTARG}" >&2
            exit 2
            ;;
    esac
done

shift $((OPTIND - 1))
```

执行示例：

```bash
./deploy.sh -e prod -f
```

> [!tip]
> `getopts` 只处理短选项，如 `-e prod`、`-f`。复杂长选项如 `--env prod --force` 可以手写 `while case`，或改用更适合参数解析的语言。

## 4.5 函数定义与调用

```bash
log_info() {
    printf '[INFO] %s\n' "$*"
}

log_info "deploy start"
```

函数参数同样使用 `$1`、`$2`、`$@`：

```bash
sum() {
    local a="$1"
    local b="$2"
    printf '%s\n' "$((a + b))"
}

result="$(sum 10 20)"
echo "${result}"
```

## 4.6 函数返回值

`return` 返回的是退出码，只适合表示成功或失败。

```bash
check_file() {
    local file="$1"
    [[ -f "${file}" ]]
}

if check_file "/etc/passwd"; then
    echo "exists"
fi
```

| 需求 | 推荐方式 |
|---|---|
| 返回业务文本 | `printf` / `echo` 输出，再用命令替换接收 |
| 表示成功失败 | `return 0` / `return 非0` |
| 终止整个脚本 | `exit code` |

> [!summary]
> 函数用 stdout 输出结果，用退出码表达成功失败。不要试图用 `return "some string"` 返回字符串。

---

# 5. 条件判断与控制流

## 5.1 条件判断的本质

Shell 判断条件时看的是**命令退出码**：

| 退出码 | 含义 |
|---|---|
| `0` | 成功，条件为真 |
| 非 `0` | 失败，条件为假 |

```bash
if systemctl is-active --quiet nginx; then
    echo "nginx running"
else
    echo "nginx not running"
fi
```

## 5.2 三种判断写法

| 写法 | 类型 | 适用场景 |
|---|---|---|
| `test expression` | 传统命令 | 老脚本 |
| `[ expression ]` | `test` 的别名 | POSIX 兼容脚本 |
| `[[ expression ]]` | Bash 关键字 | Bash 脚本推荐 |

```bash
if [[ "${name}" == "admin" ]]; then
    echo "admin"
fi
```

> [!warning]
> `[ ]` 中 `[` 后面和 `]` 前面必须有空格。Bash 脚本优先使用 `[[ ]]`，它对空字符串、模式匹配和逻辑组合更友好。

## 5.3 数值比较

| 运算符 | 含义 |
|---|---|
| `-eq` | 等于 |
| `-ne` | 不等于 |
| `-gt` | 大于 |
| `-lt` | 小于 |
| `-ge` | 大于等于 |
| `-le` | 小于等于 |

```bash
age=25

if [[ "${age}" -ge 18 ]]; then
    echo "adult"
fi

if (( age >= 18 )); then
    echo "adult"
fi
```

> [!tip]
> 纯数值计算和比较用 `(( ))` 更直观；字符串和文件判断用 `[[ ]]`。

## 5.4 字符串判断

| 运算符 | 含义 |
|---|---|
| `==` | 相等 |
| `!=` | 不相等 |
| `-z` | 空字符串 |
| `-n` | 非空字符串 |
| `=~` | 正则匹配，仅 `[[ ]]` 支持 |

```bash
email="user@example.com"

if [[ -z "${email}" ]]; then
    echo "email empty" >&2
fi

if [[ "${email}" =~ ^[A-Za-z0-9._%+-]+@[A-Za-z0-9.-]+\.[A-Za-z]{2,}$ ]]; then
    echo "valid email"
fi
```

正则捕获：

```bash
version="v3.2.1"

if [[ "${version}" =~ ^v([0-9]+)\.([0-9]+)\.([0-9]+)$ ]]; then
    echo "major=${BASH_REMATCH[1]}"
    echo "minor=${BASH_REMATCH[2]}"
    echo "patch=${BASH_REMATCH[3]}"
fi
```

## 5.5 文件判断

| 表达式 | 含义 |
|---|---|
| `-e path` | 存在 |
| `-f file` | 普通文件 |
| `-d dir` | 目录 |
| `-L path` | 符号链接 |
| `-r file` | 可读 |
| `-w file` | 可写 |
| `-x file` | 可执行 |
| `-s file` | 存在且大小大于 0 |
| `file1 -nt file2` | file1 比 file2 新 |
| `file1 -ot file2` | file1 比 file2 旧 |

```bash
check_config() {
    local config_file="$1"

    [[ -e "${config_file}" ]] || die "配置不存在: ${config_file}"
    [[ -f "${config_file}" ]] || die "不是普通文件: ${config_file}"
    [[ -r "${config_file}" ]] || die "配置不可读: ${config_file}"
}
```

## 5.6 逻辑组合

```bash
if [[ "${role}" == "admin" && "${age}" -ge 18 ]]; then
    echo "adult admin"
fi

if [[ "${env_name}" == "prod" || "${env_name}" == "staging" ]]; then
    echo "protected env"
fi

if [[ ! -d "${target_dir}" ]]; then
    echo "directory not found" >&2
fi
```

## 5.7 `for` 循环

遍历显式列表：

```bash
for server in web01 web02 db01; do
    echo "deploy ${server}"
done
```

遍历文件：

```bash
for file in ./*.log; do
    [[ -e "${file}" ]] || continue
    echo "process ${file}"
done
```

C 风格循环：

```bash
for ((i = 0; i < 10; i++)); do
    echo "${i}"
done
```

## 5.8 `while` 循环与逐行读取

```bash
i=0
while (( i < 5 )); do
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
> 读取文本行时使用 `IFS= read -r`。`IFS=` 保留前后空白，`-r` 防止反斜杠被解释。

## 5.9 输入验证示例

```bash
validate_port() {
    local port="${1:-}"

    if [[ -z "${port}" ]]; then
        echo "错误：端口不能为空" >&2
        return 1
    fi

    if [[ ! "${port}" =~ ^[0-9]+$ ]]; then
        echo "错误：端口必须是数字" >&2
        return 1
    fi

    if (( port < 1 || port > 65535 )); then
        echo "错误：端口范围必须是 1-65535" >&2
        return 1
    fi
}
```

---

# 6. 输入、输出、管道与文本处理

## 6.1 标准输入、标准输出、标准错误

| 文件描述符 | 名称 | 默认位置 |
|---|---|---|
| `0` | stdin | 键盘输入 |
| `1` | stdout | 终端输出 |
| `2` | stderr | 终端错误输出 |

```bash
echo "normal message"
echo "error message" >&2
```

## 6.2 重定向

| 写法 | 含义 |
|---|---|
| `cmd > file` | stdout 覆盖写入 |
| `cmd >> file` | stdout 追加写入 |
| `cmd 2> err.log` | stderr 写入 |
| `cmd > all.log 2>&1` | stdout 和 stderr 都写入 |
| `cmd &> all.log` | Bash 简写，stdout 和 stderr 都写入 |
| `cmd < input.txt` | 从文件读取 stdin |

> [!warning]
> 重定向顺序从左到右生效。`cmd > all.log 2>&1` 会把 stdout 和 stderr 都写入文件；`cmd 2>&1 > all.log` 会让 stderr 仍然输出到终端。

## 6.3 读取用户输入

```bash
read -r name
read -r -p "Input name: " name
read -r -s -p "Password: " password
echo
```

生产脚本中，交互确认应只用于高风险动作：

```bash
read -r -p "确认继续？[y/N] " answer
case "${answer}" in
    y|Y|yes|YES)
        ;;
    *)
        echo "cancelled"
        exit 0
        ;;
esac
```

## 6.4 管道

管道把左侧命令的 stdout 连接到右侧命令的 stdin：

```bash
ps aux | grep "[n]ginx"
```

配合 `pipefail` 后，管道中任一阶段失败都会被视为失败：

```bash
set -o pipefail
grep -i "error" app.log | sort | uniq -c
```

## 6.5 grep、sed、awk 的脚本定位

| 工具 | 主要能力 | 典型用途 |
|---|---|---|
| `grep` | 按条件筛选行 | 查日志、过滤输出 |
| `sed` | 流式编辑文本 | 替换、删除、插入 |
| `awk` | 按字段处理记录 | 抽列、统计、汇总 |

```bash
grep -iE "error|fail|timeout" app.log
sed 's/old/new/g' config.txt
awk '{print $1}' access.log
awk '{print $1}' access.log | sort | uniq -c | sort -nr | head
```

> [!info]
> 脚本中优先把文本处理限制在“搜索、替换、抽列、统计”这几类清晰任务。复杂结构化数据应使用 `jq`、`yq`、Python 等专用工具。

## 6.6 `find` 与安全批处理

普通查找：

```bash
find /var/log/myapp -type f -name '*.log' -mtime +14 -print
```

安全处理带空格文件名：

```bash
find /var/log/myapp -type f -name '*.log' -mtime +14 -print0 |
while IFS= read -r -d '' file; do
    echo "remove ${file}"
done
```

> [!warning]
> 文件名可能包含空格、换行和特殊字符。高风险批处理优先使用 `-print0` 与 `read -d ''`，或使用支持空字符分隔的 `xargs -0`。

---

# 7. 错误处理、清理与调试

## 7.1 统一失败出口

```bash
die() {
    printf '[ERROR] %s\n' "$*" >&2
    exit 1
}

[[ -f "${config_file}" ]] || die "配置文件不存在: ${config_file}"
```

建议约定退出码：

| 退出码 | 含义 |
|---|---|
| `0` | 成功 |
| `1` | 通用失败 |
| `2` | 参数错误或用法错误 |
| `126` | 命令不可执行 |
| `127` | 命令不存在 |
| `130` | 被 `Ctrl+C` 中断 |

## 7.2 可预期失败要显式处理

`grep -q` 找不到内容会返回 `1`，这在业务上可能是正常结果：

```bash
if grep -q "ready" app.log; then
    echo "ready"
else
    echo "not ready"
fi
```

不要依赖 `set -e` 猜测业务含义。脚本应明确区分“命令异常”和“业务条件不满足”。

## 7.3 `trap` 与退出清理

临时文件必须在退出时清理：

```bash
tmp_file="$(mktemp)"

cleanup() {
    rm -f -- "${tmp_file}"
}

trap cleanup EXIT
```

处理中断信号：

```bash
on_interrupt() {
    echo "interrupted" >&2
    exit 130
}

trap on_interrupt INT TERM
```

| 信号 | 常见来源 | 典型处理 |
|---|---|---|
| `EXIT` | 脚本退出 | 清理临时文件 |
| `INT` | `Ctrl+C` | 中断并返回 130 |
| `TERM` | systemd、Docker、Kubernetes 终止进程 | 优雅退出 |

## 7.4 调试脚本

语法检查：

```bash
bash -n script.sh
```

执行跟踪：

```bash
bash -x script.sh
```

局部跟踪：

```bash
set -x
command_a
command_b
set +x
```

静态检查：

```bash
shellcheck script.sh
```

> [!summary]
> 调试顺序：先 `bash -n` 查语法，再 `shellcheck` 查隐患，最后 `bash -x` 跟踪运行路径。

---

# 8. 路径安全与高风险操作

## 8.1 不依赖当前目录

脚本启动时的当前目录取决于调用者，不一定是脚本所在目录。

```bash
readonly SCRIPT_DIR="$(
    cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd -P
)"
readonly PROJECT_DIR="$(dirname -- "${SCRIPT_DIR}")"
```

需要进入工作目录时，应显式执行：

```bash
cd -- "${PROJECT_DIR}" || die "无法进入项目目录: ${PROJECT_DIR}"
```

## 8.2 删除前的路径校验

高风险变量必须检查：

```bash
validate_target_dir() {
    local target_dir="${1:-}"
    local allowed_root="/var/log/myapp"

    [[ -n "${target_dir}" ]] || die "目标目录为空"
    [[ "${target_dir}" != "/" ]] || die "禁止操作根目录"
    [[ -d "${target_dir}" ]] || die "目标目录不存在: ${target_dir}"

    case "${target_dir}" in
        "${allowed_root}"/*)
            ;;
        *)
            die "目标目录不在允许范围内: ${target_dir}"
            ;;
    esac
}
```

删除流程建议：

```text
计算目标 → 打印预览 → 用户确认或 dry-run → 执行 → 记录结果
```

> [!warning]
> `rm -rf "${var}"` 前必须确认变量非空、不是根目录、在预期目录内。自动化脚本中，路径校验比命令本身更重要。

## 8.3 命令存在性检查

```bash
require_cmd() {
    command -v "$1" >/dev/null 2>&1 || die "缺少命令: $1"
}

require_cmd curl
require_cmd jq
```

前置检查示例：

```bash
preflight_check() {
    local errors=0

    for cmd in curl jq systemctl; do
        if ! command -v "${cmd}" >/dev/null 2>&1; then
            echo "缺少命令: ${cmd}" >&2
            ((errors++))
        fi
    done

    [[ -n "${DATABASE_URL:-}" ]] || {
        echo "缺少环境变量: DATABASE_URL" >&2
        ((errors++))
    }

    (( errors == 0 )) || return 1
}
```

---

# 9. 实战脚本

## 9.1 日志清理脚本

```bash
#!/usr/bin/env bash
set -euo pipefail

readonly LOG_DIR="${1:-/var/log/myapp}"
readonly DAYS="${2:-14}"

log_info() {
    printf '[%s] [INFO] %s\n' "$(date '+%F %T')" "$*"
}

die() {
    printf '[%s] [ERROR] %s\n' "$(date '+%F %T')" "$*" >&2
    exit 1
}

main() {
    [[ "${DAYS}" =~ ^[0-9]+$ ]] || die "保留天数必须是数字: ${DAYS}"
    [[ -d "${LOG_DIR}" ]] || die "日志目录不存在: ${LOG_DIR}"
    [[ "${LOG_DIR}" != "/" ]] || die "禁止清理根目录"

    log_info "preview files older than ${DAYS} days in ${LOG_DIR}"
    find "${LOG_DIR}" -type f -name '*.log' -mtime +"${DAYS}" -print

    read -r -p "确认删除以上文件？[y/N] " answer
    case "${answer}" in
        y|Y|yes|YES)
            find "${LOG_DIR}" -type f -name '*.log' -mtime +"${DAYS}" -delete
            log_info "done"
            ;;
        *)
            log_info "cancelled"
            ;;
    esac
}

main "$@"
```

设计要点：

| 步骤 | 目的 |
|---|---|
| 参数默认值 | 无参数时也可运行 |
| 数值校验 | 防止 `find -mtime` 收到非法值 |
| 目录校验 | 防止误操作不存在或危险路径 |
| 先预览 | 让用户确认影响范围 |
| 再删除 | 高风险动作后置 |

## 9.2 服务健康检查脚本

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
    command -v systemctl >/dev/null 2>&1 || die "缺少 systemctl"
    command -v curl >/dev/null 2>&1 || die "缺少 curl"

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

## 9.3 批量备份脚本

```bash
#!/usr/bin/env bash
set -euo pipefail

readonly SRC_DIR="${1:-}"
readonly BACKUP_DIR="${2:-/tmp/backups}"
readonly TS="$(date '+%Y%m%d_%H%M%S')"

die() {
    echo "[ERROR] $*" >&2
    exit 1
}

main() {
    [[ -n "${SRC_DIR}" ]] || die "用法: $0 <源目录> [备份目录]"
    [[ -d "${SRC_DIR}" ]] || die "源目录不存在: ${SRC_DIR}"

    mkdir -p -- "${BACKUP_DIR}"

    local src_name
    src_name="$(basename -- "${SRC_DIR}")"

    local output="${BACKUP_DIR}/${src_name}.${TS}.tar.gz"
    tar -czf "${output}" -C "$(dirname -- "${SRC_DIR}")" "${src_name}"

    echo "backup created: ${output}"
}

main "$@"
```

关键点：

- 使用 `tar -C` 避免压缩包内出现冗长绝对路径。
- 输出文件带时间戳，避免覆盖旧备份。
- 对源目录和备份目录分别校验。

---

# 10. 脚本规范

## 10.1 推荐规则

| 主题 | 推荐写法 |
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

## 10.2 脚本审查清单

- 是否有正确的 shebang。
- 是否启用 `set -euo pipefail`，并处理可预期失败。
- 变量引用是否使用 `"${var}"`。
- Bash 专属语法是否没有写在 `#!/bin/sh` 脚本里。
- 参数是否有用法提示和边界校验。
- 函数是否用退出码表示成功失败。
- 是否避免依赖当前目录。
- 删除、覆盖、迁移前是否检查路径范围。
- 是否对外部命令做了存在性检查。
- 是否能通过 `bash -n` 和 `shellcheck`。

---

# 11. 学习路线

## 11.1 生存期

先掌握：

- shebang 与执行方式。
- 变量定义与 `"${var}"` 引用。
- `if`、`case`、`for`、`while`。
- 函数和 `main "$@"`。
- `set -euo pipefail`。

## 11.2 效率期

继续学习：

- `grep`、`sed`、`awk`。
- `find`、`xargs`。
- `rsync`、`ssh`、`scp`。
- `crontab`、systemd timer。
- ShellCheck。

## 11.3 工程期

开始沉淀：

- 部署脚本。
- 健康检查脚本。
- 日志清理脚本。
- 备份与恢复脚本。
- 巡检脚本。
- 事故处理 Runbook。

---

# 12. 易错点总结

> [!summary]
> 本篇核心：Shell 脚本不是把命令简单堆起来，而是把命令组织成可重复、可检查、可失败退出、可维护的自动化流程。

- 变量赋值等号两边不能有空格。
- 变量引用默认使用 `"${var}"`。
- Bash 专属语法不要放在 `#!/bin/sh` 脚本里。
- 遍历参数用 `"$@"`，遍历数组用 `"${array[@]}"`。
- `return` 返回退出码，不返回业务结果。
- `set -e` 不能替代错误处理；可预期失败要显式处理。
- 读取文件行用 `IFS= read -r`。
- 管道脚本建议开启 `pipefail`。
- 删除、覆盖、迁移前要校验路径，并尽量先预览。
- Shell 适合编排命令；复杂业务逻辑应选择更合适的语言。
