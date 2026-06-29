---
title: Git 常用参数速查
date: 2026-05-22
tags:
  - Git
  - 参数
  - 速查
aliases:
  - Git 参数速查
  - Git 常用参数
---

# 1. 参数说明

Git 参数遵循 Unix 风格，但同一个短参数在不同命令里可能完全不同。

| 命令 | 参数 | 含义 |
|---|---|---|
| `git push -u` | `--set-upstream` | 设置上游分支 |
| `git add -u` | `--update` | 只更新 tracked 文件 |
| `git stash -u` | `--include-untracked` | 保存未跟踪文件 |

> [!warning]
> 不存在“Git 中 `-u` 永远代表某个含义”。参数必须放回具体命令中理解。

---

# 2. 高频短参数总览

| 短参数 | 常见长参数 | 常见含义 | 注意 |
|---|---|---|---|
| `-m` | `--message` | 指定消息 | 常见于 `commit` / `tag` / `stash` |
| `-a` | `--all` | 全部 | 不同命令范围不同 |
| `-u` | 视命令而定 | upstream / update / untracked | ==高频混淆== |
| `-f` | `--force` | 强制执行 | 风险高 |
| `-d` | `--delete` | 删除 | 通常只删除安全对象 |
| `-D` | `--delete --force` | 强制删除 | 常见于分支 |
| `-c` | `--create` | 创建 | 常见于 `switch` |
| `-p` | `--patch` / `--prune` | 分块选择 / 清理引用 | 视命令而定 |
| `-r` | `--remote` | 远程 | 常见于 `branch` |
| `-i` | `--interactive` | 交互式 | 常见于 `rebase` / `add` |
| `-v` | `--verbose` | 详细输出 | 常见于诊断 |

---

# 3. `-u` 与 Upstream

## 3.1 Upstream 的定义

**Upstream** 是本地分支与远程分支之间的跟踪关系。设置后，Git 知道当前分支默认应该向哪里 `push`，又应该从哪里 `pull`。

```bash
git push -u origin feature/login
```

这条命令做了两件事：

1. 把本地 `feature/login` 推送到 `origin`。
2. 建立当前分支与 `origin/feature/login` 的跟踪关系。

## 3.2 设置 Upstream 的收益

| 操作 | 有 upstream | 无 upstream |
|---|---|---|
| 推送 | `git push` | `git push origin branch-name` |
| 拉取 | `git pull` | `git pull origin branch-name` |
| 查看状态 | `git status` 显示 ahead/behind | 无法判断默认远程对比对象 |

## 3.3 完整示例

```bash
# 创建分支
git switch -c exp/hproto-ablation

# 推送但不建立跟踪
git push origin exp/hproto-ablation

# 推送并建立跟踪
git push -u origin exp/hproto-ablation

# 后续可直接推送
git push
```

## 3.4 查看与修改 Upstream

```bash
# 查看所有分支的 upstream
git branch -vv

# 设置当前分支 upstream
git branch -u origin/main

# 设置指定分支 upstream
git branch -u origin/main main

# 取消 upstream
git branch --unset-upstream
```

## 3.5 Upstream 的配置本质

设置 upstream 后，Git 会在 `.git/config` 写入类似配置：

```ini
[branch "main"]
    remote = origin
    merge = refs/heads/main
```

> [!tip]
> 分支重命名后，旧 upstream 不会自动变更。若 `git branch -vv` 显示异常，应重新设置 `git branch -u`。

---

# 4. 按命令速查

## 4.1 `git add`

| 命令 | 含义 | 范围 |
|---|---|---|
| `git add .` | 添加当前目录下所有变更 | 新增、修改、删除 |
| `git add -A` | 添加整个仓库所有变更 | 新增、修改、删除 |
| `git add -u` | 更新已跟踪文件 | 修改、删除，不含新文件 |
| `git add -p` | 分块暂存 | 手动选择 hunk |

> [!tip]
> 在仓库根目录执行时，`git add .` 与 `git add -A` 通常效果一致；在子目录执行时，`-A` 更明确覆盖整个仓库。

## 4.2 `git commit`

| 参数 | 含义 | 注意 |
|---|---|---|
| `-m "msg"` | 指定提交信息 | 适合简短提交 |
| `-a` | 自动暂存 tracked 文件 | 不包含新文件 |
| `--amend` | 修改最近一次提交 | 会重写提交哈希 |
| `--no-edit` | 保留原提交信息 | 常与 `--amend` 配合 |

示例：

```bash
git commit -m "fix: correct config path"
git commit -am "docs: update README"
git commit --amend --no-edit
```

## 4.3 `git push`

| 参数 | 含义 | 注意 |
|---|---|---|
| `-u` | 设置 upstream | 首次推送分支常用 |
| `--tags` | 推送所有标签 | 发布版本时使用 |
| `-f` / `--force` | 强制覆盖远程 | 危险 |
| `--force-with-lease` | 更安全的强推 | 推荐替代 `--force` |

