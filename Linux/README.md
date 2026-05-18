# 1.安装 Linux

> 我建议新手从 EndeavourOS 这个发行版开始使用，这是基于 Arch 的发行版，实际体验下来其实和 Arch 没什么区别，但是支持 GUI 安装，且同时支持离线安装和在线安装（如果网络环境不好可以选择离线安装）。Arch 的中文 wiki 是非常完善的，而且仓库中的软件包是非常之多（尤其是 AUR），搭配 Flatpak 和 一些打包成 `appimage` 的软件，足够满足日常所需了。

进入 [Endeavouros 官网](https://endeavouros.com/)，往下翻找到 Download mirror list，选择中国的镜像源下载。

[EndeavourOS 安装教程](Arch系/EndeavourOS_安装教程.md)

> 当对 Linux 有一定了解后，可以尝试使用 NixOS。对于有“系统洁癖”的用户来说，NixOS 是非常适合的，同时 NixOS 的可复现性也确实让人着迷，比如我在这台机器上写好了配置文件（`*.nix` 文件），可以把这些配置文件复制到另一台机器上，构建（`switch`）后可以还原出一样的环境。缺点就是所有配置（包括 systemd 服务）全都得在配置文件中修改，有一定的学习成本，并且你在其他 Linux 发行版上学到的知识基本上在 NixOS 不适用（NixOS 不遵循传统的 FHS）。如果有些软件包在 NixOS 上没有，你甚至得自己打包，但问题是

点击 [NixOS Download](https://nixos.org/download/) 下载镜像，如果是初次使用，建议下载 Graphical ISO image，不建议通过系统的 Installer 进行安装，推荐打开终端，通过终端进行安装。进入 Live CD 连接好网络后，可以通过系统自带的 Firefox 打开 [NixOS 安装教程](NixOS/01_安装_NixOS.md)，跟着安装教程操作即可。

> 国内用户可从镜像源下载：[ NixOS Images 镜像使用帮助](https://help.mirror.nju.edu.cn/nixos-images/)。

