> 因为 glad 提供的是源码，所以采用 CMake 跨平台方案，直接将 glad 添加到项目目录中并引用即可。

# 1. GLAD 配置步骤

我用的是 Intel + NVIDIA 5050 显卡。NVIDIA RTX 5050 是完整支持 **OpenGL 4.6** 的，基于 Blackwell 架构，拥有 2560 个 CUDA 核心，可以使用现代 OpenGL 的所有核心功能。

---

## 1.1 在线生成 GLAD 文件

访问：[官方生成器](https://glad.dav1d.de/)。

| 配置项 | 推荐值 | 说明 |
|--------|--------|------|
| **Language** | C/C++ | 根据你的项目选择 |
| **API** | `gl` | 桌面 OpenGL（不要选 gles），建议选择最新版本 |
| **Specification** | `OpenGL` | 选用 OpenGL 规范标准 |
| **Profile** | **Core** | 现代 OpenGL，推荐 |
| **Extensions** | 见下方推荐 | 按需选择 |

1. **必选项（核心功能）**：

	- `GL_ARB_debug_output` - 调试输出，开发必备
	- `GL_ARB_direct_state_access` (DSA) - 现代 OpenGL 编程方式，强烈推荐
	- `GL_ARB_buffer_storage` - 持久映射缓冲区，性能优化

2. **图形渲染相关**：

	- `GL_ARB_texture_filter_anisotropic` - 各向异性过滤
	- `GL_ARB_texture_compression_bptc` / `GL_ARB_texture_compression_rgtc` - 纹理压缩
	- `GL_ARB_framebuffer_object` - FBO 支持（通常已包含在核心）

3. **计算与高级特性（RTX 5050 支持）**：

	- `GL_ARB_compute_shader` - 计算着色器
	- `GL_ARB_shader_storage_buffer_object` (SSBO) - 着色器存储缓冲
	- `GL_ARB_shader_image_load_store` - 图像加载/存储

4. **NVIDIA 特定优化（可选）**：

	- `GL_NV_mesh_shader` - 网格着色器（RTX 系列支持）
	- `GL_NV_ray_tracing` / `GL_NV_compute_shader_derivatives` - 光线追踪相关

5. **实用工具类**：

	- `GL_ARB_sync` - GPU-CPU 同步
	- `GL_ARB_timer_query` - GPU 计时查询
	- `GL_KHR_debug` - 调试支持

> GLAD 会自动包含该版本核心规范中的所有功能，Extensions 是额外的高级功能。如果你不确定，可以 **版本选 4.6 Core**，Extensions 留空（只勾选 `GL_ARB_debug_output` 用于调试），然后点击 **Generate** 下载。

## 1.2 集成到项目

把 `glad` 目录放到 `<your_project>/3rdparty` 目录下，然后在 `glad` 目录下创建文件 `CMakeLists.txt`，目录结构如下：

```bash
.
├── CMakeLists.txt
├── include
│   ├── KHR
│   │   └── khrplatform.h
│   └── glad
│       └── glad.h
└── src
    └── glad.c
```

`CMakeLists.txt` 配置：

```cmake
# 3rdparty/glad/CMakeLists.txt

# 1. 定义一个小项目
cmake_minimum_required(VERSION 3.20)
project(GLAD VERSION 1.0 LANGUAGES C)

# 2. 创建静态库
# CMAKE_CURRENT_SOURCE_DIR 代表当前 CMakeLists.txt 所在的目录 (即 glad 根目录)
add_library(GLAD STATIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src/glad.c
)

# 3. 设置包含目录
# PUBLIC 意味着链接 GLAD 的项目会自动获得这个 include 路径
target_include_directories(GLAD
  PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# 4. 设置别名
add_library(GLAD::GLAD ALIAS GLAD)

# 5. (可选) 如果需要特定的 OpenGL 版本，在这里定义
# 例如：target_compile_definitions(GLAD PUBLIC GLAD_GL_VERSION_3_3=1)
```

在项目的 `CMakeLists.txt` 中添加：

```cmake
add_subdirectory(./3rdparty/glad)
```

目标程序链接 glad 库：

```cmake
target_link_libraries(your_program
PRIVATE
  opengl32 # opengl32.dll 是 Windows 上的 OpenGL 实现
  GLFW::GLFW
  GLAD::GLAD
)
```
