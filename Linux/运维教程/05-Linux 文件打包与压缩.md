---
title: Linux 文件打包与压缩
date: 2026-06-15
tags:
  - Linux
  - 运维
  - 压缩
  - tar
aliases:
  - Linux 压缩解压
  - tar 命令
---

# 1. 打包与压缩的区别

Linux 中 **打包** 和 **压缩** 是两个不同动作。

| 概念 | 作用 | 是否减小体积 | 常用工具 |
|---|---|---|---|
| 打包 | 多个文件合并为一个归档文件 | 否 | `tar` |
| 压缩 | 用算法减小单个文件体积 | 是 | `gzip`、`bzip2`、`xz` |
| 打包并压缩 | 先打包再压缩 | 是 | `tar -z/-j/-J` |
| 跨平台压缩 | 打包与压缩合并 | 是 | `zip` |

> [!summary]
> 多文件或目录在 Linux 中通常先用 `tar` 打包，再用 gzip、bzip2 或 xz 压缩。

# 2. 常见格式对比

| 格式 | 扩展名 | 压缩率 | 速度 | 适用场景 |
|---|---|---|---|---|
| tar | `.tar` | 无压缩 | 快 | 只归档、不压缩 |
| gzip | `.tar.gz`、`.tgz` | 中 | 快 | 最常用，兼容性最好 |
| bzip2 | `.tar.bz2`、`.tbz2` | 较高 | 较慢 | 需要更高压缩率 |
| xz | `.tar.xz` | 高 | 慢 | 发布包、镜像等体积敏感场景 |
| zip | `.zip` | 中 | 快 | 与 Windows/macOS 共享 |

# 3. tar 命令

## 3.1 选项结构

```bash
tar [操作模式] [压缩算法] [辅助选项] -f <归档文件> <源文件或目录>
```

操作模式通常三选一：

| 选项 | 含义 |
|---|---|
| `-c` | create，创建归档 |
| `-x` | extract，解包 |
| `-t` | list，查看归档内容 |
| `-r` | append，追加文件，仅未压缩 `.tar` 支持 |
| `-u` | update，追加比归档中更新的文件 |

压缩算法：

| 选项 | 工具 | 格式 |
|---|---|---|
| `-z` | gzip | `.tar.gz`、`.tgz` |
| `-j` | bzip2 | `.tar.bz2`、`.tbz2` |
| `-J` | xz | `.tar.xz` |

辅助选项：

| 选项 | 说明 |
|---|---|
| `-f <file>` | 指定归档文件名，后面必须紧跟文件名 |
| `-v` | 显示处理过程 |
| `-C <dir>` | 解包到指定目录 |
| `-p` | 保留权限和属主信息 |
| `--exclude=<pattern>` | 排除匹配文件或目录 |
| `--exclude-vcs` | 排除 `.git`、`.svn` 等版本控制目录 |

> [!warning]
> `-f` 后面必须紧跟归档文件名。习惯写法是把 `f` 放在选项串最后，例如 `tar -czvf archive.tar.gz dir`。

## 3.2 创建归档

```bash
# 只打包，不压缩
tar -cvf archive.tar file1 file2 dir1

# gzip，最常用
tar -czvf archive.tar.gz dir1

# bzip2，压缩率更高
tar -cjvf archive.tar.bz2 dir1

# xz，压缩率最高
tar -cJvf archive.tar.xz dir1

# 排除目录
tar -czvf archive.tar.gz project/ --exclude=".git" --exclude="node_modules"

# 备份时保留权限
sudo tar -czvpf etc-backup.tar.gz /etc
```

## 3.3 查看归档内容

```bash
# 查看 .tar 内容
tar -tvf archive.tar

# 查看 .tar.gz 内容
tar -tzvf archive.tar.gz

# 查看 .tar.bz2 内容
tar -tjvf archive.tar.bz2

# 查看 .tar.xz 内容
tar -tJvf archive.tar.xz
```

> [!tip]
> 解压陌生归档前先 `tar -tf` 查看内部路径，避免文件散落到当前目录或覆盖已有文件。

## 3.4 解包与解压

