# CMakeList.txt: 顶层 CMake 项目文件，在此处执行全局配置
# 并包含子项目。
#
cmake_minimum_required (VERSION 3.8)

project ("my_coroutine")

# c++20
set(CMAKE_CXX_STANDARD 20)

# 子项目开关, ucontext只能于linux上运行，需要调试哪个项目，就把哪个项目放出来
#add_subdirectory("my_coroutine/stackful_co/ucontext")
add_subdirectory("my_coroutine/stackless_co/cpp20")
