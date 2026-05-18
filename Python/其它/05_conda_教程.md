---
title: Linux 下配置 conda 环境
date: 2024-08-19 16:21:15
excerpt: "这是一篇Linux（EndeavourOS）下如何把 conda 环境集成到终端中的教程。"
categories: "Linux教程"
---


# 1. 安装

## 1.1 Linux 环境

下载最新版 `miniconda3`：

```bash
# 下载适用于 Linux 的 Miniconda 安装包（根据你系统的架构选择 64-bit 或 32-bit）
wget https://repo.anaconda.com/miniconda/Miniconda3-latest-Linux-x86_64.sh
```

下载完成后，通过以下命令启动安装：

```bash
# 赋予安装脚本执行权限
chmod +x Miniconda3-latest-Linux-x86_64.sh

# 执行安装脚本
./Miniconda3-latest-Linux-x86_64.sh
```

（**推荐**）对于`miniforge`，下载：

```bash
# 如果是 x86_64 架构（大多数 PC）
wget https://github.com/conda-forge/miniforge/releases/latest/download/Miniforge3-Linux-x86_64.sh

# 如果是 ARM64 架构（比如 Raspberry Pi）
wget https://github.com/conda-forge/miniforge/releases/latest/download/Miniforge3-Linux-aarch64.sh
```

下载完成后，运行以下命令安装：

```bash
# 赋予安装脚本执行权限
chmod +x Miniforge3-Linux-*.sh

# 执行安装脚本
./Miniforge3-Linux-*.sh
```

---

### 1.1.1 配置

> 注：这里以 miniconda 为例，如果用的是 miniforge，操作方式与此类似。在这里需要把 `conda` 集成到 `zsh` 终端中

1. **添加 `conda` 环境变量：**

把这行代码加入到 `.zshrc` 中：

```txt
export PATH=/opt/miniconda3/bin:$PATH
```

终端输入这行可以防止 conda 自动激活环境（建议加上）：

```bash
conda config --set auto_activate_base false
```

2. **重新加载 `~/.zshrc` 文件：**

在终端中运行以下命令重新加载配置文件：

```sh
source ~/.zshrc
```

3. **运行 `conda init`：**

初始化 conda 环境：

```bash
# 这是conda默认安装的位置
/opt/miniconda3/bin/conda init zsh
```

4. **再次重新加载 `~/.zshrc` 文件：**

再次运行以下命令重新加载配置文件：

```sh
source ~/.zshrc
```

5. 验证 conda 是否配置好：

```bash
conda --version
```

> 可以不用管以上操作，直接把这段复制到 `.zshrc` 中，注意安装 miniconda 的路径：

```bash
# >>> conda initialize >>>
# !! Contents within this block are managed by 'conda init' !!
__conda_setup="$('/opt/miniconda3/bin/conda' 'shell.zsh' 'hook' 2> /dev/null)"
if [ $? -eq 0 ]; then
    eval "$__conda_setup"
else
    if [ -f "/opt/miniconda3/etc/profile.d/conda.sh" ]; then
        . "/opt/miniconda3/etc/profile.d/conda.sh"
    else
        export PATH="/opt/miniconda3/bin:$PATH"
    fi
fi
unset __conda_setup
# <<< conda initialize <<<
export CRYPTOGRAPHY_OPENSSL_NO_LEGACY=1
```

### 1.1.2 问题汇总

> 1. OpenSSL 问题

当运行 `conda activate base` 时，可能出现下面问题：

```bash
 $ conda activate base
Error while loading conda entry point: conda-content-trust (OpenSSL 3.0's legacy provider failed to load. This is a fatal error by default, but cryptography supports running without legacy algorithms by setting the environment variable CRYPTOGRAPHY_OPENSSL_NO_LEGACY. If you did not expect this error, you have likely made a mistake with your OpenSSL configuration.)

CondaError: Run 'conda init' before 'conda activate'
```

根据该错误信息，问题可能与 OpenSSL 版本有关。OpenSSL 3.0 引入了一些变化，可能导致与某些软件包的兼容性问题。在此情况下，设置环境变量 `CRYPTOGRAPHY_OPENSSL_NO_LEGACY` 可能会解决问题。

- **解决方法：**

编辑 `~/.zshrc` 文件，添加以下行到 `~/.zshrc` 文件：

```bash
export CRYPTOGRAPHY_OPENSSL_NO_LEGACY=1
```

保存并退出编辑器，然后执行 `source ~/.zshrc`。

## 1.2 Windows 环境

### 1.2.1 配置

