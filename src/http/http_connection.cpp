#include "http_connection.h"

int http_conn::s_epoll_fd = -1;
int http_conn::s_user_count = 0;

void http_conn::process() {
	// 解析http请求
//	printf("parse http\n");

	HTTP_CODE read_ret = process_read();
	if(read_ret == NO_REQUEST) {
		modify_fd(s_epoll_fd, sockfd, EPOLLIN);
		return;
	}

	// 生成响应
//	printf("resp\n");


}

void http_conn::init(int fd_sock, const sockaddr_in & cli_addr) {
	sockfd = fd_sock;
	client_address = cli_addr;

	int reuse_port = 1;
	setsockopt(fd_sock, SOL_SOCKET, SO_REUSEADDR, &reuse_port, sizeof(reuse_port));

	add_fd(s_epoll_fd, fd_sock, true);
	++s_user_count;

	init_states();
}

void http_conn::close() {
	if(sockfd != -1) {
		remove_fd(s_epoll_fd, sockfd);
		sockfd = -1;
		--s_user_count;
	}
}

bool http_conn::read() {
//	printf("read all\n");
//	return true;
	if(read_index_ >= read_buf_size) {
		return false;
	}

	int read_num_bytes;
	while(true) {
		read_num_bytes = recv(sockfd, read_buf_ + read_index_, read_buf_size - read_index_, 0);
		if(read_num_bytes == -1) {
			if(errno == EAGAIN || errno == EWOULDBLOCK) {
				// 没有数据可读
				break;
			}
			return false;
		}
		else if(read_num_bytes == 0) {
			// 对方关闭连接
			return false;
		}
		read_index_ += read_num_bytes;
		printf("read from client:\n%s\n", read_buf_);
	}

	return true;
}

http_conn::HTTP_CODE http_conn::process_read() {
	printf("process_read\n");

	LINE_STATUS l_stat = LINE_OK;
	HTTP_CODE ret = NO_REQUEST;

	char *text = nullptr;
	while( ((check_state_ == CHECK_STATE_CONTENT) && (l_stat == LINE_OK))
			|| ((l_stat = parse_line()) == LINE_OK)) {
		text = get_line();

		start_line_index_ = parse_char_index_;
		printf("get one line: %s\n", text);

		switch(check_state_) {
			case CHECK_STATE_REQUESTLINE : {
				ret = parse_request_line(text);
				if(ret == BAD_REQUEST) {
					return BAD_REQUEST;
				}
				break;
			}
			case CHECK_STATE_HEADER : {
				ret = parse_headers(text);
				if(ret == BAD_REQUEST) {
					return BAD_REQUEST;
				}
				else if(ret == GET_REQUEST) {
					return do_request();
				}
			}
			case CHECK_STATE_CONTENT : {
				ret = parse_content(text);
				if(ret == GET_REQUEST) {
					return do_request();
				}
				l_stat = LINE_OPEN;
				break;
			}
			default : {
				return INTERNAL_ERROR;
			}
		}
		return NO_REQUEST;
	}

	return NO_REQUEST;
}



http_conn::HTTP_CODE http_conn::parse_request_line(char *text) {
//	printf("parse_request_line\n");
//	return http_conn::NO_REQUEST;
	url_ = strpbrk(text, " \t");
	*url_++ = '\0';
	char * method = text;
	if(strcasecmp(method, "GET") == 0) {
		method_ = GET;
	}
	else {
		return BAD_REQUEST;
	}

	version_ = strpbrk(url_, " \t");
	if(!version_) {
		return BAD_REQUEST;
	}
	*version_++ = '\0';
	if(strcasecmp(version_, "HTTP1.1") != 0) {
		return BAD_REQUEST;
	}

	if(strncasecmp(url_, "http://", 7) == 0) {
		url_ += 7;
		url_ = strchr(url_, '/');
	}
	if(!url_ || url_[0] != '/') {
		return BAD_REQUEST;
	}

	check_state_ = CHECK_STATE_HEADER;

	return NO_REQUEST;
}

http_conn::HTTP_CODE http_conn::parse_headers(char *text) {
	printf("parse_headers\n");
	return http_conn::NO_REQUEST;
}

http_conn::HTTP_CODE http_conn::parse_content(char *text) {
	printf("parse_content\n");
	return http_conn::NO_REQUEST;
}

http_conn::HTTP_CODE http_conn::do_request() {
	return NO_REQUEST;
}

http_conn::LINE_STATUS http_conn::parse_line() {
//	printf("parse_LINE\n");
//	return http_conn::LINE_BAD;
	char c;
	for(; parse_char_index_ < read_index_; ++parse_char_index_) {
		c = read_buf_[parse_char_index_];
		if(c == '\r') {
			if(parse_char_index_ + 1 == read_index_) {
				return LINE_OPEN;
			}
			else if(read_buf_[parse_char_index_ + 1] == '\n') {
				read_buf_[parse_char_index_++] = '\0';
				read_buf_[parse_char_index_++] = '\0';
				return LINE_OK;
			}
			return LINE_BAD;
		}
		else if(c == '\n') {
			if(parse_char_index_ > 1 && read_buf_[parse_char_index_ - 1] == '\r') {
				read_buf_[parse_char_index_ - 1] = '\0';
				read_buf_[parse_char_index_++] = '\0';
				return LINE_OK;
			}
			return LINE_BAD;
		}
	}
}

bool http_conn::write() {
	printf("write all\n");
	return true;
}

void http_conn::init_states() {
	check_state_ = CHECK_STATE_REQUESTLINE;
	parse_char_index_ = 0;
	start_line_index_ = 0;
	read_index_ = 0;

	method_ = GET;
	url_ = nullptr;
	version_ = nullptr;
	host_ = nullptr;
	keep_alive_ = false;
	bzero(read_buf_, read_buf_size);
}


// 向epoll中添加需要监听的文件描述符
void add_fd(int epollfd, int fd, bool one_shot) {
	epoll_event event;
	event.data.fd = fd;
//	*******************
//	event.events = EPOLLIN | EPOLLRDHUP;
	event.events = EPOLLIN | EPOLLRDHUP | EPOLLET;

	if(one_shot) {
		event.events = event.events | EPOLLONESHOT;
	}
	epoll_ctl(epollfd, EPOLL_CTL_ADD, fd, &event);
}

void remove_fd(int epollfd, int fd) {
	epoll_ctl(epollfd, EPOLL_CTL_DEL, fd, 0);
	close(fd);
}

void modify_fd(int epollfd, int fd, int evnt) {
	epoll_event event;
	event.data.fd = fd;
	event.events = evnt | EPOLLONESHOT | EPOLLRDHUP;
	epoll_ctl(epollfd, EPOLL_CTL_MOD, fd, &event);
}

void set_nonblocking(int fd) {
	int old_flag = fcntl(fd, F_GETFL);
	int new_flag = old_flag | O_NONBLOCK;
	fcntl(fd, F_SETFL, new_flag);
}













