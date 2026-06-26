---
title: Git 环境配置与认证
date: 2026-05-22
tags:
  - Git
  - SSH
  - GitHub
  - 配置
aliases:
  - Git 初始配置
  - Git 环境配置
  - GitHub SSH 配置
---

# 1. 配置目标

Git 初始配置要解决三个问题：

| 问题 | 对应配置 | 影响 |
|---|---|---|
| 提交是谁做的 | `user.name` / `user.email` | 写入 commit 作者信息 |
| 如何连接远程仓库 | SSH 或 HTTPS + PAT | 决定 clone、pull、push 的认证方式 |
| 当前仓库推到哪里 | `git remote` | 决定本地仓库与远程仓库的关联 |

> [!summary]
> 日常开发优先使用 **SSH**；临时环境、CI/CD 或 SSH 不可用时再使用 **Personal Access Token**。

# 2. 用户身份配置

## 2.1 全局身份

全局配置适用于当前系统用户下的所有 Git 仓库。

```bash
git config --global user.name "Your Name"
git config --global user.email "your.email@example.com"
```

查看全局配置：

```bash
git config --global --list
```

## 2.2 仓库级身份

仓库级配置只对当前仓库生效，优先级高于全局配置。

```bash
git config user.name "Project Name"
git config user.email "project@example.com"
```

适合以下场景：

- 公司项目和个人项目使用不同邮箱
- 开源贡献需要使用 GitHub 认证邮箱
- 多个组织要求不同提交身份

> [!warning]
> 提交身份会写入 commit 历史。提交前建议用 `git config user.email` 确认当前仓库使用的邮箱。

# 3. SSH 认证

## 3.1 SSH 的作用

**SSH**（Secure Shell）通过密钥对完成认证。配置后，Git 可以免密码访问 GitHub、GitLab、Gitee 等远程仓库。

```text
本机私钥  <----匹配---->  平台公钥
```

| 文件 | 说明 | 是否可公开 |
|---|---|---|
| `id_ed25519` | 私钥 | 否 |
| `id_ed25519.pub` | 公钥 | 是 |

## 3.2 安装 OpenSSH

```bash
# Arch Linux
sudo pacman -S openssh

# Ubuntu / Debian
sudo apt install openssh-client

# macOS
brew install openssh
```

Windows 通常已内置 OpenSSH，可用 PowerShell 检查：

```powershell
Get-Command ssh
```

## 3.3 生成 SSH 密钥

推荐使用 Ed25519：

```bash
ssh-keygen -t ed25519 -C "your_email@example.com"
```

| 算法 | 命令 | 适用场景 |
|---|---|---|
| Ed25519 | `ssh-keygen -t ed25519 -C "email"` | 推荐，安全且密钥短 |
| RSA | `ssh-keygen -t rsa -b 4096 -C "email"` | 兼容旧系统 |

> [!warning]
> 私钥等同于身份凭证，绝不能分享。若私钥泄露，应立即删除平台上的旧公钥并重新生成密钥对。

## 3.4 添加公钥到 GitHub

查看公钥内容：

```bash
cat ~/.ssh/id_ed25519.pub
```

复制完整输出后，在 GitHub 中添加：

1. 打开 GitHub 设置。
2. 进入 **SSH and GPG keys**。
3. 点击 **New SSH key**。
4. 粘贴公钥并保存。

## 3.5 验证 SSH 连接

```bash
ssh -T git@github.com
```

成功时会看到类似提示：

```text
Hi <username>! You've successfully authenticated, but GitHub does not provide shell access.
```

排查时增加详细日志：

```bash
ssh -T -v git@github.com
```

## 3.6 配置 SSH 端口

若默认端口连接超时，可编辑 `~/.ssh/config`：

```text
Host github.com
  HostName ssh.github.com
  Port 443
  User git
```

> [!tip]
> `443` 端口常用于绕过网络环境对 SSH 默认 `22` 端口的限制。

## 3.7 Git for Windows 指定 OpenSSH

