# Key-Value Store
一个使用C + TCP + NtyCo实现的，支持多种存储引擎的Key-Value存储系统，支持多种语言客户端。
解决长、短链接之间的映射关系的问题，在数据库中构建的强查找的数据结构，通过短链接查表重定向获取实链接，可作为一个中间件使用。

## 文件介绍
1. NtyCo: 协程的实现，kvstore是基于协程实现；
2. kvs-client: 支持多语言客户端封装；
3. kvstore.h: 实现所需库的调用，以及存储引擎、日志文件、数据大小定义；
4. kvstore.c: 实现了网络基础库与kv应用协议解析，默认使用Array；
5. rbtree.c: 实现了RBTree 并封装了符合kvstore的api；
6. hash.c: 实现了Hash 并封装了符合Hash的api；
7. dhash.c: 实现了DHash 并封装了符合DHash的api；
8. skiptable.c: 实现了SkipTable 并封装了符合SkipTable的api；
9. spilt.c: 协议解析中分裂函数的实现；
10. log.c: 日志文件；
11. testcase.c: 测试用例，包含通过性测试、压力测试10W、50W、100W；

### 代码编译
```
#所有代码编译，包含testcase
$ make
```

### 执行
```
# kvstore服务启动
$ ./kvstore # 监听端口8000

打开另一个ssh终端
# 客户端连接
$ telnet xx.xx.xx.xx 8000 #xx.xx.xx.xx为kvstore服务的ip地址

```
### 执行举例
```
# kvstore服务启动
$ ./kvstore # 监听端口8000

打开另一个ssh终端
# 客户端连接
#测试用例执行
$ ./testcase 192.168.218.128 8000
```
#### 详细描述
1.头文件kvstore.h: 可根据需要进行开启（1）或关闭（0）功能：
```
  #define ENABLE_ARRAY 1        #存储引擎ARRAR开关
  #define ENABLE_RBTREE 1       #存储引擎RBTree开关   
  #define ENABLE_HASH 1         #存储引擎Hash开关
  #define ENABLE_DHASH 1        #存储引擎DHash开关
  #define ENABLE_SKIPTABLE 1    #存储引擎SkipTable开关
  #define ENABLE_LOG 1          #日志开关,testcase.c 性能测试时请置0
```

2.测试文件testcase.c: 可根据需要进行开启（1）或关闭（0）功能：
```
#define ENABLE_PASS 1 // 存储引擎通过性测试
#define ENABLE_TESTCASE_10W 1  // 10W级别插入测试
#define ENABLE_TESTCASE_50W 0  // 50W级别插入测试
#define ENABLE_TESTCASE_100W 0 // 100W级别插入测试
```
