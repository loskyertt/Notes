---
title: 终端 zsh 配置指南
date: 2024-07-19 21:40:05
tags:
  - linux/系统配置
  - zsh
  - 终端
  - 代理
---

# 1. 终端代理设置

> [!tip] Tun 模式免配置
> 如果开启了代理软件的 Tun 模式，就可以不用管这个。

建议加上，在进行通过终端的下载、更新系统、conda 下载或者 `git clone` 时，能走代理来提高下载速度。

> [!warning] Docker 需要单独配置
> `docker` 需要单独配置一套代理，参见 [[Docker_配置#1.3 代理配置]]。

`zsh` 和 `bash` 都可以用这种方式。bash 需要在 `.bashrc` 中修改，zsh 在 `.zshrc` 中修改。

```bash
nano ~/.zshrc
```

在 `.zshrc` 中添加以下内容：

```bash
proxy(){
  export http_proxy="http://127.0.0.1:7890"
  export https_proxy="http://127.0.0.1:7890"
  echo "HTTP Proxy on"
}

noproxy(){
  unset http_proxy
  unset https_proxy
  echo "HTTP Proxy off"
}
```

> [!warning] 根据实际情况填写自己的代理端口。

```bash
source ~/.zshrc
```

通过在终端输入 `proxy` 或者 `noproxy` 来开启或关闭代理。

查看终端代理地址：

```bash
env | grep -i proxy
```

---

# 2. zsh 配置

> [!tip] 推荐方案
> 推荐使用 [zimfw](https://zimfw.sh/) 插件管理工具来配置 zsh。

## 2.1 切换 zsh 为默认终端

确保已经安装了 `zsh`：

```bash
chsh -s $(which zsh)
```

> [!info] 通常要重启系统才会生效。

验证默认 shell：

```bash
echo $SHELL
```

## 2.2 自动配置（推荐）

1. 安装 `zimfw`：

```bash
# 通过 curl
curl -fsSL https://raw.githubusercontent.com/zimfw/install/master/install.zsh | zsh

# 通过 wget
wget -nv -O - https://raw.githubusercontent.com/zimfw/install/master/install.zsh | zsh
```

2. 配置插件。在 `~/.zimrc` 中添加：

```zsh
zmodule zdharma-continuum/fast-syntax-highlighting
zmodule zimfw/direnv
```

> [!warning] 需要把原来的 syntax highlighting 插件删了。

执行 `zimfw install` 安装好并重启终端后即可使用。

3. 安装 [starship](https://starship.rs/zh-cn/) 进行美化：

```bash
sudo pacman -S starship
```

在 `.zshrc` 中填入：

```zsh
eval "$(starship init zsh)"
```

## 2.3 手动配置

### 2.3.1 zsh prompt

| Code | Info |
|---|---|
| `%T` | 系统时间（时：分） |
| `%*` | 系统时间（时：分：秒） |
| `%D` | 系统日期（年-月-日） |
| `%n` | 用户名称 |
| `%B` ... `%b` | 粗体打印 |
| `%U` ... `%u` | 下划线打印 |
| `%d` | 当前工作目录 |
| `%~` | 当前目录相对于 `~` 的相对路径 |
| `%M` | 计算机的主机名 |
| `%m` | 计算机的主机名（在第一个句号之前截断） |
| `%l` | 当前的 tty |
| `%F{色码}` | 设定某个颜色的开始 |
| `%f` | 设定成预设的样式 |

推荐配置如下：

```bash
# ~/.zshrc
function git_branch_name()
{
  branch=$(git symbolic-ref HEAD 2> /dev/null | awk 'BEGIN{FS="/"} {print $NF}')
  if [[ $branch == "" ]];
  then
    :
  else
    echo '- ('$branch')'
  fi
}

setopt prompt_subst

precmd_get_conda_env_name() {
if [[ -n $CONDA_PREFIX ]]; then
    if [[ $(basename $CONDA_PREFIX) == "miniconda3" ]]; then
        CONDA_ENV="base"
    else
        CONDA_ENV="$(basename $CONDA_PREFIX)"
    fi
else
    CONDA_ENV=""
fi
}
precmd_functions+=( precmd_get_conda_env_name )
precmd_update_prompt() {
    PROMPT=$'\n'"%B%F{119}[%F{green}%D{%m/%d %H:%M}%F{119}] %F{red}%n%F{blue}@%F{yellow}%m%F{black}:%F{cyan}%~%B%F{70}$(git_branch_name)"$'\n'"%F{magenta}$CONDA_ENV%F{119} ➜ %f%b"
}
precmd_functions+=( precmd_update_prompt )
```

> [!note] 参考链接
> - [Show current branch on prompt on zsh shell](https://stackoverflow.com/questions/67587439/show-current-branch-on-prompt-on-zsh-shell)
> - [Simplest ZSH Prompt Configs for Git Branch Name](https://medium.com/pareture/simplest-zsh-prompt-configs-for-git-branch-name-3d01602a6f33)
> - [How to modify the anaconda environment prompt in zsh](https://unix.stackexchange.com/questions/656045/how-to-modify-the-anaconda-environment-prompt-in-zsh)

### 2.3.2 插件配置

- **备份（避免配置失败，最好备份下）：**

```bash
cp ~/.zshrc ~/.zshrc.backup
```

- **创建配置文件夹：**

```bash
mkdir -p .zsh/plugins
mv .zshrc .zsh/
mv .zsh_history .zsh/
```

> [!info] 若没有 `.zsh_history`，需要用 `touch` 指令创建：`touch ~/.zsh_history`

- **编辑 `.zsh/.zshrc`，加上：**

```bash
### ZSH HOME
export ZSH=$HOME/.zsh

### ---- history config ----------
export HISTFILE=$ZSH/.zsh_history
export HISTSIZE=10000
export SAVEHIST=10000
setopt HIST_IGNORE_ALL_DUPS
setopt HIST_FIND_NO_DUPS
```

- **安装插件：**

进入安装插件的目录：

```bash
cd ~/.zsh/plugins
```

插件一：语法高亮

```bash
git clone https://github.com/zdharma-continuum/fast-syntax-highlighting.git
```

插件二：自动建议

```bash
git clone https://github.com/zsh-users/zsh-autosuggestions.git
```

插件三：自动补全

```bash
git clone https://github.com/zsh-users/zsh-completions.git
```

- **在 `.zshrc` 中添加：**

```bash
source $ZSH/plugins/fast-syntax-highlighting/fast-syntax-highlighting.plugin.zsh
fpath=($ZSH/plugins/zsh-completions/src $fpath)

# zsh-autosuggestions:config
source $ZSH/plugins/zsh-autosuggestions/zsh-autosuggestions.zsh
ZSH_AUTOSUGGEST_HIGHLIGHT_STYLE="fg=#ff00ff,bg=cyan,bold,underline"
ZSH_AUTOSUGGEST_STRATEGY=(history completion)
ZSH_AUTOSUGGEST_BUFFER_MAX_SIZE=20
```

- **创建符号链接：**

确认 `~` 目录下没有 `.zshrc` 文件，如果有就 `rm .zshrc`，然后：

```bash
ln -s ~/.zsh/.zshrc ~/.zshrc
```

重新加载配置文件：

```bash
source ~/.zshrc
```

可以通过 `ls -la` 来查看是否链接成功。

---

# 3. 问题汇总

## 3.1 历史命令问题

出现这种情况：

```bash
zsh: corrupt history file /home/sky/.zsh/.zsh_history
```

> [!bug] 原因
> 有的时候系统因为某些原因强行启动时会破坏 zsh 的历史文件。

解决办法：

```bash
cp ~/.zsh_history ~/.zsh_history_backup
rm ~/.zsh_history
strings -eS ~/.zsh_history_backup > ~/.zsh_history
fc -R ~/.zsh_history
```

如果上述步骤没有解决问题，可能是因为 `.zsh_history` 文件严重损坏。在这种情况下，需要放弃旧的历史记录并创建一个新的文件。

## 3.2 安装 oh-my-zsh 可能出现的问题

> [!warning] oh-my-zsh 会覆盖配置
> 安装完 `oh-my-zsh` 后终端中有些命令可能不能使用：编辑 `.zshrc` 发现里面内容都被替换掉了，之前的配置内容都被转移到一个叫 `.zshrc.pre-oh-my-zsh` 文件中。
