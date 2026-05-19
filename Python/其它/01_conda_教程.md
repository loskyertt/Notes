---
title: Conda 环境配置与使用教程
date: 2024-08-19
tags:
  - conda
  - python
  - 环境管理
  - Linux
  - Windows
aliases:
  - conda教程
  - miniforge配置
---

# 1. 核心概念：conda 是什么

**conda** 不是单纯的 Python 包管理器，而是**可创建隔离用户态运行时的跨语言包管理器**。

可以把 conda 理解为“**用户态的小型 Linux 发行版包管理器**”，与 `apt`、`yum`、`dnf`、`pacman` 在设计思想上非常相似。

## 1.1 conda 与 pip 的根本差异

| 维度 | pip | conda |
|---|---|---|
| 定位 | 语言级包安装器（类似 npm、cargo） | 系统级发行版包管理器（类似 apt） |
| 管理范围 | Python 包 | 跨语言环境（Python + 动态库 + CUDA + …） |
| 依赖求解 | 逐步解析，不保证全局兼容 | **全环境 SAT 求解**，保证整体兼容 |
| ABI 感知 | 不懂系统 ABI / CUDA / 动态库 | 懂 CUDA 版本、编译器 ABI、动态库兼容性 |
| 安装结果 | `pip install 成功 ≠ 运行一定成功` | 环境整体一致，运行可靠性更高 |

> [!summary]
> **pip** = Python 包安装器；
> **conda** = 可创建隔离用户态运行时的跨语言包管理器。
> 这才是根本差异。

## 1.2 conda 环境的本质

conda 环境本质上是一个**独立前缀目录**，类似一个小型 rootfs：

```text
C:\Users\sky\miniforge3\envs\cloud_point
```

其内部结构类似 `/usr` 或 `/opt/custom_env`：

```text
bin/
Lib/
DLLs/
site-packages/
include/
Library/bin/
```

一个 conda 环境中包含的不仅是 Python，还有：

- 动态库（OpenSSL、libc 兼容层等）
- CUDA runtime
- numpy / scipy 等科学计算库
- ffmpeg 等系统级工具

## 1.3 为什么科学计算偏好 conda

许多 AI / 科学计算包**根本不是纯 Python**：

| 包 | 实际底层依赖 |
|---|---|
| numpy | BLAS / LAPACK |
| scipy | Fortran |
| pytorch | CUDA / C++ |
| open3d | 大型 C++ 工程 |
| opencv | ffmpeg / libjpeg |

这些包更接近 Linux 系统软件，而非简单 Python 模块。conda 能统一管理这些跨语言依赖，而 pip 无法做到。

## 1.4 conda 安装慢的原因

conda 安装包时会执行**全环境依赖求解**（SAT 约束满足问题）。

例如当执行 `conda install pytorch` 时，conda 需同时检查：

- Python 版本
- numpy ABI
- CUDA 版本
- MKL / OpenBLAS
- OpenSSL
- 编译器 ABI
- 已安装包兼容性

然后**重新求解整个环境**，找到一组全局兼容的版本。这比 pip 的逐步解析复杂得多，但也更可靠。

> [!warning]
> 当依赖约束冲突时（如 torch 需要 `numpy>=1.26`，某包需要 `numpy<1.25`），conda 求解会非常慢甚至失败。此时需手动调整版本约束。

## 1.5 conda 与 apt 的区别

| 维度 | apt / yum | conda |
|---|---|---|
| 管理对象 | 整个操作系统 | 用户态隔离环境 |
| 权限要求 | 需要 root | 不需要 root |
| 多版本共存 | 困难 | 轻松（`conda create -n py310` / `conda create -n py311`） |

conda 允许同时拥有不同 Python 版本、不同 CUDA 版本、不同 numpy ABI 的环境，互不影响。

---

# 2. 安装与配置

## 2.1 Linux 环境

### 2.1.1 下载安装

**Miniconda3**：

```bash
wget https://repo.anaconda.com/miniconda/Miniconda3-latest-Linux-x86_64.sh
chmod +x Miniconda3-latest-Linux-x86_64.sh
./Miniconda3-latest-Linux-x86_64.sh
```