```bash
# 解压 .tar.gz 到当前目录
tar -xzvf archive.tar.gz

# 解压 .tar.bz2 到当前目录
tar -xjvf archive.tar.bz2

# 解压 .tar.xz 到当前目录
tar -xJvf archive.tar.xz

# 解包 .tar
tar -xvf archive.tar

# 解压到指定目录，目录需已存在
tar -xzvf archive.tar.gz -C /path/to/dest

# 只解压归档中的特定文件
tar -xzvf archive.tar.gz path/inside/archive/file.txt

# 较新的 tar 可自动识别压缩格式
tar -xvf archive.tar.gz
```

# 4. gzip 与 gunzip

`gzip` 只能压缩单个文件，默认会用 `.gz` 文件替换原文件。

```bash
# 压缩，原文件替换为 file.txt.gz
gzip file.txt

# 压缩并保留原文件
gzip -k file.txt

# 指定压缩级别，1 最快，9 压缩率最高
gzip -9 file.txt

# 解压
gunzip file.txt.gz
gzip -d file.txt.gz

# 查看压缩信息
gzip -l file.txt.gz

# 查看内容不解压
zcat file.txt.gz

# 搜索内容不解压
zgrep "keyword" file.txt.gz
```

# 5. bzip2 与 bunzip2

`bzip2` 压缩率通常高于 `gzip`，但速度更慢。

```bash
# 压缩
bzip2 file.txt

# 压缩并保留原文件
bzip2 -k file.txt

# 指定压缩级别
bzip2 -9 file.txt

# 解压
bunzip2 file.txt.bz2
bzip2 -d file.txt.bz2

# 查看内容不解压
bzcat file.txt.bz2

# 搜索内容不解压
bzgrep "keyword" file.txt.bz2
```

# 6. xz 与 unxz

`xz` 压缩率高，速度慢，适合分发安装包或长期归档。

```bash
# 压缩
xz file.txt

# 压缩并保留原文件
xz -k file.txt

# 指定压缩级别
xz -9 file.txt

# 解压
unxz file.txt.xz
xz -d file.txt.xz

# 查看内容不解压
xzcat file.txt.xz

# 查看压缩信息
xz -l file.txt.xz
```

# 7. zip 与 unzip

`zip` 可直接压缩多个文件或目录，适合跨平台共享。

```bash
# 压缩多个文件
zip archive.zip file1.txt file2.txt

# 递归压缩目录
zip -r archive.zip dir1

# 指定压缩级别
zip -9 -r archive.zip dir1

# 向已有 zip 添加文件
zip archive.zip newfile.txt

# 查看内容
unzip -l archive.zip

# 解压到当前目录
unzip archive.zip

# 解压到指定目录
unzip archive.zip -d /path/to/dest

# 只解压指定文件
unzip archive.zip specific_file.txt

# 静默解压
unzip -q archive.zip

# 跳过已存在文件
unzip -n archive.zip

# 覆盖已存在文件
unzip -o archive.zip
```

# 8. 常用命令速查

| 场景 | 命令 |
|---|---|
| 创建 `.tar.gz` | `tar -czvf out.tar.gz dir` |
| 创建 `.tar.bz2` | `tar -cjvf out.tar.bz2 dir` |
| 创建 `.tar.xz` | `tar -cJvf out.tar.xz dir` |
| 查看归档内容 | `tar -tvf archive.tar.gz` |
| 解压到当前目录 | `tar -xvf archive.tar.gz` |
| 解压到指定目录 | `tar -xvf archive.tar.gz -C /dest` |
| 压缩单文件并保留原文件 | `gzip -k file` |
| 查看 gzip 内容 | `zcat file.gz` |
| 搜索 gzip 内容 | `zgrep "keyword" file.gz` |
| 创建 zip | `zip -r out.zip dir` |
| 查看 zip 内容 | `unzip -l out.zip` |

# 9. 易错点总结

> [!summary]
> 本篇核心：Linux 服务备份优先 `tar.gz`，跨平台共享优先 `zip`，陌生压缩包解压前先查看内容。

- `tar` 的 `-f` 后面必须紧跟归档文件名。
- `gzip`、`bzip2`、`xz` 默认会替换原文件，保留原文件需加 `-k`。
- 解压到指定目录前，目标目录通常需要先存在。
- 备份系统配置时常用 `tar -p` 保留权限。
- 对陌生归档先 `tar -tf` 或 `unzip -l`，避免覆盖文件或污染当前目录。
