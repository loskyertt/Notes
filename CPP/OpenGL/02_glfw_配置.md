# 1. Windows 平台

## 1.1 源码下载

[下载 GLFW](https://www.glfw.org/download.html)。

![img](imgs/02_glfw_配置/下载glfw.png)

## 1.2 构建和安装

> 前提配置好工具链：cmake、Ninja、mingw。推荐下载 Clion，然后把对应的工具链添加进环境变量中。

进入 `glfw-3.4` 目录，和项目根目录的 `CMakeLists.txt` 同级，执行：

```bash
cmake -B build -G Ninja -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX="C:\packages\glfw-3.4"
```

> 构建类型设置为 `Release`，安装目录设置为 `C:\packages\glfw-3.4`。

构建好后进行编译：

```bash
cmake --build build -j10
```

安装：

```bash
cmake --install build
```

因为这里用的是 **单配置生成器**（Ninja）在构建时会直接指定构建类型（如 `Release` 或 `Debug`），因此在安装时不需要再指定 `--config Release`。

但是若使用的是 **多配置生成器**（Visual Studio）在构建时不会直接指定构建类型，而是在安装时需要明确指定构建类型。

```bash
# 默认使用 Visual Studio 构建（如果有的话）
cmake -B build -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX="C:\packages\glfw-3.4"

# 编译
cmake --build build --config Release -j10

# 安装
cmake --install build --config Release
```

> 如果要卸载，直接删除 `C:\packages\glfw-3.4` 即可。

---

# 2. Linux 平台

---

# 3. 使用示例

> 这里仅展示了链接 `glfw` 库的配置，其他配置省略。

`CMakeLists.txt` 配置：

```cmake
set(CMAKE_PREFIX_PATH "C:/packages/glfw-3.4")
find_package(glfw3 REQUIRED)

# 检查目标是否存在
if(TARGET glfw)
  message(STATUS "glfw target found!")
  # 获取该目标的包含目录属性
  get_target_property(GLFW_INC glfw INTERFACE_INCLUDE_DIRECTORIES)
  message(STATUS "glfw include path: ${GLFW_INC}")
else()
  message(STATUS "glfw target NOT found!")
endif()

add_executable(example main.cpp)

target_link_libraries(example
  PRIVATE
  opengl32  # Windows 上负责具体的渲染命令
  glfw
)
```

测试代码：

```cpp
#include <GLFW/glfw3.h>

int main(void) {
  GLFWwindow *window;

  /* Initialize the library */
  if (!glfwInit())
    return -1;

  /* Create a windowed mode window and its OpenGL context */
  window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
  if (!window) {
    glfwTerminate();
    return -1;
  }

  /* Make the window's context current */
  glfwMakeContextCurrent(window);

  /* Loop until the user closes the window */
  while (!glfwWindowShouldClose(window)) {
    /* Render here */
    glClear(GL_COLOR_BUFFER_BIT);

    // 生成三角形
    glBegin(GL_TRIANGLES);
    glVertex2f(-0.5f, -0.5f);
    glVertex2f(0.0f, 0.5f);
    glVertex2f(0.5f, -0.5f);
    glEnd();

    /* Swap front and back buffers */
    glfwSwapBuffers(window);

    /* Poll for and process events */
    glfwPollEvents();
  }

  glfwTerminate();
  return 0;
}
```

这会显示一个窗口并生成一个三角形。

# 4. 补充

> 如果下载的是预编译版的 glfw，里面是没有包含 `GLFWConfig.cmake` 文件的，因此需要自己写 `FindGLFW.cmake`。

glfw-3.4.bin.WIN64 目录下的结构：

```shell
.
├── LICENSE.md
├── README.md
├── cmake
│   └── FindGLFW.cmake
├── include
│   └── GLFW
│       ├── glfw3.h
│       └── glfw3native.h
├── lib-mingw-w64
│   ├── glfw3.dll
│   ├── libglfw3.a
│   └── libglfw3dll.a
├── lib-static-ucrt
│   ├── glfw3.dll
│   └── glfw3dll.lib
├── lib-vc2013
│   ├── glfw3.dll
│   ├── glfw3.lib
│   ├── glfw3_mt.lib
│   └── glfw3dll.lib
├── lib-vc2015
│   ├── glfw3.dll
│   ├── glfw3.lib
│   ├── glfw3_mt.lib
│   └── glfw3dll.lib
├── lib-vc2017
│   ├── glfw3.dll
│   ├── glfw3.lib
│   ├── glfw3_mt.lib
│   └── glfw3dll.lib
├── lib-vc2019
│   ├── glfw3.dll
│   ├── glfw3.lib
│   ├── glfw3_mt.lib
│   └── glfw3dll.lib
└── lib-vc2022
    ├── glfw3.dll
    ├── glfw3.lib
    ├── glfw3_mt.lib
    └── glfw3dll.lib
```

在 `glfw-3.4.bin.WIN64/cmake/FindGLFW.cmake` 文件（需要自己创建）中写入：

```cmake
# 1. 设置头文件路径
find_path(GLFW_INCLUDE_DIR
    NAMES GLFW/glfw3.h  # 寻找包含 GLFW 文件夹的那个 glfw3 目录
    PATHS "${CMAKE_CURRENT_LIST_DIR}/../include"
    NO_DEFAULT_PATH # 不包含默认路径
)

# 2. 根据编译器和架构自动选择库目录
# 映射编译器版本
if(MSVC)
  # Visual Studio 版本映射
  if(MSVC_VERSION GREATER_EQUAL 1930)
    set(_COMPILER_DIR "lib-vc2022")      # VS 2022
  elseif(MSVC_VERSION GREATER_EQUAL 1920)
    set(_COMPILER_DIR "lib-vc2019")      # VS 2019
  elseif(MSVC_VERSION GREATER_EQUAL 1910)
    set(_COMPILER_DIR "lib-vc2017")      # VS 2017
  elseif(MSVC_VERSION GREATER_EQUAL 1900)
    set(_COMPILER_DIR "lib-vc2015")      # VS 2015
  elseif(MSVC_VERSION GREATER_EQUAL 1800)
    set(_COMPILER_DIR "lib-vc2013")      # VS 2013
  else()
    set(_COMPILER_DIR "lib-static-ucrt")
  endif()
else()
  set(_COMPILER_DIR "lib-mingw-w64")
endif()

# 3. 查找库
find_library(GLFW_LIBRARY
    NAMES glfw3dll glfw3 # 优先找动态导入库，找不到找静态库
    PATHS "${CMAKE_CURRENT_LIST_DIR}/../${_COMPILER_DIR}"
    NO_DEFAULT_PATH # 不包含默认路径
)

# 4. 定义接口库供外部使用
add_library(GLFW INTERFACE)
target_include_directories(GLFW INTERFACE ${GLFW_INCLUDE_DIR})
target_link_libraries(GLFW INTERFACE ${GLFW_LIBRARY})

add_library(GLFW::GLFW ALIAS GLFW)
```

然后再在你的项目 `CMakeLists.txt` 的配置中添加：

```cmake
# for each "win32/x.cpp", generate target "x"
message("CMakeLists.txt for win32 directory")

list(APPEND CMAKE_MODULE_PATH
  "C:\\Packages\\glfw-3.4.bin.WIN64\\cmake"
)

find_package(GLFW REQUIRED)

file(GLOB_RECURSE all_examples *.cpp)
foreach(v ${all_examples})
  string(REGEX MATCH "win32/.*" relative_path ${v})
  message(${relative_path})
  string(REGEX REPLACE "win32/" "" target_name ${relative_path})
  string(REGEX REPLACE ".cpp" "" target_name ${target_name})

  add_executable(${target_name} ${v})

  target_link_libraries(${target_name}
    PRIVATE
    opengl32 # Windows，负责具体的渲染命令
    GLFW::GLFW
  )

  # MSVC UTF-8 支持
  if(MSVC)
    target_compile_options(${target_name} PRIVATE /utf-8)
  endif()
endforeach()

message("\n--------------------------------------------")
```