**Miniforge**（推荐）：

```bash
# x86_64 架构（大多数 PC）
wget https://github.com/conda-forge/miniforge/releases/latest/download/Miniforge3-Linux-x86_64.sh

# ARM64 架构（如 Raspberry Pi）
wget https://github.com/conda-forge/miniforge/releases/latest/download/Miniforge3-Linux-aarch64.sh

chmod +x Miniforge3-Linux-*.sh
./Miniforge3-Linux-*.sh
```

> [!tip]
> 推荐使用 **Miniforge** 而非 Miniconda。Miniforge 默认使用 conda-forge 频道，社区维护更活跃，包更新更快。

### 2.1.2 集成到 Zsh

以下以 miniconda 为例，miniforge 操作方式完全相同。

**步骤 1**：添加环境变量到 `~/.zshrc`：

```bash
export PATH=/opt/miniconda3/bin:$PATH
```

**步骤 2**：禁止 conda 自动激活 base 环境（建议）：

```bash
conda config --set auto_activate_base false
# 或者（新版命令）
conda config --set auto_activate false
```

查看当前 auto_activate 设置：

```bash
conda config --show auto_activate
```

> [!tip]
> 该命令同样适用于 Windows PowerShell。

**步骤 3**：重新加载配置：

```bash
source ~/.zshrc
```

**步骤 4**：初始化 conda：

```bash
/opt/miniconda3/bin/conda init zsh
```

**步骤 5**：再次重新加载配置：

```bash
source ~/.zshrc
```

**步骤 6**：验证安装：

```bash
conda --version
```

> [!tip]
> 也可以不执行 `conda init`，直接将以下内容手动添加到 `~/.zshrc`（注意修改安装路径）：
>
> ```bash
> # >>> conda initialize >>>
> __conda_setup="$('/opt/miniconda3/bin/conda' 'shell.zsh' 'hook' 2> /dev/null)"
> if [ $? -eq 0 ]; then
>     eval "$__conda_setup"
> else
>     if [ -f "/opt/miniconda3/etc/profile.d/conda.sh" ]; then
>         . "/opt/miniconda3/etc/profile.d/conda.sh"
>     else
>         export PATH="/opt/miniconda3/bin:$PATH"
>     fi
> fi
> unset __conda_setup
> # <<< conda initialize <<<
> export CRYPTOGRAPHY_OPENSSL_NO_LEGACY=1
> ```

### 2.1.3 常见问题

**OpenSSL 报错**：

运行 `conda activate base` 时出现 OpenSSL 3.0 legacy provider 错误：

```bash
Error while loading conda entry point: conda-content-trust (OpenSSL 3.0's legacy provider failed to load...)
CondaError: Run 'conda init' before 'conda activate'
```

解决方法：在 `~/.zshrc` 中添加：

```bash
export CRYPTOGRAPHY_OPENSSL_NO_LEGACY=1
```

然后执行 `source ~/.zshrc`。

## 2.2 Windows 环境

### 2.2.1 下载安装

