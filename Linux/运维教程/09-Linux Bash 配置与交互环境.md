---
title: Shell 配置 — Bash
date: 2026-06-15
tags:
  - linux
  - bash
  - shell
  - 配置
aliases:
  - Bash 配置
  - bashrc
  - bash_profile
---

# 1. 核心概念

## 1.1 Bash 配置的本质

Bash 配置的核心目标是：**控制 Shell 会话的行为**。通过一系列配置文件和内置机制，实现环境变量设置、命令别名定义、函数封装、提示符定制等功能。

配置之所以复杂，是因为 Bash 在不同**启动模式**下会加载不同的配置文件。理解启动模式是理解配置加载的前提。

## 1.2 Shell 的四种启动模式

Bash 会话由两个维度交叉定义：

| 维度 | 类型 | 含义 | 典型场景 |
|---|---|---|---|
| 交互性 | **交互式** (Interactive) | 等待用户输入命令 | 终端模拟器、SSH 登录 |
| 交互性 | **非交互式** (Non-interactive) | 执行脚本，不等待输入 | `bash script.sh` |
| 登录 | **登录 Shell** (Login) | 用户身份验证后的 Shell | SSH 登录、`su - user`、TTY 登录 |
| 登录 | **非登录 Shell** (Non-login) | 无需验证即获得的 Shell | 图形终端打开新标签页、`bash` 直接执行 |

**判断当前 Shell 类型：**

```bash
# 判断是否为交互式 Shell
# 输出 "interactive" 则为交互式，否则为非交互式
[[ $- == *i* ]] && echo "interactive" || echo "non-interactive"

# 判断是否为登录 Shell
# 输出 "login" 则为登录 Shell
shopt -q login_shell && echo "login" || echo "non-login"
```

> [!tip] 记忆技巧
> 交互式关注”能不能打字“，登录式关注”有没有验过身份“。两者独立——SSH 登录是**交互式 + 登录**，图形终端新标签是**交互式 + 非登录**，`bash script.sh` 是**非交互式 + 非登录**。

---

# 2. 配置文件加载顺序

## 2.1 加载流程图

```mermaid
flowchart TD
    A["Bash 启动"] --> B{"登录 Shell?"}
    B -- 是 --> C["/etc/profile"]
    C --> D["~/.bash_profile<br/>~/.bash_login<br/>~/.profile<br/>（按顺序，找到即停）"]
    D --> E["会话就绪"]
    B -- 否 --> F{"交互式?"}
    F -- 是 --> G["~/.bashrc"]
    G --> E
    F -- 否 --> H{"指定了 --rcfile?"}
    H -- 是 --> I["加载指定文件"]
    I --> E
    H -- 否 --> J["使用默认环境<br/>（不加载用户配置）"]
    J --> E

    style C fill:#e8f5e9
    style D fill:#e3f2fd
    style G fill:#fff3e0
```

## 2.2 各配置文件职责

| 文件 | 作用域 | 触发条件 | 典型用途 |
|---|---|---|---|
| `/etc/profile` | 全局 | 登录 Shell | 系统级环境变量、PATH |
| `/etc/profile.d/*.sh` | 全局 | 被 `/etc/profile` 调用 | 按应用拆分的全局配置 |
| `/etc/bash.bashrc` | 全局 | 交互式非登录 Shell | 系统级别名、函数 |
| `~/.bash_profile` | 用户 | 登录 Shell（优先级最高） | 用户环境变量、启动 `~/.bashrc` |
| `~/.bash_login` | 用户 | 登录 Shell（次选） | 同上，较少使用 |
| `~/.profile` | 用户 | 登录 Shell（末选） | 兼容其他 Shell 的配置 |
| `~/.bashrc` | 用户 | 交互式非登录 Shell | 别名、函数、提示符定制 |
| `~/.bash_logout` | 用户 | 登录 Shell 退出时 | 清理临时文件、历史写入 |

> [!warning] 关键陷阱
> - `~/.bash_profile`、`~/.bash_login`、`~/.profile` 三者**只加载第一个找到的**，不会叠加。
> - 大多数发行版的 `~/.bash_profile` 末尾会 `source ~/.bashrc`，确保登录 Shell 也能获得 `~/.bashrc` 中的配置。**如果你的 `~/.bash_profile` 没有这样做，登录 Shell 下别名和函数将不生效。**

## 2.3 退出时的清理

登录 Shell 退出时加载 `~/.bash_logout`，典型配置：

```bash
# 清除屏幕内容，防止敏感信息残留
clear
```

---

# 3. 核心配置语法与变量

## 3.1 环境变量

