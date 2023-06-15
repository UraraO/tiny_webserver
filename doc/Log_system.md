# Log系统的设计

## 结构
从底层到上层：
- file_util
- log_file
- log_stream
- async_log
- ulog

#### 1.file_util
- file_util封装了最底层的日志文件操作，定义了一个append_file类，维护打开的日志文件描述符和一个缓冲区，通过append方法直接向日志文件写，flush方法刷新缓冲区

- 成员：
fd：日志文件描述符
buffer：缓冲区

- 方法：
append，直接向文件写
flush，刷新写入缓冲区

#### 2.log_file
- log_file封装了append_file类，增加了flush_per_N成员，可以在N次append之后再flush，减少了磁盘IO

- 成员：
append_file：日志文件类
basename：日志文件名
N：N次append之后再flush的次数

- 方法：
append，直接向文件写
flush，刷新写入缓冲区

#### 3.log_stream
- log_stream用于格式化输出，封装了大量不同类型的输出运算符<<，方便进行LOG << something；

- 成员：
buffer：用于保存类型转化的结果

- 方法：
<<：大量的输出运算符，将不同类型的输入转化成字符串保存到buffer中，
用于最终输出到日志文件中

#### 4.async_log
- async_log是日志系统的核心，拥有一个独立的线程，内部包含多个缓冲区，其中部分缓冲区从前端接收日志行输入，另一部分缓冲区向文件写，当前端缓冲区满或刷新时间到，则交换前后端缓冲区，最终实现异步写入日志，将文件IO和前端写日志分离，防止文件IO影响业务响应延迟
- 以下的前端为webserver的业务，也就是http服务器，后端为异步日志线程实际往日志文件中写的操作
- async_log维护1个日志线程，多个缓冲区，以及一些同步工具，多个缓冲区分为两类：1.前端缓冲区，从前端接收日志请求；2.后端缓冲区，将接收到的日志写入文件；两类缓冲区异步工作，互不影响，防止后端文件IO延迟过大影响前端。

- 成员：
buffers：多个缓冲区，一部分从前端接收日志，一部分向文件写
filename：日志文件名，用于初始化一个log_file
thread：一个异步日志线程，线程函数无限循环，接收日志输入，向文件写
mutex，cond：一些同步工具

- 方法：
append，向缓冲区写日志

- 异步日志具体原理：
		async_log类内维护一些缓冲区buffers，一个缓冲区cur，一个缓冲区next
		日志线程内有另外的本地缓冲区列表need_to_write，两个临时缓冲区b1，b2
		async_log类内的缓冲区cur，next，buffers用于从前端接收日志请求，前端写的日志通过append方法写入cur，然后将cur加入buffers，next用于cur容量不够时替换备用
		日志线程中无限循环逻辑如下：buffers中保存前端来的请求，need_to_write不断与buffers交换，每次写完后，与buffers交换然后再去写。cur每次将buffer1取走，next每次将buffer2取走，buffer1和buffer2每次都重置为空缓冲区。
		每次循环开始，buffer1，buffer2，need_to_write都为空。
#### 5.ulog
- ulog是日志接口，保存一个log_stream对象
用法：LOG << something;
LOG是一个ulog对象的stream
- 在这一过程中：LOG涉及一个ulog对象的构造，然后向ulog对象的流<< something，最后ulog对象析构
1.ulog对象构造，此时会将系统时间格式化并输出
2.<< something，将something输出到ulog的流中
3.ulog对象析构，此时将打日志的请求行的行号和文件名输出
最终的日志文件中，一条日志类似这样：
		2023/06/13  13:57:36  New connection from 124.222.5.77:40446 -- /root/remote_repo/tiny_WebServer/src/server.cpp:47
	前面是时间，中间是日志消息，后面是请求日志的文件和代码行

- 成员：
buffer：用于保存类型转化的结果

- 方法：
<<：大量的输出运算符，将不同类型的输入转化成字符串保存到buffer中，用于最终输出到日志文件中