- [下载 Miniforge](https://github.com/conda-forge/miniforge)（推荐）
- [下载 Miniconda](https://www.anaconda.com/docs/getting-started/miniconda/install#anaconda-website)

推荐安装 [最新版 PowerShell](https://github.com/PowerShell/PowerShell)（PowerShell 7+），其指令功能更强大。

### 2.2.2 集成到 PowerShell

**步骤 1**：在 PowerShell 中执行初始化（替换为你的安装路径）：

```powershell
& 'C:\Users\sky\miniforge3\Scripts\conda.exe' init powershell
```

> `conda init powershell` 会将 conda 的初始化脚本写入 PowerShell 配置文件（`$PROFILE`），添加 `conda` 函数、激活/停用钩子及路径处理。

**步骤 2**：关闭并重新打开 PowerShell。

**步骤 3**：验证 conda 可用：

```powershell
conda --version
where.exe python
```

> [!warning]
> 如果第 1 步报"脚本被禁用"或权限错误，先执行：
>
> ```powershell
> Set-ExecutionPolicy -Scope CurrentUser -ExecutionPolicy RemoteSigned -Force
> ```
>
> 然后重试 `conda init` 与重启操作。

### 2.2.3 常见问题

**cmake 误找 conda 的 FLANN 库**：

conda 环境激活后会将自己的库路径加入环境变量，导致 cmake 优先找到 conda 的 FLANN（带 lz4 依赖），而非 PCL 自带的 FLANN。

解决方法：构建前临时退出 conda 环境：

```powershell
conda deactivate
cmake -B build -G Ninja -DCMAKE_BUILD_TYPE=Debug -DCMAKE_C_COMPILER=cl -DCMAKE_CXX_COMPILER=cl
```

---

# 3. conda 常用指令

## 3.1 配置查看

```bash
conda config --show channels     # 查看镜像源
conda config --show-sources      # 查看配置文件内容
```

## 3.2 代理设置

```bash
# 添加代理
conda config --set proxy_servers.http http://127.0.0.1:10809
conda config --set proxy_servers.https http://127.0.0.1:10809

# 移除代理
conda config --remove-key proxy_servers
```

> [!info]
> 如果启用了代理软件的 TUN 模式，无需单独为 conda 配置代理。

## 3.3 环境管理

| 操作 | 命令 |
|---|---|
| 创建环境（指定名称） | `conda create -n <name> python=<版本>` |
| 创建环境（指定路径） | `conda create --prefix /path/to/env python=3.11` |
| 激活环境 | `conda activate <name>` |
| 退出环境 | `conda deactivate` |
| 查看所有环境 | `conda info --envs` 或 `conda env list` |
| 删除环境（全部） | `conda remove -n <name> --all` |
| 删除环境（指定） | `conda env remove -n <name>` |
| 克隆/重命名环境 | `conda create -n <new_name> --clone <old_name>` |

> [!warning]
> `--prefix/-p` 与 `--name/-n` 不能同时使用。推荐使用 `-n` 命名方式，路径方式管理不便。

> [!tip]
> 删除环境后，可手动清理残留目录：
> - Windows：`C:\Users\<用户名>\miniforge3\envs\`
> - Linux：`~/miniforge3/envs/`

## 3.4 包安装

```bash
# 查询 conda-forge 中的包
conda search -c conda-forge <package_name>

# 从 conda-forge 安装
conda install -c conda-forge <package_name>

# 安装指定版本
conda install -c conda-forge <package_name>=<版本号>
```

> [!info]
> Miniforge 默认从 conda-forge 下载包，无需手动指定 `-c conda-forge`。

示例——安装 GDAL：

```bash
conda install geos proj              # 先装依赖
conda install -c conda-forge gdal=3.2.1  # 再装指定版本
```

## 3.5 环境迁移

**打包环境**：

```bash
conda pack -n <env_name> -o environment.tar.gz
```

如果提示 `No command 'conda pack'`，先安装：

```bash
conda install -c conda-forge conda-pack
```

**在新机器上还原**：

```bash
# 进入 conda 的 envs 目录
cd ~/miniforge3/envs/    # Linux
# 或 cd C:\Users\<用户名>\miniforge3\envs\    # Windows

mkdir <env_name>
tar -xzvf environment.tar.gz -C <env_name>
```

> [!tip]
> Linux 下可通过 `where conda` 查看 conda 安装路径。

## 3.6 环境导出与导入

```bash
# 导出环境配置
conda env export -n <env_name> > environment.yml

# 从配置文件创建环境
conda env create -f environment.yml
```

> [!tip]
> `environment.yml` 比 `conda pack` 更轻量，只记录依赖信息而非二进制文件，适合跨平台迁移和版本控制。

## 3.7 缓存清理

**一键清理**：

```bash
conda clean --all
```

该命令清理：未使用的包、索引缓存、tarball 缓存、临时文件。

**分项清理**：

```bash
conda clean --packages         # 删除未使用的包
conda clean --index-cache      # 清除索引缓存
conda clean --tarballs         # 清除下载的 tarball 文件
conda clean --force-pkgs-dirs  # 强制清理包目录
```

**预览清理内容**（不实际删除）：

```bash
conda clean --all --dry-run
```

输出示例：

```text
Will remove 118 (973.5 MB) tarball(s).
Will remove 1 index cache(s).
Will remove 35 (2.14 GB) package(s).
```

| 项目 | 说明 |
|---|---|
| Tarball | conda 下载的安装包压缩文件，安装后不再需要 |
| Packages | 已解压但未被任何环境使用的包（如删除环境后的残留） |
| DryRun | 模拟清理，不实际删除 |

> [!warning]
> - `conda clean` 仅清理 `pkgs/` 缓存目录，不影响正在使用的环境，是安全的。
> - **不要手动删除** `pkgs` 文件夹内容，可能破坏硬链接导致环境损坏。

**各系统默认缓存路径**：

| 操作系统 | 缓存路径 |
|---|---|
| Linux / macOS | `~/miniconda3/pkgs/` 或 `~/anaconda3/pkgs/` |
| Windows | `C:\Users\<用户名>\miniconda3\pkgs\` |

---

# 4. pip 常用指令

## 4.1 使用临时镜像源

```bash
pip install <package_name> -i <镜像源URL>
```

## 4.2 缓存管理

| 操作 | 命令 |
|---|---|
| 查看缓存位置 | `pip cache dir` |
| 查看缓存信息 | `pip cache info` |
| 列出缓存中的包 | `pip cache list` |
| 清除所有缓存 | `pip cache purge` |
| 删除特定包缓存 | `pip cache remove <package_name>` |
| 安装时跳过缓存 | `pip install --no-cache-dir <package_name>` |

**各系统默认缓存路径**：

| 操作系统 | 缓存路径 |
|---|---|
| Linux | `~/.cache/pip` |
| macOS | `~/Library/Caches/pip` |
| Windows | `%LocalAppData%\pip\Cache` |

---

# 5. 镜像源配置

## 5.1 pip 常用镜像源

| 名称 | URL |
|---|---|
| 清华 | `https://pypi.tuna.tsinghua.edu.cn/simple` |
| 中科大 | `https://pypi.mirrors.ustc.edu.cn/simple` |
| 阿里云 | `http://mirrors.aliyun.com/pypi/simple/` |
| 豆瓣 | `http://pypi.douban.com/simple/` |

## 5.2 conda 镜像源配置

推荐直接编辑 `~/.condarc`（Linux/macOS）或 `C:\Users\<用户名>\.condarc`（Windows），写入以下内容：

```yaml
channels:
  - defaults
show_channel_urls: true
default_channels:
  - https://mirrors.tuna.tsinghua.edu.cn/anaconda/pkgs/main
  - https://mirrors.tuna.tsinghua.edu.cn/anaconda/pkgs/r
  - https://mirrors.tuna.tsinghua.edu.cn/anaconda/pkgs/msys2
custom_channels:
  conda-forge: https://mirrors.tuna.tsinghua.edu.cn/anaconda/cloud
  pytorch: https://mirrors.tuna.tsinghua.edu.cn/anaconda/cloud
```

> [!warning]
> - `pkgs/free` 频道已于 2020 年停止维护，不要添加该频道。
> - Windows 用户若无法直接创建 `.condarc` 文件，可先执行 `conda config --set show_channel_urls yes` 生成该文件后再修改。

配置完成后，清除索引缓存使配置生效：

```bash
conda clean -i
```

> [!info]
> Miniforge 默认使用 conda-forge 频道，上述 `custom_channels` 配置会将 conda-forge 请求自动导向清华镜像，无需额外操作。

---

# 6. 工程实践建议

## 6.1 conda 与 pip 的分工原则

| 工具 | 负责管理 | 典型包 |
|---|---|---|
| **conda** | 基础设施（底层依赖） | python、pytorch、cuda、numpy、scipy、opencv |
| **pip** | 纯 Python 上层库 | hydra、wandb、timm、black、rich |

> [!tip]
> 记忆技巧：**conda 管地基，pip 管装修**。需要编译/CUDA/动态库的用 conda，纯 Python 的用 pip。

## 6.2 conda 最擅长的场景

- ABI 复杂的项目（如点云处理、深度学习）
- CUDA 版本敏感的项目
- Windows 下 DLL 依赖容易冲突的项目
- 需要多版本 Python / CUDA 共存的开发环境

---

# 7. 常见问题排查

配置完成后 conda 仍无法正常使用时，按以下步骤逐项排查。

## 7.1 确认 conda 是否在 PATH 中

```bash
# Linux / macOS
which conda

# Windows PowerShell
where.exe conda
```

若无输出，说明 conda 未加入 PATH。检查：

- `conda init` 是否执行过并重启了终端
- `~/.zshrc`（Linux）或 `$PROFILE`（Windows PowerShell）中是否包含 conda initialize 代码块

## 7.2 检查 .condarc 配置是否生效

```bash
conda config --show-sources
```

该命令会列出所有配置来源及其路径。确认：

- 镜像源 URL 是否正确加载
- 是否存在多个 `.condarc` 文件互相覆盖（常见于系统级与用户级配置并存）

## 7.3 查看详细安装日志

安装失败时使用 `-v` 参数获取详细输出：

```bash
conda install <package> -v        # 详细日志
conda install <package> -vv       # 更详细
conda install <package> -vvv      # 最详细（含 HTTP 请求）
```

重点关注：

- **HTTP 404 / ConnectionError**：镜像源 URL 有误或该频道不存在
- **ResolvePackageNotFound**：指定版本在当前频道不存在，尝试 `conda search <package>` 确认可用版本
- **CollectPackageMetadataError**：索引缓存损坏，执行 `conda clean -i` 后重试

## 7.4 检查网络与代理

```bash
# 测试镜像源连通性
curl -I https://mirrors.tuna.tsinghua.edu.cn/anaconda/pkgs/main/

# 查看当前代理配置
conda config --show proxy_servers
```

常见情况：

| 现象 | 原因 | 解决 |
|---|---|---|
| 连接超时 | 镜像源不可达或被墙 | 切换镜像源或配置代理 |
| SSL 证书错误 | 代理软件拦截 HTTPS | 配置 `proxy_servers` 或关闭代理的 HTTPS 拦截 |
| 下载极慢 | 未配置镜像源 | 按 5.2 节配置国内镜像 |
| 配了代理仍无法连接 | TUN 模式与手动代理冲突 | 移除 `conda config --remove-key proxy_servers`，让 TUN 模式接管 |

## 7.5 检查环境状态

```bash
conda info                    # 查看 conda 基本信息、安装路径、活跃环境
conda list -n <env_name>      # 查看指定环境已安装的包
conda doctor                  # 检查环境完整性（conda >= 4.11）
```

> [!tip]
> `conda doctor` 会检测环境中缺失的依赖、损坏的包和硬链接问题，是排查环境异常的首选命令。

## 7.6 重置配置

当配置严重混乱时，可重置为默认状态：

```bash
# 备份当前配置
cp ~/.condarc ~/.condarc.bak

# 重置所有 conda 配置
conda config --remove-key channels
conda config --remove-key default_channels
conda config --remove-key custom_channels
conda config --remove-key proxy_servers
```

> [!warning]
> 重置后需重新配置镜像源（见 5.2 节），否则 conda 将使用默认的 repo.anaconda.com 源，国内访问可能极慢。

> [!summary]
> 本教程核心要点：
> 1. **conda 是跨语言环境管理器**，不是 Python 专用工具
> 2. **conda 做全环境求解**，pip 只做逐步解析——这是可靠性差异的根源
> 3. **conda 管底层依赖，pip 管纯 Python 包**——这是最佳工程实践
> 4. **Miniforge 优于 Miniconda**——默认 conda-forge 频道，社区更活跃
> 5. **禁止手动删除 `pkgs/` 目录**——必须使用 `conda clean`
