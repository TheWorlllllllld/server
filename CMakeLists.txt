cmake_minimum_required(VERSION 3.0)
project(chat)

# 配置编译选项
set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} -g)

#设置可执行文件最终存放位置
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

#配置头文件搜索路径
include_directories(${PROJECT_SOURCE_DIR}/include)
include_directories(${PROJECT_SOURCE_DIR}/include/server)
include_directories(${PROJECT_SOURCE_DIR}/include/server/mysql)
include_directories(${PROJECT_SOURCE_DIR}/include/server/model)
include_directories(${PROJECT_SOURCE_DIR}/include/server/redis)
include_directories(${PROJECT_SOURCE_DIR}/thirdparty)
#加载子目录
add_subdirectory(src)

#设置需要编译的源文件列表
# set(SRC_LIST main.cpp)


#把指定路径下所有源文件放入变量中
#aux_source_directory(. SRC_LIST)

#用变量SRC_LIST生成可执行文件 
# add_executable(server ${SRC_LIST})

#为server链接库
# target_link_libraries(server muduo_net muduo_base pthread) 