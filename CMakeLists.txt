#设置cmake的最低版本和项目名称
cmake_minimum_required(VERSION 3.0)
project(mprpc)

#生成debug版本进行gdb调试
set(CMAKE_BUILD_TYPE "Debug")

#输出路径
#设置项目可执行文件输出的路径
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
#设置项目库文件输出的路径
set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)

#输入路径
#设置项目编译头文件搜索路径 -I

#本地调用头文件
include_directories(${PROJECT_SOURCE_DIR}/src/include)

#protoc 生成的message和service的头文件
include_directories(${PROJECT_SOURCE_DIR}/example)

#设置项目库文件搜索路径 -L
link_directories(${PROJECT_SOURCE_DIR}/lib)

add_subdirectory(src)
add_subdirectory(example)