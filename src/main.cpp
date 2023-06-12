
// http://124.222.5.77:80


#include <getopt.h>
#include <string>

#include "event_loop.h"
#include "server.h"
#include "ulog.h"


using std::string;

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
				if (log_path.size() < 2 || optarg[0] != '/') {
					printf("logPath should start with \"/\"\n");
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


// http://124.222.5.77:80
int main(int argc, char *argv[]) {
	int thread_num = 4;
	int port = 80;
	string log_path = "./WebServer.log";
	// parse args
	parse_args(argc, argv, thread_num, log_path, port);
	ulog::set_log_filename(log_path);
	// main_loop
	event_loop main_loop;
	server http_server(&main_loop, thread_num, port);
	http_server.start();
	main_loop.loop();
	return 0;
}
