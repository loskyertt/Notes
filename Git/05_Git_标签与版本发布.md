---
title: Git 标签与版本发布
date: 2026-05-22
tags:
  - Git
  - Tag
  - Release
aliases:
  - Git Tag
  - Git 标签
  - Git 版本发布
---

# 1. 标签的作用

**Tag** 用于给某个 commit 打上稳定名称，常用于标记发布版本。

```text
A---B---C---D
        ^
      v1.0.0
```

与分支不同，标签通常不会移动。

| 对象 | 是否移动 | 典型用途 |
|---|---|---|
| Branch | 会随着新提交移动 | 持续开发 |
| Tag | 通常固定在某个提交 | 标记版本 |

> [!summary]
> 分支表示“正在发展的线”，标签表示“已经确认的重要版本点”。

# 2. 标签类型

## 2.1 轻量标签

轻量标签只是一个指向 commit 的引用。

```bash
git tag v1.0.0
```

适合临时标记、个人本地标记。

## 2.2 附注标签

附注标签是一个完整 Git 对象，包含标签作者、日期、说明信息，并可签名。

```bash
git tag -a v1.0.0 -m "Release version 1.0.0"
```

正式发布推荐使用附注标签。

## 2.3 类型对比

| 维度 | 轻量标签 | 附注标签 |
|---|---|---|
| 本质 | 指向 commit 的引用 | 独立 Git 对象 |
| 包含信息 | 只有提交指针 | 标签作者、日期、说明 |
| 是否可签名 | 否 | 是 |
| 推荐场景 | 临时标记 | 正式发布 |

> [!tip]
> 面向团队或公开发布时，优先使用 `git tag -a` 创建附注标签。

# 3. 标签基础操作

## 3.1 创建标签

```bash
# 在当前 commit 创建轻量标签
git tag v1.0.0

# 在当前 commit 创建附注标签
git tag -a v1.0.0 -m "Release version 1.0.0"

# 给历史 commit 打标签
git tag -a v0.9.0 <commit> -m "Release version 0.9.0"
```

## 3.2 查看标签

```bash
# 查看所有标签
git tag

# 按模式筛选
git tag -l "v1.*"

# 查看标签详情
git show v1.0.0
```

## 3.3 推送标签

```bash
# 推送单个标签
git push origin v1.0.0

# 推送所有本地标签
git push --tags
```

> [!warning]
> 普通 `git push` 不会自动推送本地标签。发布后需要显式推送标签。

## 3.4 删除标签

```bash
# 删除本地标签
git tag -d v1.0.0

# 删除远程标签
git push origin --delete v1.0.0
```

如果远程 Git 版本较旧，也可使用：

```bash
git push origin :refs/tags/v1.0.0
```

# 4. 语义化版本

常见版本号格式：

```text
vMAJOR.MINOR.PATCH
```

| 字段 | 含义 | 示例 |
|---|---|---|
| MAJOR | 不兼容变更 | `v2.0.0` |
| MINOR | 向后兼容的新功能 | `v1.3.0` |
| PATCH | 向后兼容的 Bug 修复 | `v1.3.2` |

预发布版本：

```text
v1.0.0-alpha.1
v1.0.0-beta.1
v1.0.0-rc.1
```

> [!tip]
> `rc` 表示 Release Candidate，通常用于正式发布前的候选版本。

# 5. 发布流程

## 5.1 标准发布流程

```bash
# 1. 切到主分支并同步
git switch main
git pull --rebase

# 2. 确认工作区干净
git status

# 3. 运行测试或构建
npm test
npm run build

# 4. 创建附注标签
git tag -a v1.2.0 -m "Release version 1.2.0"

# 5. 推送代码和标签
git push origin main
git push origin v1.2.0
```

不同技术栈的测试命令不同，核心原则是：**先确认主分支质量，再打标签**。

## 5.2 使用发布分支

适合需要发布前冻结代码的项目。

```bash
git switch main
git pull --rebase
git switch -c release/v1.2.0

# 修改版本号、更新 changelog、修复发布前 bug
git add -p
git commit -m "chore: prepare release v1.2.0"

git switch main
git merge --no-ff release/v1.2.0
git tag -a v1.2.0 -m "Release version 1.2.0"
git push origin main
git push origin v1.2.0
```

发布分支策略与 Git Flow 的关系见 [[04_Git_分支管理与协作策略]]。

## 5.3 发布检查清单

| 检查项 | 命令或动作 |
|---|---|
| 当前在正确分支 | `git branch --show-current` |
| 工作区干净 | `git status` |
| 已同步远程 | `git pull --rebase` |
| 测试通过 | 项目测试命令 |
| 版本号已更新 | 检查项目配置文件 |
| Changelog 已更新 | 检查 `CHANGELOG.md` |
| 标签为附注标签 | `git tag -a` |
| 标签已推送 | `git push origin <tag>` |

# 6. 常见问题

## 6.1 标签打错了 commit

若标签尚未推送：

```bash
git tag -d v1.2.0
git tag -a v1.2.0 <correct_commit> -m "Release version 1.2.0"
```

若标签已经推送，需要先确认团队没有依赖旧标签，再删除远程标签并重新推送。

```bash
git tag -d v1.2.0
git push origin --delete v1.2.0
git tag -a v1.2.0 <correct_commit> -m "Release version 1.2.0"
git push origin v1.2.0
```

> [!warning]
> 已发布标签不应随意改写。公开项目中移动标签会破坏使用者对版本的信任。

## 6.2 忘记推送标签

检查本地和远程是否一致：

```bash
git tag
git ls-remote --tags origin
```

推送缺失标签：

```bash
git push origin v1.2.0
```

## 6.3 拉取远程标签

```bash
# fetch 默认通常会带回可达标签
git fetch origin

# 显式拉取所有标签
git fetch --tags
```

# 7. 标签与 GitHub Release

Git 标签是 Git 层面的版本标记；GitHub Release 是平台层面的发布页。

| 对象 | 所在层级 | 内容 |
|---|---|---|
| Git Tag | Git 仓库 | 指向 commit 的版本标记 |
| GitHub Release | GitHub 平台 | 发布说明、二进制附件、下载入口 |

常见流程：

1. 本地创建并推送 tag。
2. 在 GitHub 上基于 tag 创建 Release。
3. 编写 Release Notes。
4. 上传构建产物。

# 8. 总结

> [!summary]
> 标签用于把某个 commit 固定为版本点。正式发布优先使用附注标签，命名遵循语义化版本，并在测试通过、工作区干净、主分支同步后再创建和推送。
