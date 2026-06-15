---
title: Linux 文件与目录基本操作
date: 2026-06-15
tags:
  - Linux
  - 运维
  - 文件系统
  - 命令行
aliases:
  - Linux 文件操作
  - 文件与目录操作
---

# 1. 操作范围总览

Linux 文件操作围绕四类任务展开：

| 任务 | 高频命令 | 关注点 |
|---|---|---|
| 创建 | `touch`、`mkdir` | 是否需要递归创建父目录 |
| 查看 | `ls`、`cat`、`less`、`head`、`tail` | 文件大小、分页、日志追踪 |
| 变更 | `cp`、`mv`、`rm`、`rmdir` | 覆盖、递归、删除不可恢复 |
| 查找 | `find`、`locate`、`which`、`whereis` | 实时性、匹配条件、执行动作 |

> [!summary]
> 文件操作的核心习惯：先确认路径，再执行变更；涉及删除、覆盖、递归时优先加 `-i` 或先用只读命令预览。

# 2. 创建文件与目录

## 2.1 `touch`

`touch` 主要用于更新时间戳；目标不存在时会创建空文件。

```bash
# 创建单个空文件
touch file.txt

# 创建多个文件
touch file1.txt file2.txt file3.txt

# 更新已有文件的访问时间和修改时间
touch existing_file.txt
```

> [!info]
> `touch` 不适合写入内容，只适合创建空文件或更新时间戳。写入内容可使用编辑器、重定向或脚本。

## 2.2 `mkdir`

```bash
# 创建目录
mkdir logs

# 创建多个目录
mkdir dir1 dir2 dir3

# 递归创建多级目录
mkdir -p /opt/myapp/logs
```

`-p` 会自动创建缺失的父目录，且目标目录已存在时不报错，适合脚本使用。

# 3. 查看目录内容

## 3.1 `ls`

```bash
# 列出当前目录
ls

# 长格式
ls -l

# 显示隐藏文件
ls -a

# 人类可读大小
ls -lh

# 按修改时间排序，新文件在前
ls -lt

# 递归列出子目录
ls -R
```

`ls -l` 输出结构：

```text
-rwxr-xr-- 1 alice dev 4096 Jan 1 12:00 script.sh
│           │ │     │   │              └── 文件名
│           │ │     │   └──────────────── 文件大小
│           │ │     └──────────────────── 所属组
│           │ └────────────────────────── 所有者
│           └──────────────────────────── 硬链接数
└──────────────────────────────────────── 文件类型与权限
```

## 3.2 路径定位

```bash
# 当前目录
pwd

# 查看绝对路径
realpath file.txt

# 查看符号链接最终目标
readlink -f link_name
```

# 4. 查看文件内容

## 4.1 小文件查看

```bash
# 输出完整文件
cat file.txt

# 拼接多个文件输出
cat part1.txt part2.txt

# 显示行号
cat -n file.txt
```

> [!warning]
> 大文件不要直接 `cat`，会刷屏并影响终端响应。优先用 `less`、`head`、`tail`。

## 4.2 分页查看

```bash
# 分页查看，推荐
less file.txt

# 简单分页，只能向下翻页
more file.txt
```

`less` 常用快捷键：

| 按键 | 作用 |
|---|---|
| `Space` / `f` | 下一页 |
| `b` | 上一页 |
| `j` / `k` | 下一行 / 上一行 |
| `g` / `G` | 文件开头 / 文件末尾 |
| `/<pattern>` | 向下搜索 |
| `?<pattern>` | 向上搜索 |
| `n` / `N` | 下一个 / 上一个匹配 |
| `q` | 退出 |

## 4.3 只看开头或末尾

```bash
# 前 10 行
head file.txt

# 前 20 行
head -n 20 file.txt

# 后 10 行
tail file.txt

# 后 20 行
tail -n 20 file.txt

# 实时追踪日志
tail -f app.log

# 日志轮转后继续追踪新文件
tail -F app.log
```

> [!tip]
> 生产日志建议用 `tail -F`，它能在 logrotate 后继续跟踪新日志文件。

# 5. 复制、移动与重命名

## 5.1 `cp`

```bash
# 复制文件
cp source.txt dest.txt

# 复制文件到目录
cp source.txt /tmp/

# 递归复制目录
cp -r source_dir dest_dir

# 保留权限、时间戳等属性
cp -p source.txt dest.txt

# 归档复制，尽量保留所有属性
cp -a source_dir dest_dir

# 覆盖前确认
cp -i source.txt dest.txt
```

## 5.2 `mv`

`mv` 既可移动文件，也可重命名文件。

```bash
# 重命名
mv old_name.txt new_name.txt

# 移动文件
mv file.txt /tmp/

# 移动目录
mv source_dir /opt/

# 多个文件移动到同一目录
mv file1.txt file2.txt /tmp/

# 覆盖前确认
mv -i source dest
```

| 命令 | 操作文件 | 操作目录 |
|---|---|---|
| `cp` | 直接操作 | 需要 `-r` |
| `mv` | 直接操作 | 直接操作 |
| `rm` | 直接操作 | 需要 `-r` |

> [!warning]
> `mv old existing_file` 会覆盖目标文件。重要文件移动或重命名时建议使用 `mv -i`。

# 6. 删除文件与目录

## 6.1 `rm`

```bash
# 删除文件
rm file.txt

# 删除多个文件
rm file1.txt file2.txt

# 删除前确认
rm -i file.txt

# 递归删除目录
rm -r dir

# 强制递归删除目录
rm -rf dir
```

| 选项 | 含义 |
|---|---|
| `-r` / `--recursive` | 递归删除目录及其内容 |
| `-f` / `--force` | 忽略不存在的文件，不提示确认 |
| `-i` | 删除前逐项确认 |

