# CMakeList.txt: my_coroutine 的 CMake 项目，在此处包括源代码并定义
# 项目特定的逻辑。
#
cmake_minimum_required (VERSION 3.8)

project("stackless_co_cpp20")


message( STATUS "CMAKE_VERSION ${CMAKE_VERSION}" )
message (STATUS "CMAKE_SOURCE_DIR: ${CMAKE_SOURCE_DIR}")
message (STATUS "CMAKE_BUILD_TYPE: ${CMAKE_BUILD_TYPE}")
message (STATUS "CMAKE_CXX_COMPILER: ${CMAKE_CXX_COMPILER}")
message (STATUS "PROJECT_NAME: ${PROJECT_NAME}")
message (STATUS "CMAKE_CURRENT_SOURCE_DIR: ${CMAKE_CURRENT_SOURCE_DIR}")

# c++20

set(CMAKE_CXX_STANDARD 20)

# 将源代码添加到此项目的可执行文件。
include_directories(
	${CMAKE_CURRENT_SOURCE_DIR}/
	${CMAKE_CURRENT_SOURCE_DIR}/abstract_co/
	${CMAKE_CURRENT_SOURCE_DIR}/Dispatchers/
	${CMAKE_CURRENT_SOURCE_DIR}/Awaiters/
	${CMAKE_CURRENT_SOURCE_DIR}/helpful_components/
)

add_executable(${PROJECT_NAME} 
  "main.cpp"
  #"Utils.cpp"
  "test.h"
    "Awaiters/SleepAwaiter.h")