### 3.1.1 定义与导出

```bash
# 局部变量：仅当前 Shell 可见
MY_VAR="hello"

# 导出为环境变量：子进程可继承
export MY_VAR="hello"

# 等价写法
MY_VAR="hello"
export MY_VAR
```

> [!warning] 注意
> 不加 `export` 的变量**不会传递给子进程**。在 `~/.bashrc` 中设置 PATH 时必须 `export`，否则子 Shell（如 `vim` 中 `:!cmd`）无法获取。

### 3.1.2 常用环境变量

| 变量 | 含义 | 示例值 |
|---|---|---|
| `PATH` | 命令搜索路径 | `/usr/local/bin:/usr/bin:/bin` |
| `HOME` | 用户主目录 | `/home/sky` |
| `USER` | 当前用户名 | `sky` |
| `SHELL` | 默认 Shell 路径 | `/bin/bash` |
| `PWD` | 当前工作目录 | `/home/sky/project` |
| `LANG` | 语言区域设置 | `en_US.UTF-8` |
| `HISTSIZE` | 内存中保留的历史条数 | `100000` |
| `HISTFILESIZE` | 磁盘历史文件最大条数 | `200000` |
| `HISTTIMEFORMAT` | 历史记录时间格式 | `"%F %T "` |

### 3.1.3 PATH 配置最佳实践

```bash
# 追加路径（推荐）：不影响系统已有路径
export PATH="$PATH:/opt/myapp/bin"

# 前置路径：优先使用自定义版本
export PATH="/usr/local/bin:$PATH"
```

> [!tip] 提示
> 将自定义路径放在 `$PATH` 之后（追加），避免意外覆盖系统命令；仅在明确需要覆盖时才前置。

## 3.2 别名（alias）

```bash
# 基本语法：alias 别名='实际命令'
alias ll='ls -alF'           # 详细列表
alias la='ls -A'             # 显示隐藏文件（不含 . 和 ..）
alias gs='git status'        # Git 快捷操作
alias rm='rm -i'             # 删除前确认

# 查看所有已定义别名
alias

# 查看特定别名定义
alias ll
# 输出：alias ll='ls -alF'

# 临时取消别名，使用原始命令
\rm file.txt
```