> [下载 miniforge](https://github.com/conda-forge/miniforge) 或者 [下载 miniconda](https://www.anaconda.com/docs/getting-started/miniconda/install#anaconda-website)。推荐下载 miniforge

推荐下载 [最新版本的 PowerShel](https://github.com/PowerShell/PowerShell)（我写这篇文章时，最新版是 PowerShel7），并将其设置为默认终端。最新版本的 PowerShel 其指令功能更加强大。

1. 在刚安装好的 PowerShel 中执行指令：

```bash
& 'C:\Users\sky\miniforge3\Scripts\conda.exe' init powershell
```

要把 `C:\Users\sky\miniforge3\Scripts\conda.exe` 改为你自己的 miniforge 的安装路径。

> `conda init powershell` 只是把 conda 的初始化脚本写入你的 PowerShell 配置文件（`$PROFILE`），添加一个 `conda` 函数、激活/停用钩子以及必要的路径处理。

输出这样表示初始化成功：

```shell
no change     C:\Users\sky\miniforge3\Scripts\conda.exe
no change     C:\Users\sky\miniforge3\Scripts\conda-env.exe
no change     C:\Users\sky\miniforge3\Scripts\conda-script.py
no change     C:\Users\sky\miniforge3\Scripts\conda-env-script.py
no change     C:\Users\sky\miniforge3\condabin\conda.bat
no change     C:\Users\sky\miniforge3\Library\bin\conda.bat
no change     C:\Users\sky\miniforge3\condabin\_conda_activate.bat
no change     C:\Users\sky\miniforge3\condabin\rename_tmp.bat
no change     C:\Users\sky\miniforge3\condabin\conda_auto_activate.bat
no change     C:\Users\sky\miniforge3\condabin\conda_hook.bat
no change     C:\Users\sky\miniforge3\Scripts\activate.bat
no change     C:\Users\sky\miniforge3\condabin\activate.bat
no change     C:\Users\sky\miniforge3\condabin\deactivate.bat
modified      C:\Users\sky\miniforge3\Scripts\activate
modified      C:\Users\sky\miniforge3\Scripts\deactivate
modified      C:\Users\sky\miniforge3\etc\profile.d\conda.sh
modified      C:\Users\sky\miniforge3\etc\fish\conf.d\conda.fish
no change     C:\Users\sky\miniforge3\shell\condabin\Conda.psm1
modified      C:\Users\sky\miniforge3\shell\condabin\conda-hook.ps1
no change     C:\Users\sky\miniforge3\Lib\site-packages\xontrib\conda.xsh
modified      C:\Users\sky\miniforge3\etc\profile.d\conda.csh
modified      C:\Users\sky\Documents\WindowsPowerShell\profile.ps1
modified      C:\Users\sky\Documents\PowerShell\profile.ps1

==> For changes to take effect, close and re-open your current shell. <==
```

2. 关闭并重新打开 PowerShell。

（推荐：直接关闭 PowerShell 再打开）

3. 打开新的 PowerShell 后验证 `conda` 可用并激活环境：

```shell
conda --version
where.exe python
```

如果在运行 **第 1 步** 时报“脚本被禁用”或权限相关错误，先运行（以当前用户作用域允许脚本）：

```shell
Set-ExecutionPolicy -Scope CurrentUser -ExecutionPolicy RemoteSigned -Force
```

然后重试 `conda init` 与重启操作。

### 1.2.2 问题汇总

> 1. 解决 cmake 的 find_package 查找 conda 的 FLANN（带 lz4 依赖）现象。会和 PCL 的 FLANN 混在一起！

当把 conda 集成到默认终端时（如 PowerShell），conda 环境激活后会把自己的库路径塞进环境变量，cmake 就优先找到了 conda 的 FLANN，而不是 PCL 自带的。

建议构建前临时退出 conda 环境：

```powershell
conda deactivate
cmake -B build -G Ninja -DCMAKE_BUILD_TYPE=Debug -DCMAKE_C_COMPILER=cl -DCMAKE_CXX_COMPILER=cl
```

---

# 2. conda 指令

## 2.1 查看配置

```shell
conda config --show channels    # 查看镜像源
conda config --show-sources        # 查看配置文件内容
```

## 2.2 设置代理端口

> 如果启用了代理软件的 TUN 模式，可以不用单独为 conda 配置代理地址。

```shell
# 添加代理地址端口
conda config --set proxy_servers.http http://127.0.0.1:10809
conda config --set proxy_servers.https http://127.0.0.1:10809

# 移除代理
conda config --remove-key proxy_servers
```

## 2.3 环境管理

1. 创建（带名字）

```bash
conda create -n <conda_name> python=<版本号>

# 在指定文件路径下创建conda环境
conda create --yes --prefix /home/sky/桌面/pointTest/.conda python=3.11
```

> 注意：`--prefix/-p` 不能与 `--name/-n` 同时使用！

2. 激活 conda 环境

```bash
conda activate <conda_name>
```

3. 回到 base 环境

```bash
conda deactivate
```

4. 查看有哪些 conda 环境

```bash
conda info --envs

# 或者
conda env list
```

5. 删除全部环境

```bash
conda remove -n env_name --all
```

6. 删除指定环境

```bash
conda env remove -n env_name
```

> 确认环境已经删除后，可以手动删除 `C:\Users\<用户名>\miniforge3\envs\`（Windows）或者 `~/miniforge3/envs/`（Linux）下残留的已删除环境的目录。

7. 重命名环境（将 --clone 后面的环境重命名成 -n 后面的名字）

```bash
# 比如：将 py3 重命名为 torch
conda create -n torch --clone py3
```

## 2.4 下载库

1. 查询 conda-forge 中的包

```bash
conda search -c conda-forge <package_name>
```

2. 从 conda-forge 渠道中提供的包安装

```bash
conda install -c conda-forge <package_name>
```

3. 安装指定版本的包

```bash
conda install -c conda-forge <package_name>=<版本号>
```

> miniforge 默认是从 conda-forge 中下载包。

比如安装 GDAL 库：

```shell
#  安装 gdal 的依赖库 geos 和 proj
conda install geos proj

# 安装指定版本 GDAL
conda install -c conda-forge gdal=3.2.1
```

## 2.5 迁移 conda 环境

将要迁移的环境打包

```shell
conda pack -n 虚拟环境名称 -o environment.tar.gz
```

如果报错：No command ‘conda pack’

```shell
# 尝试使用
conda install -c conda-forge conda-pack
```

复制压缩文件到新的电脑环境。进到conda的安装目录：`/anaconda`(`或者miniconda`)/`envs`/

```shell
# cd 到 conda 的安装路径
mkdir environment

# 解压conda环境：
tar -xzvf environment.tar.gz -C  environment
```

> 对于 Linux 可以通过 `where conda` 查看 conda的安装路径。

## 2.6 缓存清理

1. 清除未使用的包和缓存

```bash
conda clean --all
```

该命令会清理：未使用的包、索引缓存、tarball 缓存、临时文件。

2. 分项清理（更精细控制）

```bash
conda clean --packages      # 删除未使用的包
conda clean --index-cache   # 清除索引缓存
conda clean --tarballs      # 清除下载的 tarball 文件
conda clean --force-pkgs-dirs  # 强制清理包目录
```

3. 查看可清理内容（不实际删除）

```bash
conda clean --all --dry-run
```

输出示例：

```bash
Will remove 118 (973.5 MB) tarball(s).
Will remove 1 index cache(s).
Will remove 35 (2.14 GB) package(s).
There are no tempfile(s) to remove.
There are no logfile(s) to remove.

DryRunExit: Dry run. Exiting.
```

- **Tarball (973.5 MB)**：这是 conda 下载过的安装包压缩包（.tar.bz2 或 .conda 文件），安装完成后通常不再需要。
- **Packages (2.14 GB)**：这是已经解压但未在任何虚拟环境中使用的包（例如你创建过又删除了环境，残留的包）。
- **DryRun**：这里运行的是 `--dry-run`，意思是 **“模拟清理”**。系统只是计算了能删多少，**实际上还没有删除任何文件**。

> 执行 `conda clean --all` 就能删除未使用的包和 tarball。

`conda clean` 的作用对象仅针对 `pkgs/` 缓存目录，执行该命令后，conda 会检查 `pkgs/` 中的每个包是否被任何现有环境（包括 `base`）引用。只有当没有任何环境使用某个包时，它才会被删除。因此，这种清理是很安全的，并不会破坏 `base` 环境（或其他任何现有环境）中正在使用的包。

> 注意：- **不要手动删除** `pkgs` 文件夹里的内容。手动删除可能会破坏硬链接，导致环境损坏。必须使用 `conda clean` 命令。

4. 各系统默认缓存路径

| 操作系统 | 缓存路径 |
|----------|----------|
| Linux/macOS    | `~/miniconda3/pkgs/` 或 `~/anaconda3/pkgs/` |
| Windows  | `C:\Users\<用户名>\miniconda3\pkgs\` 或 `anaconda3\pkgs\` |

---

# 3. pip 指令

## 3.1 使用临时镜像源下载库

```shell
pip install <package_name> -i <镜像源url>
```

## 3.2 缓存清理

1. 查看缓存位置

```bash
pip cache dir
```

2. 查看缓存信息（大小、包数量等）

```bash
pip cache info
```

3. 列出缓存中的包：

```bash
pip cache list
```

4. 清除所有缓存：

```bash
pip cache purge
```

5. 删除特定包的缓存（支持通配符）：

```bash
pip cache remove <package_name>
```

6. 各系统默认缓存路径

| 操作系统 | 缓存路径 |
|----------|----------|
| Linux    | `~/.cache/pip` |
| macOS    | `~/Library/Caches/pip` |
| Windows  | `%LocalAppData%\pip\Cache` |

7. 安装时跳过缓存

```bash
pip install --no-cache-dir <package_name>
```

---

# 4. 配置镜像源

```shell
https://pypi.tuna.tsinghua.edu.cn/simple    # 清华
https://pypi.mirrors.ustc.edu.cn/simple        # 中科大
http://mirrors.aliyun.com/pypi/simple/        # 阿里云
http://pypi.douban.com/simple/            # 豆瓣
```
