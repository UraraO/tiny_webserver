# 并发结构

## 结构
从底层到上层：
- timer
- http_data
- channel
- uepoll
- event_loop
- event_loop_thread
- event_loop_thread_pool
- server

#### 1.timer
- timer_node
  timer_node是计时器节点，其中保存一个time_point表示超时时间点，当到了这个时间点即计时器超时
  可以判断一个计时器是否超时，是否有效，是否已经弃用

- timer_manager
  timer_manager用一个优先队列管理多个timer_node，最先超时的node一定在堆顶，可以方便地处理超时节点
  manager中直接保存超时处理方法

#### 2.http_data
- http_data
  http_data保存http协议中的多种字段和解析方法，以及对应请求报文的处理方法，读，写，建立连接等
  一个httpdata与一个timer_node对应，因为一个http_data一定有其超时时间
  一个httpdata有一个与其对应的event_loop，一个channel

#### 3.channel
- channel负责管理一个fd，其管理fd关注的IO事件，正在活动的IO事件，以及多种事件的回调方法
  有一个weak_ptr指向自身的拥有者http_data
  当fd上有活动事件，就通过channel处理

- 方法：
    - set_xx_handler，设置其读，写，建立连接的回调方法
      用于http连接的，各种回调方法由http_data实现并设置
      用于event_loop唤醒的，由event_loop实现并设置
    - handle_events，根据epoll监听到的活动事件及其注册好的回调函数，进行处理

#### 4.uepoll
- uepoll包装了epoll，其中维护监听到有响应的epoll_event的数组，一个timer_manager，监听的fd对应的channel和http_data
- epoll监听大量fd所对应的事件，events就保存这些事件，然后两个哈希表存储这些fd对应的channel和http_data

  有一个weak_ptr指向自身的拥有者http_data
  当fd上有活动事件，就通过channel处理

- 方法：
    - epoll_add，向epoll中添加需要监听的fd
    - epoll_mod，修改epoll中fd的信息
    - epoll_del，从epoll中删除一个fd
    - poll，epoll监听有活动的fd，将得到的有活动fd列表包装，返回。在这一过程中，还会将有活动事件的channel的 revents（正在活动的事件）设置好

#### 5.event_loop
- one loop per thread顾名思义，一个线程只运行一个event_loop，创建了event_loop对象的线程是IO线程，专用于执行event_loop.loop()
- event_loop类的构造函数会检测当前线程是否已经有其他的event_loop，如果已有，则输出错误
- 维护一个等待队列，等待队列中存放其他线程需要交给本线程执行的方法，主要是主loop建立连接之后，将连接交付给IO线程进行http数据传输和处理
- 方法：
    - run_in_loop，给一个方法，如果run_in_loop在当前线程内调用，则直接执行该方法，如果是在别的线程调用，则将方法加入待执行队列，然后唤醒loop线程，在loop线程执行，相当于把一个方法让一个loop对应的线程去执行
    - queue_in_loop，给一个方法，让loop对应的IO线程去执行，相当于主动异步调用一个函数（async，future，只不过此处的方法没有返回值）
    - loop，一个event_loop的核心工作，IO线程实际上就在无限循环该函数
        - 调用epoll的poll方法，取出所有监听到活动事件的channel
        - handle所有有活动事件的channel
        - 执行等待队列的所有事件，此处包含两种，一是server的main_loop建立新连接，二是部分连接的关闭转给IO线程处理
    - xxx_poller，转调用对应的uepoll_add，mod，del

#### 6.event_loop_thread
- 包装一个线程，该线程初始化一个event_loop，然后loop

#### 7.event_loop_thread_pool
- 包装数个loop_thread，然后将已建立的连接逐个放入这些线程池的等待队列里

#### 8.server
- 一个server有一个主线程，一个线程池
- 主线程即main_loop，在main函数内建立，由server管理，main_loop仅保存listen_fd，用于接收新连接请求，然后将连接交给线程池内的IO线程处理业务
- 线程池内是多个IO线程，用于接收主线程中已经建立连接的channel，然后用epoll监听是否有IO事件，进行处理