> [!warning]
> `git push --force` 会覆盖远程历史。需要强推时优先使用 `--force-with-lease`。

## 4.4 `git pull`

| 参数 | 含义 | 适用场景 |
|---|---|---|
| `--rebase` | 拉取后用 rebase 整合 | 保持个人分支线性 |
| `--ff-only` | 只允许快进 | 防止意外产生 merge commit |

常用配置：

```bash
git config --global pull.rebase true
```

## 4.5 `git fetch`

| 参数 | 含义 |
|---|---|
| `--all` | 拉取所有 remote |
| `-p` / `--prune` | 清理远程已删除分支的本地引用 |

```bash
git fetch -p
```

适合清理 `git branch -r` 中已经失效的远程分支。

## 4.6 `git branch`

| 参数 | 含义 |
|---|---|
| `-a` | 显示本地和远程分支 |
| `-r` | 只显示远程分支 |
| `-d` | 删除已合并本地分支 |
| `-D` | 强制删除本地分支 |
| `-m old new` | 重命名分支 |
| `-vv` | 显示 upstream 和 ahead/behind |

## 4.7 `git switch`

| 参数 | 含义 |
|---|---|
| `-c` | 创建并切换到新分支 |

```bash
git switch main
git switch -c feature/login
git switch -c fix/bug-101 9309c5e
```

## 4.8 `git stash`

| 参数 | 含义 | 覆盖范围 |
|---|---|---|
| 无参数 | 保存 tracked 文件变更 | tracked |
| `-u` | 包含未跟踪文件 | tracked + untracked |
| `-a` | 包含忽略文件 | tracked + untracked + ignored |
| `-m` | 添加说明 | 便于后续识别 |

```bash
git stash push -u -m "WIP: save unfinished refactor"
```

## 4.9 `git log`

| 参数 | 含义 |
|---|---|
| `--oneline` | 单行显示 |
| `--graph` | 显示拓扑图 |
| `--all` | 显示所有分支历史 |
| `--stat` | 显示文件统计 |
| `-p` | 显示 patch |
| `--grep` | 按提交信息搜索 |

推荐组合：

```bash
git log --oneline --graph --all
```

## 4.10 `git diff`

| 命令 | 比较对象 |
|---|---|
| `git diff` | 工作区 vs 暂存区 |
| `git diff --staged` | 暂存区 vs HEAD |
| `git diff branch1..branch2` | 两个分支 |
| `git diff --stat` | 差异文件统计 |

## 4.11 `git reset`

| 参数 | HEAD | 暂存区 | 工作区 | 风险 |
|---|---|---|---|---|
| `--soft` | 移动 | 保留 | 保留 | 低 |
| `--mixed` | 移动 | 重置 | 保留 | 低 |
| `--hard` | 移动 | 重置 | 丢弃 | 高 |

> [!warning]
> `git reset --hard` 会丢弃未提交修改。执行前先确认是否需要 `git stash` 或创建备份分支。

## 4.12 `git restore`

| 命令 | 作用 |
|---|---|
| `git restore <file>` | 丢弃工作区修改 |
| `git restore .` | 丢弃当前目录下工作区修改 |
| `git restore --staged <file>` | 取消暂存 |
| `git restore --source=<commit> <file>` | 从指定提交恢复文件 |

## 4.13 `git rebase`

| 参数 | 含义 |
|---|---|
| `-i` / `--interactive` | 交互式 rebase |
| `--continue` | 解决冲突后继续 |
| `--abort` | 放弃 rebase |
| `--skip` | 跳过当前提交 |

交互式 rebase 常用操作：

| 操作 | 缩写 | 含义 |
|---|---|---|
| `pick` | `p` | 保留提交 |
| `reword` | `r` | 修改提交信息 |
| `squash` | `s` | 合并到前一个提交并保留信息 |
| `fixup` | `f` | 合并到前一个提交并丢弃信息 |
| `drop` | `d` | 删除提交 |

# 5. 场景速查

| 场景 | 命令 |
|---|---|
| 查看仓库状态 | `git status -s` |
| 创建功能分支 | `git switch -c feature/xxx` |
| 首次推送分支 | `git push -u origin feature/xxx` |
| 临时保存含新文件的修改 | `git stash push -u -m "WIP: xxx"` |
| 查看分支追踪状态 | `git branch -vv` |
| 清理失效远程分支 | `git fetch -p` |
| 查看历史拓扑 | `git log --oneline --graph --all` |
| 整理最近 5 个提交 | `git rebase -i HEAD~5` |
| 安全强推 | `git push --force-with-lease` |

# 6. 总结

> [!summary]
> 参数速查的核心不是背短参数，而是按命令记忆行为边界。最容易混淆的是 `-u`、`-a`、`-p`：它们在不同命令中含义不同，使用前必须确认上下文。