如果 Git for Windows 自带 SSH 与系统 OpenSSH 冲突，可手动指定：

```bash
git config --global core.sshCommand "C:/PROGRA~1/OpenSSH/ssh.exe"
```

`PROGRA~1` 是 `Program Files` 的短路径写法，可避免空格带来的解析问题。

# 4. Personal Access Token

## 4.1 PAT 的定位

**Personal Access Token**（PAT）是 GitHub 对 HTTPS 密码认证的替代方式。

| 方式 | 优点 | 局限 |
|---|---|---|
| SSH | 配置一次后长期免密 | 首次配置稍复杂 |
| PAT | 适合临时环境和自动化 | 令牌需要管理和轮换 |

适用场景：

- 临时访问私有仓库
- SSH 网络不通
- CI/CD 自动化拉取或推送
- 不方便配置 SSH 密钥的机器

## 4.2 生成 PAT

1. 打开 GitHub 的 Personal Access Tokens 设置页面。
2. 生成新 token。
3. 按用途选择最小权限。
4. 生成后立即保存。

> [!warning]
> Token 通常只显示一次。泄露后应立即撤销并重新生成。

## 4.3 使用 PAT

当远程 URL 使用 HTTPS 时：

```bash
git push origin main
# Username: your_username
# Password: ghp_xxxxxxxxxxxx
```

密码位置填入 token，而不是 GitHub 登录密码。

## 4.4 凭证存储

明文保存凭证：

```bash
git config --global credential.helper store
```

只在内存中缓存一段时间：

```bash
git config --global credential.helper "cache --timeout=3600"
```

> [!warning]
> `store` 会把凭证明文写入本机文件。个人可信设备可以使用；共享机器不建议使用。

# 5. 远程仓库配置

## 5.1 查看远程仓库

```bash
git remote -v
```

输出会显示 fetch 和 push 使用的 URL。

## 5.2 添加远程仓库

```bash
git remote add origin git@github.com:user/repo.git
```

`origin` 是默认远程名，不是强制要求，但团队协作中通常沿用这个命名。

## 5.3 切换 SSH 与 HTTPS

切换为 SSH：

```bash
git remote set-url origin git@github.com:user/repo.git
```

切换为 HTTPS：

```bash
git remote set-url origin https://github.com/user/repo.git
```

## 5.4 多远程仓库

同一份代码可同步到多个平台：

```bash
git remote add github git@github.com:user/repo.git
git remote add gitee git@gitee.com:user/repo.git
```

分别推送：

```bash
git push github main
git push gitee main
```

# 6. 配置检查清单

| 检查项 | 命令 |
|---|---|
| 查看全局配置 | `git config --global --list` |
| 查看当前仓库身份 | `git config user.name && git config user.email` |
| 查看 SSH 是否可用 | `ssh -T git@github.com` |
| 查看远程 URL | `git remote -v` |
| 查看远程详情 | `git remote show origin` |

# 7. 常见问题

## 7.1 SSH 认证成功但 push 失败

检查当前远程 URL 是否仍然是 HTTPS：

```bash
git remote -v
```

若是 HTTPS，切换为 SSH：

```bash
git remote set-url origin git@github.com:user/repo.git
```

## 7.2 提交作者邮箱错误

若尚未推送，可修改最近一次提交：

```bash
git config user.email "correct@example.com"
git commit --amend --reset-author
```

若已经推送到公共分支，不要轻易重写历史，优先保留现状或与团队确认。

## 7.3 Token 反复要求输入

检查凭证助手：

```bash
git config --global credential.helper
```

必要时重新配置凭证存储或清理系统凭证管理器中的旧 token。

# 8. 总结

> [!summary]
> Git 环境配置的核心是：**提交身份正确、认证方式稳定、远程 URL 明确**。SSH 是日常开发首选；PAT 是 HTTPS 和自动化场景的补充。配置完成后，再进入 [[02_Git_核心模型与日常操作]] 学习日常命令会更顺畅。
