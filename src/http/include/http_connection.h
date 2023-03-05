#ifndef HTTP_CONNECTION_H
#define HTTP_CONNECTION_H

#include <sys/epoll.h>
#include <cstdio>
#include <cstdlib>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <stdarg.h>
#include <string.h>
#include <errno.h>
//#include <uio.h>
#include "../../thread_pool/thread_sync/include/locker.h"

class http_conn {
public:
	http_conn() {}
	~http_conn() {}

	static int s_epoll_fd;
	static int s_user_count;
	static const int read_buf_size = 4096;
	static const int write_buf_size = 1024;

	void process(); // 处理客户端请求

	void init(int fd_sock, const sockaddr_in & cliaddr);
	void close();
	bool read();
	bool write();



	enum METHOD {
		GET = 0,
		POST,
		PUT,
		HEAD,
		DELETE,
		TRACE,
		OPTION,
		CONNECT
	};

	enum CHECK_STATE {
		CHECK_STATE_REQUESTLINE,
		CHECK_STATE_HEADER,
		CHECK_STATE_CONTENT
	};

	enum HTTP_CODE {
		NO_REQUEST,
		GET_REQUEST,
		BAD_REQUEST,
		NO_RESOURCE,
		FORBIDDEN_REQUEST,
		FILE_REQUEST,
		INTERNAL_ERROR,
		CLOSED_CONNECTION
	};

	enum LINE_STATUS {
		LINE_OK = 0,
		LINE_BAD,
		LINE_OPEN
	};

	HTTP_CODE process_read();
	HTTP_CODE parse_request_line(char *text);
	HTTP_CODE parse_headers(char *text);
	HTTP_CODE parse_content(char *text);
	HTTP_CODE do_request();

	LINE_STATUS parse_line();

private:
	int sockfd;
	sockaddr_in client_address;
	char read_buf_[read_buf_size];
	int read_index_;    // 标识当前缓冲区指针所在位置

	int parse_char_index_;
	int start_line_index_;
	CHECK_STATE check_state_;

	char * url_;
	char * version_;
	METHOD method_;
	char * host_;
	bool keep_alive_;

	void init_states();
	char * get_line() {
		return read_buf_ + start_line_index_;
	}

};

void add_fd(int epollfd, int fd, bool one_shot);
void remove_fd(int epollfd, int fd);
void modify_fd(int epollfd, int fd, int evnt);
void set_nonblocking(int fd);

#endif
