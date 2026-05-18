# 1. Steam GameScope 屏幕闪烁

> Q：nixos 启用了 steam gameScopeSession 后，可能会出现一个问题：进入 steam scope 模式后（类似于 steam deck），当移动鼠标时，屏幕就是正常的，但是鼠标一旦静止，屏幕就会立即息屏。

在 Gamescope 环境下，如果鼠标静止即黑屏，通常是因为 **Intel 核显的面板自刷新 (Panel Self Refresh, PSR)** 技术与 Gamescope 的渲染机制发生了冲突。

简单来说，当画面没有变化时，Intel 驱动尝试通过 PSR 进入省电状态，但 Gamescope 可能没有正确触发唤醒信号，导致驱动误以为可以关闭显示输出。

## 1.2 解决方案

> 一般需要通过内核参数禁用 Intel 的 PSR 功能。这是解决 Intel 核显在 Wayland/Gamescope 下闪屏或息屏最有效的办法。

1. 在你的 NixOS 配置文件中找到 `boot.kernelParams`，添加 `i915.enable_psr=0`：

```bash
boot.kernelParams = [ 
  "i915.enable_psr=0"  # 禁用 Intel 的面板自刷新
];
```

> 设置 `i915.enable_psr=0` 可以彻底关闭这个节能开关。虽然会稍微增加一点功耗，但可以保证显示信号的绝对稳定。

2. 保存文件后，运行：

```bash
sudo nixos-rebuild switch
```

然后 **重启电脑**，这个内核参数才能生效。

---
