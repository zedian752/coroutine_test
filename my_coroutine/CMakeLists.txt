# CMakeList.txt: my_coroutine 的 CMake 项目，在此处包括源代码并定义
# 项目特定的逻辑。
#
cmake_minimum_required (VERSION 3.8)

project("stackful_co")


message( STATUS "CMAKE_VERSION ${CMAKE_VERSION}" )
message (STATUS "CMAKE_SOURCE_DIR: ${CMAKE_SOURCE_DIR}")
message (STATUS "CMAKE_BUILD_TYPE: ${CMAKE_BUILD_TYPE}")
message (STATUS "CMAKE_CXX_COMPILER: ${CMAKE_CXX_COMPILER}")
message (STATUS "PROJECT_NAME: ${PROJECT_NAME}")


# 开启编译asm配置
enable_language(ASM C)

# 将源代码添加到此项目的可执行文件。
include_directories(
	${CMAKE_CURRENT_SOURCE_DIR}/stackful_co/ucontext
)

add_executable(${PROJECT_NAME} 
${CMAKE_CURRENT_SOURCE_DIR}/stackful_co/ucontext/main.c
${CMAKE_CURRENT_SOURCE_DIR}/stackful_co/ucontext/coroutine.c
${CMAKE_CURRENT_SOURCE_DIR}/stackful_co/ucontext/coroutine.h

)



# TODO: 如有需要，请添加测试并安装目标。
