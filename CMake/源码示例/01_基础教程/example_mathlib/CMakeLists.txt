# 纯头文件库 - 没有 .cpp 文件
add_library(math_lib INTERFACE)

# 只需要告诉使用者头文件在哪里
target_include_directories(math_lib
  INTERFACE 
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# 不需要编译任何源文件，因为没有实现文件
