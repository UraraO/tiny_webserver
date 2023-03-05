#include <cstdio>
#include <cstdlib>
#include <string.h>
//#include <sys/socket.h>
//#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/epoll.h>
#include <signal.h>
#include "../thread_pool/thread_sync/include/locker.h"
#include "../thread_pool/include/thread_pool.h"
#include "../http/include/http_connection.h"

#define MAX_FD 65535
#define MAX_EVENT_NUM 10000

/*
 * http://124.222.5.77:10000/
 */

// 注册信号捕捉
void add_sig(int sig, void(handler)(int)) {
	struct sigaction sigact;
	memset(&sigact, '\0', sizeof(sigact));
	sigact.sa_handler = handler;
	sigfillset(&sigact.sa_mask);
	sigaction(sig, &sigact, nullptr);
}

int main(int argc, char* argv[]) {

	if(argc <= 1) {
		printf("run as the format: %s port_number\n", basename(argv[0]));
		exit(-1);
	}

	// 获取端口号
	int port = atoi(argv[1]);

	// 对多种信号进行捕捉处理
	// SIGPIPE
	add_sig(SIGPIPE, SIG_IGN);

	// 创建线程池
	thread_pool<http_conn> * pool = nullptr;
	try{
		pool = new thread_pool<http_conn>;
	} catch(...) {
		exit(-1);
	}

	// 保存客户信息
	http_conn * users = new http_conn[MAX_FD];

	int fd_listen = socket(PF_INET, SOCK_STREAM, 0);
	if(fd_listen == -1) {
		perror("fd_listen socket");
		exit(-1);
	}

	// 配置端口复用
	int reuse_port = 1;
	setsockopt(fd_listen, SOL_SOCKET, SO_REUSEADDR, &reuse_port, sizeof(reuse_port));

	// 绑定
	struct sockaddr_in address_server;
	address_server.sin_family = AF_INET;
	address_server.sin_port = htons(port);
	address_server.sin_addr.s_addr = INADDR_ANY;
	int ret_bind = bind(fd_listen, (struct sockaddr *) &address_server, sizeof(address_server));
	if(ret_bind == -1) {
		perror("bind listen");
		exit(-1);
	}
	listen(fd_listen, 5);


	epoll_event events[MAX_EVENT_NUM];
	int epollfd = epoll_create(5);

	// 将监听的文件描述符添加进epoll
	add_fd(epollfd, fd_listen, false);
	http_conn::s_epoll_fd = epollfd;

	while(true) {
		int event_num = epoll_wait(epollfd, events, MAX_EVENT_NUM, -1);
		if((event_num < 0) && (errno != EINTR)) {
			perror("epoll failure");
			break;
		}

		for(int i = 0; i < event_num; ++i) {
			int fd_sock = events[i].data.fd;
			if(fd_sock == fd_listen) {
				// 客户端请求连接
				struct sockaddr_in client_addr;
				socklen_t client_addr_len = sizeof(client_addr);
				int fd_conn = accept(fd_listen, (struct sockaddr *) &client_addr, &client_addr_len);

				if(http_conn::s_user_count >= MAX_FD) {
					// 当前连接数已满
					// 给客户端返回服务器正忙信息
					// **********************
					close(fd_conn);
					continue;
				}
				// 初始化新客户数据，置于数组中
				users[fd_conn].init(fd_conn, client_addr);

			}
			else if(events[i].events & (EPOLLRDHUP | EPOLLHUP | EPOLLERR)) {
				// 客户端异常
				users[fd_sock].close();
			}
			else if(events[i].events & EPOLLIN) {
				if(users[fd_sock].read()) {
					// 一次读取所有数据
					pool->append(users + fd_sock);
				}
				else {
					users[fd_sock].close();
				}
			}
			else if(events[i].events | EPOLLOUT) {
				if(!users[fd_sock].write()) {
					users[fd_sock].close();
				}
			}
		}
	}
	close(epollfd);
	close(fd_listen);

	delete pool;
	delete[] users;
	return 0;
}







