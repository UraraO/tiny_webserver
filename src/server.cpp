

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <functional>

#include "server.h"
#include "util.h"
#include "ulog.h"


server::server(event_loop *loop, int thread_num, int port) :
	loop_(loop),
	thread_num_(thread_num),
	m_pool(new event_loop_thread_pool(loop, thread_num)),
	started_(false),
	accept_channel_(new channel(loop)),
	port_(port),
	listen_fd_(socket_bind_listen(port))
{
	accept_channel_->set_fd(listen_fd_);
	handle_for_sigpipe();
	if(setSocketNonBlocking(listen_fd_) < 0) {
		perror("set socket non block failed");
		abort();
	}
}

void server::start() {
	m_pool->start();
	// acceptChannel_->setEvents(EPOLLIN | EPOLLET | EPOLLONESHOT);
	accept_channel_->set_events(EPOLLIN | EPOLLET);
	accept_channel_->set_read_handler([this] { handle_new_conn(); });
	accept_channel_->set_conn_handler([this] { handle_this_conn(); });
	loop_->add_to_poller(accept_channel_, 0);
	started_ = true;
}

void server::handle_new_conn() {
	sockaddr_in client_addr;
	memset(&client_addr, 0, sizeof(sockaddr_in));
	socklen_t client_addr_len = sizeof(client_addr);
	int accept_fd = 0;
	while ((accept_fd = accept(listen_fd_, (sockaddr *)&client_addr, &client_addr_len)) > 0) {
		event_loop *loop = m_pool->get_next_loop();
		LOG << "New connection from " << inet_ntoa(client_addr.sin_addr) << ":"
		    << ntohs(client_addr.sin_port);
		// cout << "new connection" << endl;
		// cout << inet_ntoa(client_addr.sin_addr) << endl;
		// cout << ntohs(client_addr.sin_port) << endl;
		/*
		// TCP的保活机制默认是关闭的
		int optval = 0;
		socklen_t len_optval = 4;
		getsockopt(accept_fd, SOL_SOCKET,  SO_KEEPALIVE, &optval, &len_optval);
		cout << "optval ==" << optval << endl;
		*/
		// 限制服务器的最大并发连接数
		if (accept_fd >= MAXFD) {
			close(accept_fd);
			continue;
		}
		// 设为非阻塞模式
		if (setSocketNonBlocking(accept_fd) < 0) {
			LOG << "Set non block failed!";
			// perror("Set non block failed!");
			return;
		}

		setSocketNodelay(accept_fd);
		// setSocketNoLinger(accept_fd);

		shared_ptr<http_data> req_info(new http_data(loop, accept_fd));
		req_info->get_channel()->set_holder(req_info);
		loop->queue_in_loop([req_info] { req_info->new_event(); });
	}
	accept_channel_->set_events(EPOLLIN | EPOLLET);
}
