# 9.1.1-kvstore

#### 文件介绍
1. 1-1-rbtree-btree: rbtree与btree的服务课程1.1的代码实现，不影响kvstore的实现
2. 2-1-1-multi-io：服务器课程2.1的代码实现，含网络基础库select/poll/epoll的封装。kvstore使用的协程作为网络基础库，不影响kvstore的实现。
3. mem_pool：服务器课程3.1.2的代码实现的内存池。不影响kvstore的实现
4. NtyCo: 协程的实现，kvstore是基于协程实现
5. kvstore.c: 实现了网络基础库与kv应用协议解析
6. rbtree.c: 实现了rbtree 并封装了符合kvstore的api
7. hash.c: 实现了hash 并封装了符合hash的api
8. testcase.c: 测试用例


#### 代码编译（20230807以前的老版本）
```
$ cd 9.1.1-kvstore/NtyCo
$ make  # 先编译NtyCo,生成libntyco.a

$ cd ../ # 进入kvstore的根目录
再编译kvstore.c 
$ gcc -o kvstore kvstore.c -I ./NtyCo/core/ -L ./NtyCo/ -lntyco -lpthread -ldl 
```

#### 代码编译（20230807以后的版本）
```
$ cd 9.1.1-kvstore/
$ make

```

#### 执行
```
# kvstore服务启动
$ ./kvstore # 监听端口8000

打开另一个ssh终端
# 客户端连接
$ telnet xx.xx.xx.xx 8000 #xx.xx.xx.xx为kvstore服务的ip地址

```
### testcast
$ gcc -o testcast testcast.c
$ ./testcast 192.168.218.128 8000