> [!warning] 注意
> - 别名**不支持参数替换**。如需参数，请使用函数（见 [[#3.3 函数]]）。
> - 在脚本中别名默认不展开，需 `shopt -s expand_aliases` 才能使用。

## 3.3 函数

```bash
# 基本语法
function_name() {
    # 函数体
    # $1, $2, ... 为位置参数
}

# 示例：快速创建目录并进入
mkcd() {
    mkdir -p "$1"    # -p 确保父目录存在
    cd "$1"          # 进入创建的目录
}

# 使用
mkcd ~/project/new_dir
```

### 3.3.1 别名 vs 函数选择原则

| 场景 | 推荐方式 | 原因 |
|---|---|---|
| 简单命令缩写 | `alias` | 语法简洁 |
| 需要参数处理 | 函数 | 别名无法接收参数 |
| 包含条件/循环逻辑 | 函数 | 别名仅做文本替换 |

## 3.4 Shell 选项（shopt）

```bash
# 查看所有选项状态
shopt

# 查看特定选项
shopt histappend
# 输出：histappend  on

# 开启选项
shopt -s histappend    # 历史追加而非覆盖

# 关闭选项
shopt -u histappend
```

**常用选项：**

| 选项 | 作用 | 推荐设置 |
|---|---|---|
| `histappend` | 退出时追加历史而非覆盖 | `on`（多终端共享历史必备） |
| `autocd` | 输入目录名直接 cd，无需 `cd` 前缀 | 按需 |
| `cdspell` | 自动修正 cd 拼写错误 | 按需 |
| `checkwinsize` | 命令执行后检查终端窗口大小 | `on`（默认） |

---

# 4. 实战场景与排错

## 4.1 历史记录增强：记录命令执行目录

### 4.1.1 问题背景

Bash 默认的 `history` 只记录命令本身，**不记录执行时的工作目录**：

```bash
$ history
1001  ls
1002  vim test.txt
```

无法得知 `vim test.txt` 是在哪个目录下执行的。

### 4.1.2 回溯已有历史的方法

| 方法 | 命令 | 能否获取目录 | 前提条件 |
|---|---|---|---|
| `history` + `HISTTIMEFORMAT` | `export HISTTIMEFORMAT="%F %T "` | 否 | 需提前配置 |
| `~/.bash_history` | `cat ~/.bash_history` | 否 | 默认仅存命令文本 |
| `auditd` 审计日志 | `ausearch -i -ua $(id -u)` | **是** | 系统需安装并启用 auditd |

> [!summary] 核心结论
> 若事前未配置目录记录且未启用 auditd，**事后无法从历史中恢复命令执行目录**。必须提前配置。

### 4.1.3 推荐配置：带目录的命令日志

在 `~/.bashrc` 末尾添加以下配置：

```bash
# --- 历史记录增强配置 ---

# 内存中保留的历史条数
export HISTSIZE=100000
# 磁盘历史文件最大条数
export HISTFILESIZE=200000
# 历史记录显示时间戳
export HISTTIMEFORMAT="%F %T "
# 忽略连续重复命令
export HISTCONTROL=ignoredups:erasedups
# 退出时追加历史（而非覆盖），多终端共享历史的关键
shopt -s histappend

# 去重状态：记录上次写入的条目（目录+命令），避免重复写入
LAST_LOGGED_ENTRY=""

# 命令日志函数：每次提示符显示前自动调用
log_command() {
    local cmd
    local entry

    # 从 history 获取最近一条命令，去除行号前缀
    cmd=$(history 1 | sed 's/^[ ]*[0-9]\+[ ]*//')

    # 将"当前目录+命令"组合为唯一标识，用于去重
    entry="$(pwd)|$cmd"

    # 如果与上次完全相同则跳过（同目录同命令不重复记录）
    if [[ "$entry" == "$LAST_LOGGED_ENTRY" ]]; then
        return
    fi

    LAST_LOGGED_ENTRY="$entry"

    # 追加写入日志文件，格式：时间 | 用户 | 目录 | 命令
    printf '%s | %s | %s | %s\n' \
        "$(date '+%F %T')" \
        "$USER" \
        "$(pwd)" \
        "$cmd" \
        >> "$HOME/.full_history"
}

# PROMPT_COMMAND：每次显示提示符前执行
# history -a 确保命令立即写入磁盘历史文件
PROMPT_COMMAND="history -a; log_command"
```

加载配置：

```bash
source ~/.bashrc
```

### 4.1.4 生效效果

正常使用 Shell 后，查看日志：

```bash
tail ~/.full_history
```

输出示例：

```text
2026-06-10 14:30:01 | sky | /home/sky | pwd
2026-06-10 14:30:05 | sky | /tmp | ls
2026-06-10 14:30:10 | sky | /home/sky/project | git status
```

实时监控：

```bash
tail -f ~/.full_history
```

### 4.1.5 去重逻辑说明

去重依据是 **目录+命令** 的组合，而非仅命令本身：

| 场景 | 是否记录 | 原因 |
|---|---|---|
| `/tmp` 下执行 `ls`，再在 `/home` 下执行 `ls` | 两次均记录 | 目录不同，属于不同上下文 |
| `/tmp` 下连续执行两次 `ls` | 仅记录一次 | 目录和命令完全相同 |
| 新终端中首次执行与上次终端相同的命令 | 会记录 | `LAST_LOGGED_ENTRY` 在新 Shell 中重新初始化 |

> [!warning] 注意
> `LAST_LOGGED_ENTRY` 是 Shell 变量，仅在当前 Shell 会话内有效。新开终端后该变量重置为空，因此不同终端间不共享去重状态——这是预期行为，不会导致日志丢失。

### 4.1.6 进阶方案：Bash 5.x 历史编号去重

Bash 5.x 可利用历史序号判断是否已记录，比字符串比较更可靠：

```bash
LAST_HIST_NUM=0

log_command() {
    local hist_num
    local cmd

    # 获取最近一条历史记录的编号
    hist_num=$(history 1 | awk '{print $1}')

    # 编号未变则说明无新命令，跳过
    if [[ "$hist_num" == "$LAST_HIST_NUM" ]]; then
        return
    fi

    LAST_HIST_NUM="$hist_num"

    cmd=$(history 1 | sed 's/^[ ]*[0-9]\+[ ]*//')

    printf '%s | %s | %s | %s\n' \
        "$(date '+%F %T')" \
        "$USER" \
        "$(pwd)" \
        "$cmd" \
        >> "$HOME/.full_history"
}

PROMPT_COMMAND="history -a; log_command"
```

### 4.1.7 扩展日志：记录更多信息

如需记录主机名、PID、TTY 等信息：

```bash
log_command() {
    local cmd
    cmd=$(history 1 | sed 's/^[ ]*[0-9]\+[ ]*//')

    # 格式：时间 | 用户 | 主机 | PID | TTY | 目录 | 命令
    printf '%s | user=%s | host=%s | pid=%s | tty=%s | cwd=%s | cmd=%s\n' \
        "$(date '+%F %T')" \
        "$USER" \
        "$(hostname)" \
        "$$" \
        "$(tty 2>/dev/null)" \
        "$(pwd)" \
        "$cmd" \
        >> "$HOME/.full_history"
}
```

输出示例：

```text
2026-06-10 15:30:01 | user=sky | host=server01 | pid=32418 | tty=/dev/pts/2 | cwd=/home/sky/project | cmd=git status
```

### 4.1.8 关于 `trap DEBUG` 方案

Bash 提供 `trap DEBUG` 机制可在命令执行前触发：

```bash
preexec_logger() {
    local cmd="$BASH_COMMAND"
    echo "$(date '+%F %T') | $(pwd) | $cmd" >> "$HOME/.full_history"
}
trap 'preexec_logger' DEBUG
```

> [!warning] 不推荐此方案
> `trap DEBUG` 会捕获**所有**命令执行，包括函数内部的 `printf`、`sed`、`history` 等子命令，产生大量噪音日志。`PROMPT_COMMAND` 方案仅在提示符显示前触发一次，更干净可靠。

## 4.2 提示符定制（PS1）

`PS1` 变量控制主提示符的显示内容：

```bash
# 默认提示符
echo $PS1
# 输出：[\u@\h \W]\$

# 自定义提示符：显示用户@主机 全路径 时间
export PS1='[\u@\h \w \t]\$ '
# 生效后显示：[sky@server01 /home/sky/project 14:30:01]$
```

**常用转义序列：**

| 转义 | 含义 | 示例输出 |
|---|---|---|
| `\u` | 用户名 | `sky` |
| `\h` | 主机名（短） | `server01` |
| `\H` | 主机名（全） | `server01.example.com` |
| `\w` | 当前目录（完整路径） | `/home/sky/project` |
| `\W` | 当前目录（仅目录名） | `project` |
| `\t` | 24 小时制时间 | `14:30:01` |
| `\d` | 日期 | `Sun Jun 15` |
| `\$` | root 显示 `#`，普通用户显示 `$` | `$` |

## 4.3 配置失效排查

| 现象 | 可能原因 | 排查方法 |
|---|---|---|
| 别名不生效 | 在登录 Shell 中，`~/.bash_profile` 未 source `~/.bashrc` | 检查 `~/.bash_profile` 是否包含 `source ~/.bashrc` |
| 环境变量子进程看不到 | 忘记 `export` | `export VAR=value` 替代 `VAR=value` |
| 修改 `~/.bashrc` 后未生效 | 未重新加载 | `source ~/.bashrc` 或新开终端 |
| 多终端历史丢失 | `histappend` 未开启或 `PROMPT_COMMAND` 未配置 | `shopt -s histappend` 并配置 `PROMPT_COMMAND='history -a'` |
| SSH 登录后提示符异常 | `~/.bash_profile` 覆盖了 PS1 | 检查 `~/.bash_profile` 中的 PS1 设置 |
| 脚本中别名不生效 | 非交互式 Shell 默认不展开别名 | 脚本开头加 `shopt -s expand_aliases` |

> [!tip] 通用排查流程
> 1. 确认当前 Shell 类型：`echo $-`（含 `i` 为交互式）、`shopt -q login_shell && echo login`
> 2. 确认配置文件是否被加载：在文件中加入 `echo "loaded ~/.bashrc"` 临时调试
> 3. 确认变量是否生效：`echo $VAR_NAME`
> 4. 确认加载顺序：在 `set -x` 模式下启动 Bash 观察执行过程

---

# 5. 推荐的 bash 配置

```bash
# ============================================================
#  .bashrc — 服务端运维优化版
#  适用：Ubuntu / Debian / CentOS / RHEL 及各主流 Linux 发行版
# ============================================================

# 非交互式 shell 直接返回
[[ $- != *i* ]] && return

# ============================================================
#  HELPERS — 辅助函数
# ============================================================
_has() {
    command -v "$1" >/dev/null 2>&1
}

# ============================================================
#  PATH
# ============================================================
# 注意：前后加冒号 (:) 是为了防止 /a/b 匹配到 /a/bc 这种情况
[[ -d "$HOME/.bin" && ":$PATH:" != *":$HOME/.bin:"* ]] && PATH="$HOME/.bin:$PATH"
[[ -d "$HOME/.local/bin" && ":$PATH:" != *":$HOME/.local/bin:"* ]] && PATH="$HOME/.local/bin:$PATH"

# ============================================================
#  EXPORT — 环境变量
# ============================================================
export EDITOR='vim'
export VISUAL='vim'
export PAGER='less'
export LESS='-R --quit-if-one-screen'

# 历史记录：忽略重复 & 空格开头，保留 50000 条
export HISTCONTROL=ignoreboth:erasedups
export HISTSIZE=50000
export HISTFILESIZE=100000
export HISTTIMEFORMAT='%F %T  '   # 每条命令显示时间戳

# ============================================================
#  SHOPT — shell 选项
# ============================================================
shopt -s autocd        # 直接输入目录名即可 cd
shopt -s cdspell       # cd 拼写自动纠错
shopt -s cmdhist       # 多行命令保存为单行历史
shopt -s histappend    # 追加历史，不覆盖
shopt -s checkwinsize  # 每次命令后更新窗口尺寸

# TAB 补全忽略大小写
bind "set completion-ignore-case on"

# 多个匹配时立即列出
bind "set show-all-if-ambiguous on"

# 第一次 TAB 显示所有候选
bind "set show-all-if-unmodified on"

# 补全时把 symlink 当目录处理
bind "set mark-symlinked-directories on"

# ============================================================
#  PROMPT — PS1
# ============================================================

# Git 官方 Prompt（存在时加载）
if [[ -f /usr/share/git/completion/git-prompt.sh ]]; then
    source /usr/share/git/completion/git-prompt.sh
fi

# Git 分支 fallback：官方 git-prompt 不存在时使用
git_branch_name() {
    git rev-parse --is-inside-work-tree >/dev/null 2>&1 || return 0

    local branch
    branch=$(git symbolic-ref --quiet --short HEAD 2>/dev/null)

    # detached HEAD 时显示短 commit
    if [[ -z "$branch" ]]; then
        branch=$(git rev-parse --short HEAD 2>/dev/null) || return 0
        branch="detached:${branch}"
    fi

    [[ -n "$branch" ]] && printf ' (%s)' "$branch"
}

update_prompt() {
    local exit_code=$?

    local green="\[\e[32m\]"
    local red="\[\e[31m\]"
    local yellow="\[\e[33m\]"
    local cyan="\[\e[36m\]"
    local magenta="\[\e[35m\]"
    local reset="\[\e[0m\]"
    local bold="\[\e[1m\]"

    local datetime
    datetime=$(date +'%m/%d %H:%M')

    local git_br=""
    if declare -F __git_ps1 >/dev/null 2>&1; then
        git_br="$(__git_ps1 " (%s)")"
    elif command -v git >/dev/null 2>&1; then
        git_br="$(git_branch_name)"
    fi

    local status=""
    if (( exit_code != 0 )); then
        status=" ${red}[✘${exit_code}]${reset}"
    fi

    local symbol
    if (( EUID == 0 )); then
        symbol="${red}#${reset}"
    else
        symbol="${green}\$${reset}"
    fi

    PS1="\n${bold}${green}[${cyan}${datetime}${green}] ${red}\u${green}@${yellow}\h${reset}:${cyan}\w${magenta}${git_br}${reset}${status}\n${bold}${symbol} ${reset}"
}

# 保留已有 PROMPT_COMMAND，并避免重复追加
[[ "$PROMPT_COMMAND" != *update_prompt* ]] && \
    PROMPT_COMMAND="${PROMPT_COMMAND:+$PROMPT_COMMAND; }update_prompt"

# ============================================================
#  ALIASES — 基础
# ============================================================

# 文件列表
alias ls='ls --color=auto'
alias ll='ls -alFh'
alias la='ls -A'
alias l='ls -CF'
alias lh='ls -lhS'                  # 按文件大小排序
alias lt='ls -lhtr'                 # 按修改时间排序（最新在底部）

# 导航
alias ..='cd ..'
alias ...='cd ../..'
alias ....='cd ../../..'
alias back='cd -'                   # 返回上一个目录

# 安全操作
alias rmi='rm -I'
alias cpi='cp -i'
alias mvi='mv -i'
alias mkdir='mkdir -pv'

# grep / 搜索
alias grep='grep --color=auto'
alias egrep='grep -E --color=auto'
alias fgrep='grep -F --color=auto'
_has rg && alias rg='rg --sort path'           # ripgrep（需安装）

# 杂项
alias df='df -hT'                   # 显示文件系统类型
alias du='du -h'
alias free='free -mht'
alias wget='wget -c'                 # 断点续传
alias path='echo $PATH | tr ":" "\n"'
alias now='date +"%Y-%m-%d %H:%M:%S %Z"'
alias cls='clear'
alias h='history'
alias j='jobs -l'
alias which='which -a'
alias typea='type -a'

# ============================================================
#  ALIASES — 进程 & 系统监控
# ============================================================
alias psa='ps auxf'
alias psm='ps aux --sort=-%mem | head -20'      # 内存占用 Top20
alias psc='ps aux --sort=-%cpu | head -20'      # CPU 占用 Top20

alias top='top -c'
# 优先使用 htop / btop（如已安装）
_has htop && alias ht='htop'
_has btop && alias bt='btop'

# 快速计时
alias rtime='/usr/bin/time -v'

# ============================================================
#  ALIASES — 网络诊断
# ============================================================
alias ip='ip --color=auto'
alias ipa='ip -br addr'             # 简洁显示 IP 地址
alias ipr='ip -br route'            # 路由表
alias ports='ss -tulnp'             # 监听端口（替代 netstat）
alias lsports='lsof -i -P -n'       # 所有网络连接
alias myip='curl -s https://ifconfig.me && echo'
alias myip6='curl -s https://api64.ipify.org && echo'
alias ping5='ping -c 5'
alias nmap-quick='nmap -sV --open'

# DNS 查询
alias digs='dig +short'
alias digfull='dig +noall +answer +authority'
alias nslook='nslookup'

# 带宽监控（需安装对应工具）
alias bwmon='iftop -n'              # 实时带宽
alias nethogs='nethogs'             # 按进程显示带宽

# ============================================================
#  ALIASES — 日志查看
# ============================================================
alias jctl='journalctl -p 3 -xb'                   # 本次启动的错误日志
alias jctlf='journalctl -f'                         # 实时跟踪日志
alias jctlu='journalctl -u'                         # 用法: jctlu nginx
alias syslog='tail -f /var/log/syslog'
alias messages='tail -f /var/log/messages'
alias authlog='tail -f /var/log/auth.log'
alias kernlog='dmesg -T | tail -50'                 # 带时间戳的内核日志
alias lastlog='last -n 20'                          # 最近登录记录
alias faillog='lastb -n 20 2>/dev/null || journalctl _SYSTEMD_UNIT=sshd.service | grep "Failed"'

# ============================================================
#  ALIASES — systemd 服务管理
# ============================================================
alias sc='systemctl'
alias scs='systemctl status'
alias scst='systemctl start'
alias scsp='systemctl stop'
alias scr='systemctl restart'
alias scrl='systemctl reload'
alias sce='systemctl enable'
alias scd='systemctl disable'
alias scfail='systemctl list-units --failed'        # 列出失败的服务
alias sclist='systemctl list-units --type=service --state=running'

# ============================================================
#  ALIASES — 防火墙
# ============================================================
# UFW（Debian/Ubuntu 系）
alias ufw-status='sudo ufw status verbose'
alias ufw-list='sudo ufw status numbered'

# firewalld（RHEL/CentOS 系）
alias fw-status='sudo firewall-cmd --state'
alias fw-list='sudo firewall-cmd --list-all'

# iptables 查看
alias ipt='sudo iptables -L -n -v --line-numbers'
alias ipt6='sudo ip6tables -L -n -v --line-numbers'

# ============================================================
#  ALIASES — 磁盘 & 存储
# ============================================================
alias duf='df -hT | grep -v tmpfs'             # 过滤临时文件系统
alias dush='du -sh * | sort -rh | head -20'    # 当前目录各项大小 Top20
alias iostat='iostat -xz 1 5'                  # I/O 统计
alias lsblkh='lsblk -o NAME,SIZE,TYPE,FSTYPE,MOUNTPOINT,UUID'

# ============================================================
#  ALIASES — SSH / 远程
# ============================================================
alias scp='scp -p'
alias ssh-keygen-ed='ssh-keygen -t ed25519 -C'  # 用法: ssh-keygen-ed "comment"
alias who-ssh='who | grep pts'                  # 列出当前 SSH 连接

# ============================================================
#  ALIASES — Docker（如使用）
# ============================================================
alias d='docker'
alias dps='docker ps --format "table {{.ID}}\t{{.Image}}\t{{.Status}}\t{{.Ports}}\t{{.Names}}"'
alias dpsa='docker ps -a --format "table {{.ID}}\t{{.Image}}\t{{.Status}}\t{{.Names}}"'
alias di='docker images'
alias dex='docker exec -it'                    # 用法: dex <容器名> bash
alias dlog='docker logs -f --tail 100'         # 用法: dlog <容器名>
alias dprune-force='docker system prune -f'
alias dc='docker-compose'
alias dcu='docker-compose up -d'
alias dcd='docker-compose down'
alias dcl='docker-compose logs -f'

# ============================================================
#  ALIASES — Git（运维场景）
# ============================================================
alias gs='git status -sb'
alias gp='git pull'
alias glog='git log --oneline --graph --decorate -20'
alias gdiff='git diff --stat'

# ============================================================
#  ALIASES — 包管理（按发行版自动选择）
# ============================================================
if command -v apt &>/dev/null; then
    alias update='sudo apt update && sudo apt upgrade -y'
    alias install='sudo apt install'
    alias search='apt search'
    alias remove='sudo apt remove'
    alias autoremove='sudo apt autoremove -y'
elif command -v dnf &>/dev/null; then
    alias update='sudo dnf upgrade -y'
    alias install='sudo dnf install'
    alias search='dnf search'
    alias remove='sudo dnf remove'
    alias autoremove='sudo dnf autoremove -y'
elif command -v yum &>/dev/null; then
    alias update='sudo yum update -y'
    alias install='sudo yum install'
    alias search='yum search'
    alias remove='sudo yum remove'
fi

# ============================================================
#  FUNCTIONS — 实用函数
# ============================================================

# 用法: psgrep nginx
psg() {
    [[ -z "$1" ]] && echo "用法: psg <keyword>" && return 2
    ps aux | grep -i -- "$1" | grep -v grep
}

# 监听端口
listen() {
    if [[ -z "$1" ]]; then
        echo "用法: listen <端口|关键字>"
        echo "示例: listen 80"
        echo "示例: listen nginx"
        return 2
    fi

    ss -tulnp | grep --color=auto -F -- "$1"
}

# 推送并显示进度
rsync-push() {
    rsync -avzP --stats "$1" "$2"
}

# 拉取并显示进度
rsync-pull() {
    rsync -avzP --stats "$1" "$2"
}

# 停止所有容器
dstop-all() {
    if ! command -v docker >/dev/null 2>&1; then
        echo "错误: 未找到 docker 命令"
        return 127
    fi

    local ids
    ids=$(docker ps -q)

    if [[ -z "$ids" ]]; then
        echo "没有运行中的容器"
        return 0
    fi

    docker ps
    read -r -p "确认停止所有运行中的容器？[y/N] " ans

    if [[ "$ans" == [yY] ]]; then
        docker stop $ids
    else
        echo "已取消"
        return 1
    fi
}

# 解压：支持常见格式
ex() {
    local file="$1"

    if [[ -z "$file" ]]; then
        echo "用法: ex <压缩文件>"
        return 2
    fi

    if [[ ! -f "$file" ]]; then
        echo "错误: 不是有效文件: $file"
        return 1
    fi

    case "$file" in
        *.tar|*.tar.gz|*.tgz|*.tar.bz2|*.tar.xz|*.tar.zst)
            tar -xaf "$file"
            ;;

        *.gz)
            gunzip -- "$file"
            ;;

        *.bz2)
            bunzip2 -- "$file"
            ;;

        *.zip)
            command -v unzip >/dev/null 2>&1 || {
                echo "错误: 未找到 unzip 命令"
                return 127
            }
            unzip -- "$file"
            ;;

        *.rar)
            command -v unrar >/dev/null 2>&1 || {
                echo "错误: 未找到 unrar 命令"
                return 127
            }
            unrar x "$file"
            ;;

        *.7z)
            command -v 7z >/dev/null 2>&1 || {
                echo "错误: 未找到 7z 命令"
                return 127
            }
            7z x "$file"
            ;;

        *.Z)
            uncompress -- "$file"
            ;;

        *.deb)
            command -v ar >/dev/null 2>&1 || {
                echo "错误: 未找到 ar 命令"
                return 127
            }
            ar x "$file"
            ;;

        *)
            echo "错误: 无法识别的压缩格式: $file"
            return 2
            ;;
    esac
}

# mkcd：创建目录并进入
mkcd() {
    if [[ -z "$1" ]]; then
        echo "用法: mkcd <目录>"
        return 2
    fi

    mkdir -p -- "$1" && cd -- "$1" || return
}

# 查找进程并显示（带 PID）
pf() {
    if [[ -z "$1" ]]; then
        echo "用法: pf <关键字>"
        echo "示例: pf nginx"
        return 2
    fi

    ps aux | grep -F -i --color=auto -- "$1" | grep -F -v -- "grep -F -i"
}

# 查看端口占用
whichport() {
    if [[ -z "$1" ]]; then
        echo "用法: whichport <端口>"
        echo "示例: whichport 80"
        return 2
    fi

    if [[ ! "$1" =~ ^[0-9]+$ ]]; then
        echo "错误: 端口必须是数字"
        return 2
    fi

    if ! command -v lsof >/dev/null 2>&1; then
        echo "错误: 未找到 lsof 命令"
        return 127
    fi

    lsof -i :"$1"
}

# 查看某服务的日志（最近 N 行，默认 100）
svclog() {
    local svc="$1"
    local lines="${2:-100}"

    if [[ -z "$svc" ]]; then
        echo "用法: svclog <服务名> [行数]"
        echo "示例: svclog nginx 200"
        return 2
    fi

    if [[ ! "$lines" =~ ^[0-9]+$ ]]; then
        echo "错误: 行数必须是数字"
        return 2
    fi

    if ! command -v journalctl >/dev/null 2>&1; then
        echo "错误: 未找到 journalctl 命令"
        return 127
    fi

    journalctl -u "$svc" -n "$lines" --no-pager
}

# 快速备份文件（追加时间戳）
bak() {
    if [[ -z "$1" ]]; then
        echo "用法: bak <文件>"
        return 2
    fi

    if [[ ! -e "$1" ]]; then
        echo "错误: 文件不存在: $1"
        return 1
    fi

    if [[ -d "$1" ]]; then
        echo "错误: bak 只用于备份普通文件，不处理目录: $1"
        return 1
    fi

    cp -v -- "$1" "${1}.bak.$(date +%Y%m%d_%H%M%S)"
}

# 统计目录下各扩展名文件数量
ext-count() {
    local dir="${1:-.}"

    if [[ ! -d "$dir" ]]; then
        echo "错误: 不是有效目录: $dir"
        return 1
    fi

    find "$dir" -type f -name '*.*' |
        awk -F. '{print $NF}' |
        sort |
        uniq -c |
        sort -rn
}

# SSH 隧道快捷方式
# 用法: tunnel <本地端口> <远端主机> <远端端口> [跳板机]
tunnel() {
    local local_port="$1"
    local remote_host="$2"
    local remote_port="$3"
    local jump_host="${4:-}"
    if [[ -n "$jump_host" ]]; then
        ssh -J "$jump_host" -L "${local_port}:${remote_host}:${remote_port}" -N "$remote_host"
    else
        ssh -L "${local_port}:localhost:${remote_port}" -N "$remote_host"
    fi
}

# 等待某主机端口开放（用于脚本等待服务启动）
wait-port() {
    local host="$1"
    local port="$2"
    local timeout="${3:-60}"

    if [[ -z "$host" || -z "$port" ]]; then
        echo "用法: wait-port <主机> <端口> [超时秒数]"
        echo "示例: wait-port 127.0.0.1 3306 30"
        return 2
    fi

    if [[ ! "$port" =~ ^[0-9]+$ ]]; then
        echo "错误: 端口必须是数字"
        return 2
    fi

    if [[ ! "$timeout" =~ ^[0-9]+$ ]]; then
        echo "错误: 超时时间必须是数字"
        return 2
    fi

    if ! command -v nc >/dev/null 2>&1; then
        echo "错误: 未找到 nc 命令"
        return 127
    fi

    echo "等待 ${host}:${port} 开放（超时 ${timeout}s）..."

    local start=$SECONDS
    until nc -z "$host" "$port" 2>/dev/null; do
        if (( SECONDS - start >= timeout )); then
            echo "超时！${host}:${port} 仍未开放"
            return 1
        fi
        sleep 1
    done

    echo "${host}:${port} 已开放（耗时 $((SECONDS - start))s）"
}

# 快速查看系统概况
sysinfo() {
    echo "=== 系统信息 ==="
    echo "主机名:  $(hostname -f)"
    echo "系统:    $(cat /etc/os-release 2>/dev/null | grep PRETTY_NAME | cut -d'"' -f2)"
    echo "内核:    $(uname -r)"
    echo "运行时间:$(uptime -p)"
    echo "负载:    $(uptime | awk -F'load average:' '{print $2}')"
    echo ""
    echo "=== CPU ==="
    echo "核心数:  $(nproc)"
    grep "model name" /proc/cpuinfo | head -1 | awk -F': ' '{print "型号:   "$2}'
    echo ""
    echo "=== 内存 ==="
    free -h
    echo ""
    echo "=== 磁盘 ==="
    df -hT | grep -v tmpfs | grep -v devtmpfs
    echo ""
    echo "=== 网络接口 ==="
    ip -br addr
}

# ============================================================
#  个人配置文件（不被覆盖，建议在此添加私人 alias）
# ============================================================
[[ -f ~/.bashrc-local ]] && source ~/.bashrc-local
```
