# JSON库 - 有公共API和内部实现
add_library(json_parser STATIC
    src/json_parser.cpp
    src/internal_utils.cpp
)

# 公共头文件 - 使用者需要
target_include_directories(json_parser
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# 内部头文件 - 只有自己编译时需要
target_include_directories(json_parser
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/internal
)

# 内部依赖库 - 使用者不需要知道
target_link_libraries(json_parser
    PRIVATE
        some_internal_lib
)