> [!warning]
> ==`rm -rf` 不可撤销==。执行前用 `pwd`、`ls`、`realpath` 确认路径，尤其不要在 root 权限下对通配符结果盲删。

## 6.2 `rmdir`

```bash
# 删除空目录
rmdir empty_dir

# 删除多级空目录
rmdir -p a/b/c
```

`rmdir` 只能删除空目录，比 `rm -r` 安全，适合清理确认为空的目录。

# 7. 查找文件

## 7.1 `find` 基本语法

```bash
find [路径] [条件] [动作]
```

## 7.2 按名称与类型查找

```bash
# 按文件名精确匹配
find /path -name "app.log"

# 不区分大小写
find /path -iname "app.log"

# 通配符匹配
find /path -name "*.log"

# 普通文件
find /path -type f

# 目录
find /path -type d

# 符号链接
find /path -type l
```

## 7.3 按大小与时间查找

```bash
# 大于 1MB
find /path -type f -size +1M

# 小于 500KB
find /path -type f -size -500k

# 最近 7 天内修改过
find /path -type f -mtime -7

# 30 天以前修改过
find /path -type f -mtime +30

# 最近 7 天内访问过
find /path -type f -atime -7

# 最近 7 天内元数据变化过
find /path -type f -ctime -7
```

| 时间条件 | 含义 |
|---|---|
| `mtime` | 文件内容修改时间 |
| `atime` | 文件内容访问时间 |
| `ctime` | 文件内容或元数据变化时间 |

## 7.4 按权限与所有者查找

```bash
# 指定用户
find /path -user alice

# 指定组
find /path -group dev

# 精确权限
find /path -perm 755

# 其他用户有写权限
find /path -perm -o+w
```

## 7.5 组合条件与执行动作

```bash
# AND：默认就是并且
find /path -name "*.log" -size +100M

# OR
find /path -name "*.txt" -o -name "*.log"

# NOT
find /path -type f ! -name "*.txt"

# 只列出详细信息
find /path -name "*.log" -ls

# 对结果执行命令
find /path -name "*.log" -exec ls -lh {} +

# 删除匹配文件
find /path -name "*.tmp" -delete

# 安全处理包含空格的文件名
find /path -name "*.txt" -print0 | xargs -0 grep "keyword"
```

> [!warning]
> 使用 `-delete` 或 `-exec rm` 前，先去掉删除动作预览结果，确认匹配范围正确后再执行。

# 8. 快速定位命令和文件

## 8.1 `locate`

`locate` 查询预建索引，速度快但结果不是实时的。

```bash
# 查找路径中包含关键词的文件
locate nginx.conf

# 不区分大小写
locate -i nginx.conf

# 限制输出数量
locate -n 10 nginx.conf

# 更新索引
sudo updatedb
```

## 8.2 `which`、`whereis` 与 `type`

```bash
# 在 PATH 中查找可执行文件
which python3

# 显示所有匹配的可执行文件
which -a python3

# 查找二进制、源码和 man 页
whereis python3

# 查看 shell 如何解析命令
type -a python3
```

# 9. 查看空间占用

## 9.1 `du`

`du` 统计文件或目录实际占用的磁盘空间。

```bash
# 目录总大小
du -sh /path/to/dir

# 目录下每个子项大小
du -h /path/to/dir

# 只看一层
du -h --max-depth=1 /path/to/dir

# 按大小排序
du -h /path/to/dir | sort -h

# 找最大 10 项
du -h /path/to/dir | sort -rh | head -10
```

## 9.2 `df`

`df` 查看文件系统整体容量。

```bash
# 磁盘容量
df -h

# 指定路径所在文件系统容量
df -h /home

# inode 使用情况
df -i
```

> [!info]
> `du` 看“某个目录实际占用多少”，`df` 看“整个文件系统还剩多少”。

# 10. 查看文件元信息

## 10.1 `stat`

```bash
stat file.txt
```

| 时间戳 | 全称 | 触发条件 |
|---|---|---|
| `atime` | Access Time | 读取文件内容 |
| `mtime` | Modify Time | 修改文件内容 |
| `ctime` | Change Time | 修改内容或元数据 |
| `Birth` | Birth Time | 文件创建时间，取决于文件系统支持 |

## 10.2 `file`

`file` 根据文件内容判断真实类型，而不是依赖扩展名。

```bash
file image.png
file script.sh
file /bin/ls
```

# 11. 常用速查

| 场景 | 命令 |
|---|---|
| 当前目录 | `pwd` |
| 列出详细信息 | `ls -lah` |
| 创建多级目录 | `mkdir -p /path/to/dir` |
| 分页查看 | `less file` |
| 跟踪日志 | `tail -F app.log` |
| 复制目录并保留属性 | `cp -a src dst` |
| 移动或重命名 | `mv old new` |
| 查找大文件 | `find /path -type f -size +1G` |
| 目录大小 | `du -sh *` |
| 文件系统容量 | `df -h` |
| 查看元信息 | `stat file` |
| 判断文件类型 | `file file` |

# 12. 易错点总结

> [!summary]
> 本篇核心：查看用 `ls/less/tail`，查找用 `find`，空间用 `du/df`，删除前必须确认路径和匹配范围。

- `cp` 复制目录需要 `-r`，`mv` 移动目录不需要。
- `rm -rf` 不可撤销，执行前先确认 `pwd` 和目标路径。
- `tail -F` 比 `tail -f` 更适合日志轮转场景。
- `locate` 依赖索引，刚创建的文件可能查不到。
- `ls -lh` 显示目录项大小，不代表目录内文件总大小；目录占用看 `du -sh`。
