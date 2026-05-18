# 接口库 - 定义接口，但实现由使用者提供
add_library(renderer INTERFACE)

# 告诉使用者需要链接的系统库
target_link_libraries(renderer
  INTERFACE 
    opengl32
)

# 告诉使用者需要的头文件路径
target_include_directories(renderer
  INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# 告诉使用者需要的编译定义
target_compile_definitions(renderer
  INTERFACE
    USE_OPENGL
)
