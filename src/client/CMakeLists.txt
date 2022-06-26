#把指定路径下所有源文件放入定义的SRC_LIST变量中
aux_source_directory(. SRC_LIST)

#指定生成可执行文件
add_executable(client ${SRC_LIST})
#链接库
target_link_libraries(client pthread) 