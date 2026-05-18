# 1.搭建多语言开发环境

先开启 flakes 功能。每一个开发环境的搭建都是通过在对应项目中写 `flake.nix` 实现，然后在终端中执行 `nix develop` 就能构建项目环境，再在终端中执行 `code .` 就能在 VSCode 中打开该项目，并使用该终端中的环境。如下示例：

```shell
 tree . -L 1
.
├── cmake_build_debug.sh
├── CMakeLists.txt
├── example
├── flake.lock
├── flake.nix
├── format_count.sh
├── include
├── lib
├── LICENSE
├── README.md
├── src
└── test
```

这是我当前的 `test_cpp` 目录下的项目结构，重点是要有 `flake.nix`。

然后执行 `nix develop`：

![img](imgs/04_Nix_Dev_Env/01_flake配置示例.png)

在原来的系统环境中是没有 C++ 工具链的。

因为各个开发环境的 `flake.nix` 是能复用的，所以可以集中保存好，需要的时候复制过去即可。

---

# 2.C/C++ 开发环境

在 `flake.nix` 中写入：

```shell
{
  description = "C++ Clang 开发环境";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";
  };

  outputs = { self, nixpkgs }:
    let
      system = "x86_64-linux"; # 如果是 ARM 架构则改为 aarch64-linux
      pkgs = import nixpkgs { inherit system; };
      # 创建一个基于 clang 的 shell
      chanllengedShell = pkgs.mkShell.override { stdenv = pkgs.clangStdenv; };
    in
    {
      devShells.${system}.default = chanllengedShell {
        # 1. 编译和构建工具
        nativeBuildInputs = with pkgs; [
          cmake              # 构建工具
          ninja              # 更快的构建后端
          clang-tools        # 包含 clangd (LSP), clang-format, clang-tidy
          gdb                # 调试器
        ];

        # 2. 运行时依赖库 (第三方库放这里)
        buildInputs = with pkgs; [
          # 示例：添加常用库
          llvmPackages.openmp
          boost
          nlohmann_json
          fmt
        ];

        # 环境变量设置
        shellHook = ''
          echo "C++ 开发环境已加载！"
          echo "编译器: $(clang --version | head -n 1)"
          # 告诉 CMake 如何找到 Nix 安装的库
          export CMAKE_EXPORT_COMPILE_COMMANDS=1
        '';
      };
    };
}
```

然后在 `.vscode/settings,json` 中添加下面的内容：

```json
{
    "clangd.arguments": [
        "--query-driver=/nix/store/**-clang-**/bin/clang++",
        "--compile-commands-dir=build",
        "--background-index",
        "--log=verbose"
    ],
    "clangd.path": "clangd",
}
```

>> 这两个字段必须添加，否走 VSCode 编辑器中的 clangd 无法找到标准库！

---

# 3.Node.js 开发环境

在 `flake.nix` 中写入：

```shell
{
  description = "Node.js 开发环境";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";
  };

  outputs = { self, nixpkgs }:
    let
      # 建议使用此方式支持多架构 (x86_64-linux, aarch64-darwin 等)
      system = "x86_64-linux"; 
      pkgs = import nixpkgs { inherit system; };
    in
    {
      devShells.${system}.default = pkgs.mkShell {
        # 1. 开发工具 (编译时工具)
        nativeBuildInputs = with pkgs; [
          nodejs_20           # Node.js 运行时 (也可以选 nodejs_18, nodejs_22 等)
          yarn
          typescript-language-server # LSP 服务
          nodePackages.typescript    # tsc 编译器
          nodePackages.prettier      # 代码格式化
        ];

        # 2. 运行时依赖或系统库
        buildInputs = with pkgs; [
          # openssl
          # pkg-config
        ];

        # 环境变量设置
        shellHook = ''
          # 自动启用 corepack 以支持 pnpm/yarn
          # corepack enable --node-path ${pkgs.nodejs_20}/bin/node

          echo "Node.js 开发环境已加载！"
          echo "Node版本: $(node --version)"
          echo "NPM版本:  $(npm --version)"
          
          # 防止 node_modules 中的二进制文件找不到
          export PATH="$PWD/node_modules/.bin:$PATH"
        '';
      };
    };
}
```

>> **注意事项**：在 Nix 的逻辑中，我们**永远不要**使用 `npm install -g` 或 `pnpm add -g`。 如果你需要某个全局工具（比如 `pm2` 或 `vercel-cli`），你应该把它们直接写在 `flake.nix` 的 `nativeBuildInputs` 列表里。同样在 Python 环境中，也不要直接 `pip install`，而是先创建虚拟环境，把包安在虚拟环境中。