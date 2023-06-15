
// http://124.222.5.77:80


#include <getopt.h>
#include <string>

#include "event_loop.h"
#include "server.h"
#include "ulog.h"

#include <iostream>

using std::cin;
using std::cout;

using std::string;

// main函数参数解析
/*
 * 用法：命令行中
 * ./WebServer [-t thread_numbers] [-p port] [-l log_file_path(should begin with '/')]
 * 例如：
 * ./tiny_WebServer -t 4 -p 10000 -l ./webserver_pressure_test_1.log
 * 声明线程数为4，端口号10000，日志文件路径为绝对路径的/webserver_pressure_test_1.log，若需要使用相对路径则输入./webserver_pressure_test_1.log
 *
 * */
void parse_args(int argc, char *argv[], int &thread_num, string &log_path, int &port);
void parse_args(int argc, char *argv[], int &thread_num, string &log_path, int &port) {
	int opt;
	const char *format = "t:l:p:";
	while ((opt = getopt(argc, argv, format)) != -1) {
		switch (opt) {
			case 't': {
				thread_num = atoi(optarg);
				break;
			}
			case 'l': {
				log_path = optarg;
				if(log_path.size() < 2) {
					if(optarg[0] != '/') {
						printf("logPath should start with \"/\"\n");
						abort();
					}
				}
				else if(log_path.size() >= 2 || optarg[0] == '.') {
					printf("log_path is a relative path\n");
				}
				else if(log_path.size() >= 2 || optarg[0] == '/') {
					printf("log_path is a absolute path\n");
				}
				else {
					printf("log_path is not available\n");
					abort();
				}
				break;
			}
			case 'p': {
				port = atoi(optarg);
				break;
			}
			default:
				break;
		}
	}
}


// http://124.222.5.77:80/hello
// http://124.222.5.77:80/favicon.ico
// http://124.222.5.77:10000/hello
// http://124.222.5.77:10000/favicon.ico
int main(int argc, char *argv[]) {
	int thread_num = 4;
	int port = 80;
	string log_path = "./WebServer.log";
	// parse args
	parse_args(argc, argv, thread_num, log_path, port);
	if(log_path == "./WebServer.log") {
		printf("use default log_path\n");
	}
	printf("log_path is: %s\n", log_path.c_str());
	ulog::set_log_filename(log_path);

	event_loop main_loop;   // main_loop内仅保存accept_fd，用于接收新连接，由server维护和接收连接请求
	server http_server(&main_loop, thread_num, port);
	http_server.start();
	main_loop.loop();



	/*printf("type in 'quit' to stop server\n");
	string quit_;
	while(cin >> quit_) {
		if(quit_ == "quit") {
			main_loop.quit();
			main_th.join();
			http_server.get_loop()->quit();
			return 0;
		}
		quit_.clear();
	}*/

	return 0;

}
