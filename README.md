# KeyValueStore

## 文件介绍
1. NtyCo: 协程的实现，kvstore是基于协程实现；
2. kvs-client: 支持多语言客户端封装；
3. kvstore.c: 实现了网络基础库与kv应用协议解析，默认使用Array
4. rbtree.c: 实现了RBTree 并封装了符合kvstore的api
5. hash.c: 实现了Hash 并封装了符合Hash的api
6. dhash.c: 实现了DHash 并封装了符合DHash的api
7. skiptable.c: 实现了SkipTable 并封装了符合SkipTable的api
8. log.c: 日志文件
9. testcase.c: 测试用例

### 代码编译
```
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
1.头文件在\KVStore\NtyCo\core\nty_coroutine.h文件中，可根据需要进行开启（1）或关闭（0）功能：
```
  #define ENABLE_ARRAY 1        #ARRAR开关
  #define ENABLE_RBTREE 1       #RBTree开关   
  #define ENABLE_HASH 1         #Hash开关
  #define ENABLE_DHASH 1        #DHash开关
  #define ENABLE_SKIPTABLE 1    #SkipTable开关
  #define ENABLE_LOG 1          #日志开关
```
