## 简介
Linux高级环境编程作业，百列属性百万行表（全int64_t），支持追加一条、范围查询、建立索引，持久化存储
<br>
## 使用方法
mkdir build && cd build && cmake .. && make
<br>
./test
<br>
执行测试文件在src/main.cpp里
<br>
以接口的形式提供给测试文件调用，接口在include/db.h里
