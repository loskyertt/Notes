---
name: project-context-export
description: 'Analyze the current project and generate a structured Markdown project context summary including architecture decisions, completed work, pending tasks, important file modifications, technical rationale, and next-session onboarding information. Use when user asks to summarize the project, export project memory, continue work across sessions, or generate a handoff/context document.'
license: MIT
allowed-tools:
  - Bash
  - Read
  - Grep
  - Glob
---

# Project Context Export Skill

## Overview

该 Skill 用于：

- 总结当前项目状态
- 导出长期上下文记忆
- 生成可供后续新会话直接加载的项目 Markdown 文档
- 帮助 AI Agent 快速恢复上下文
- 用于多人协作交接（handoff）
- 用于阶段性开发归档

最终输出应为：

```text
PROJECT_CONTEXT.md
```

或：

```text
SESSION_SUMMARY.md
```

格式的结构化 Markdown 文档。

---

# 核心目标

生成一个：

- 可长期维护
- 可快速阅读
- 可供 Agent 恢复上下文
- 包含技术决策依据
- 包含当前进度与待办

的项目上下文文件。

---

# 工作流程

## 第一步：分析项目结构

必须优先分析：

```bash
pwd
ls
find . -maxdepth 2
```

重点识别：

- 核心源码目录
- 配置目录
- 构建系统
- 测试目录
- 文档目录
- 第三方依赖
- examples/demo/tools/scripts

---

## 第二步：分析 Git 历史

执行：

```bash
git log --oneline -20
git status
git diff
```

目标：

- 理解最近开发方向
- 提取关键决策
- 识别重要架构调整
- 识别未提交工作
- 识别当前开发阶段

---

## 第三步：分析核心文件

必须优先阅读：

- README.md
- CMakeLists.txt
- package.json
- pyproject.toml
- requirements.txt
- Cargo.toml
- go.mod

以及：

- 核心入口文件
- 主架构模块
- 当前高频修改文件

---

## 第四步：提取项目上下文

需要总结：

### 1. 项目目标

包括：

- 项目用途
- 核心业务目标
- 当前开发阶段
- 技术路线

---

### 2. 核心架构

包括：

- 模块划分
- 数据流
- 依赖关系
- 核心设计模式
- 系统层次结构

例如：

- Reactor
- MVC
- ECS
- Pipeline
- ProtoNet
- Encoder-Decoder
- Event-driven

等。

---

### 3. 关键技术决策

必须记录：

- 为什么采用某方案
- 为什么放弃某方案
- 当前 compromise
- 已知限制
- 性能/复杂度权衡

重点关注：

- 架构演进
- 重构原因
- 性能优化方向
- 数据结构选择
- 网络模型
- 训练策略
- 推理流程

---

### 4. 已完成内容

必须按模块整理：

```md
## 已完成

### 网络层
- 完成 epoll Reactor 封装
- 支持 LT/ET 模式
- 新增连接池

### 日志系统
- 集成 g3log
- 支持异步 sink
```

要求：

- 不遗漏关键成果
- 说明技术意义
- 说明当前稳定性

---

### 5. 当前进行中

包括：

- 正在开发功能
- 当前实验
- 当前重构
- 当前调试方向
- 尚未验证部分

必须明确：

- 当前卡点
- 当前风险
- 当前假设

---

### 6. TODO / 后续计划

按优先级输出：

```md
## TODO

### 高优先级
- 修复 ET 模式连接泄漏
- 完成 PAP-FZS3D 原始实验复现

### 中优先级
- 增加单元测试
- 优化日志格式

### 低优先级
- 重构 examples 目录
```

---

### 7. 重要文件修改记录

输出：

```md
## 重要文件

| 文件 | 修改内容 | 原因 |
|---|---|---|
| protonet_QGPA.py | 新增高度残差原型推理 | 利用森林垂直结构先验 |
| CMakeLists.txt | 集成 gtest/g3log | 完善工程化 |
```

必须说明：

- 文件作用
- 最近修改原因
- 当前状态

---

### 8. 当前技术债务

包括：

- 临时方案
- hack 实现
- 未抽象代码
- 性能瓶颈
- 重复逻辑
- 已知 bug

---

### 9. 下一次会话建议

必须生成：

```md
## 下一次会话建议

建议优先继续：

1. xxx
2. xxx
3. xxx

推荐首先阅读：
- xxx.cpp
- xxx.py
- xxx.md
```

目标：

帮助下一个 Agent 快速进入状态。

---

# 输出格式要求

必须输出：

- 结构化 Markdown
- 清晰标题层级
- 可读性优先
- 技术术语准确
- 避免废话
- 避免泛泛而谈

---

# 推荐输出结构

```md
# 项目上下文总结

## 项目概述

## 当前开发阶段

## 核心架构

## 关键技术决策

## 已完成内容

## 当前进行中

## TODO

## 重要文件修改记录

## 技术债务

## 风险与限制

## 下一次会话建议
```

---

# 分析原则

## 必须遵守

- 必须基于真实代码分析
- 不允许凭空猜测
- 优先关注核心模块
- 优先关注近期修改
- 必须结合 git 历史
- 必须结合目录结构

---

# 高价值分析重点

Agent 应特别关注：

## 架构层

- 模块边界
- 耦合关系
- 抽象层次
- 核心数据流

---

## 工程层

- 构建系统
- 测试体系
- CI/CD
- 依赖管理

---

## 算法层（如存在）

- 模型结构
- 推理流程
- loss 设计
- 数据处理 pipeline
- 训练策略
- benchmark

---

## 性能层

- 热路径
- cache
- IO
- concurrency
- GPU
- memory

---

# 安全约束

禁止：

- 修改代码
- 自动 commit
- 自动删除文件
- 自动 refactor
- 自动安装依赖

仅允许：

- 读取
- 分析
- 总结
- 生成 Markdown 文档

---

# 用户触发词

以下情况必须触发：

- “总结当前项目”
- “导出项目上下文”
- “生成项目记忆”
- “生成会话总结”
- “输出项目 Markdown 总结”
- “生成 handoff 文档”
- “总结关键决策”
- “生成下次会话上下文”

---

# 最终目标

让新的 Agent 或新的会话：

- 5 分钟内理解项目
- 快速恢复上下文
- 明确当前状态
- 理解关键决策
- 知道下一步做什么
