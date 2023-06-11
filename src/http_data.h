
/*
 * http_data,定义了http请求的内容和解析方法
 * 建立连接，读，写
 * event_loop的回调函数即从此处获取定义
 *
 * */


#pragma once
#include <sys/epoll.h>
#include <unistd.h>
#include <functional>
#include <map>
#include <memory>
#include <string>
#include <mutex>
#include <unordered_map>
#include "timer.h"

using std::map;
using std::shared_ptr;
using std::make_shared;
using std::string;
using std::unordered_map;
using std::call_once;
using std::once_flag;
using std::weak_ptr;


class event_loop;
class timer_node;
class channel;

// enum http报文中的多种数据
enum ProcessState {
	STATE_PARSE_URI = 1,
	STATE_PARSE_HEADERS,
	STATE_RECV_BODY,
	STATE_ANALYSIS,
	STATE_FINISH
};

enum URIState {
	PARSE_URI_AGAIN = 1,
	PARSE_URI_ERROR,
	PARSE_URI_SUCCESS,
};

enum HeaderState {
	PARSE_HEADER_SUCCESS = 1,
	PARSE_HEADER_AGAIN,
	PARSE_HEADER_ERROR
};

enum AnalysisState { ANALYSIS_SUCCESS = 1, ANALYSIS_ERROR };

enum ParseState {
	H_START = 0,
	H_KEY,
	H_COLON,
	H_SPACES_AFTER_COLON,
	H_VALUE,
	H_CR,
	H_LF,
	H_END_CR,
	H_END_LF
};

enum ConnectionState { H_CONNECTED = 0, H_DISCONNECTING, H_DISCONNECTED };

enum HttpMethod { METHOD_POST = 1, METHOD_GET, METHOD_HEAD };

enum HttpVersion { HTTP_10 = 1, HTTP_11 };

/* mime：MIME (Multipurpose Internet Mail Extensions) 是描述消息内容类型的标准，用来表示文档、文件或字节流的性质和格式。
 * 浏览器通常使用 MIME 类型（而不是文件扩展名）来确定如何处理URL，因此 We b服务器在响应头中添加正确的 MIME 类型非常重要。如果配置不正确，浏览器可能会无法解析文件内容，网站将无法正常工作，并且下载的文件也会被错误处理。
 * MIME 类型通用结构：
	type/subtype
 * 具体情况参考以下网址
 * https://www.runoob.com/http/mime-types.html
 * */

class MIME_type{
public:
	MIME_type() = default;
	MIME_type(const MIME_type &other) = delete;
	MIME_type(MIME_type &other) = delete;

	static string get_MIME(const string &suffix);

private:
	static void init();
	static unordered_map<string, string> m_mime;
	static once_flag once_flag_;
};


class http_data : public std::enable_shared_from_this<http_data> {
public:
	http_data(event_loop *loop, int connfd);
	~http_data() { close(fd_); }
	void reset();
	void seperate_timer();
	void link_timer(const shared_ptr<timer_node>& mtimer) {
		// shared_ptr重载了bool, 但weak_ptr没有
		timer_ = mtimer;
	}
	shared_ptr<channel> get_channel() { return m_channel; }
	event_loop* getLoop() { return m_loop; }
	void handle_close();
	void new_event();

private:
	event_loop *m_loop;
	shared_ptr<channel> m_channel;
	int fd_;
	string inBuffer_;
	string outBuffer_;
	bool error_;
	ConnectionState connectionState_;

	HttpMethod method_;
	HttpVersion HTTPVersion_;
	string fileName_;
	string path_;
	int nowReadPos_;
	ProcessState state_;
	ParseState hState_;
	bool keepAlive_;
	map<string, string> headers_;
	weak_ptr<timer_node> timer_;

	void handle_read();
	void handle_write();
	void handle_conn();
	void handle_error(int fd, int err_num, string short_msg);
	URIState parseURI();
	HeaderState parse_headers();
	AnalysisState analysis_request();
};












