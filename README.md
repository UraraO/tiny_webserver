# C++ multi-thread webserver

[![standard-readme compliant](https://img.shields.io/badge/readme%20style-standard-brightgreen.svg?style=flat-square)](https://github.com/RichardLitt/standard-readme)  

## Introduction
本项目是一个基于C++11的web服务器，可以解析get，head请求，支持http长连接，采用Reactor并发模型，使用异步日志系统记录日志。
参考了linyacool的WebServer：https://github.com/linyacool/WebServer

## Table of Contents

| Part Ⅰ | Part Ⅱ |
| :--------: | :---------: |
| [并发模型](https://github.com/UraraO/tiny_WebServer/tree/main/doc/concurrency_frame.md)|[异步日志系统](https://github.com/UraraO/tiny_WebServer/tree/main/doc/Log_system.md)|

- [Install](#install)
- [Usage](#usage)
- [Technical Details](#technical-details)
- [Model](#model)
- [Webbench](#webbench)

## Install
	./build.sh

## Usage
	./tiny_WebServer [-t thread_numbers] [-p port] [-l log_file_path]
	./tiny_WebServer -t 4 -p 80 -l ./webserver.log
relative path maybe "./webserver.log", absolute path maybe "user/log_path/webserver.log"

## Technical Details
- 使用Reactor并发模型，主Reactor接收连接，多个从Reactor处理连接
- 使用Epoll边沿触发的IO多路复用技术，非阻塞IO
- 使用多线程充分利用多核CPU，并使用线程池避免线程频繁创建销毁的开销
- 使用基于小根堆的定时器关闭超时请求
- 主线程只负责accept请求，并以Round Robin的方式分发给其它IO线程(兼计算线程)，锁的争用只会出现在主线程和某一特定线程中
- 使用eventfd实现了线程的异步唤醒
- 使用多缓冲区技术实现了简单的异步日志系统
- 大量使用智能指针等RAII技术，防止内存泄漏
- 使用状态机解析了HTTP请求,支持管线化
- 支持优雅关闭连接

## Model
Reactor并发模型，其中主Reactor即main_loop，负责接收客户端连接请求，多个从Reactor即server的thread_pool中的多个IO线程，负责处理连接的IO事件
![并发模型](https://github.com/UraraO/tiny_webserver/blob/main/doc/concurrency_frame.jpg)

## Webbench
[![压力测试结果](https://github.com/UraraO/tiny_webserver/blob/main/doc/webbench_result.jpg)

