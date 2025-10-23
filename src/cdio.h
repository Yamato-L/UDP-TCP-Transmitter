
#ifndef CDIO_H_
#define CDIO_H_

#include <iostream>
#include <fstream>
#include <string>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <chrono>

#include "asio.hpp"
#include "nlohmann/json.hpp"

#define MAX_LENGTH 1024

// Forward declarations
class simapi;

class asio_io
{
public:
	// TCP 服务器构造
	asio_io(asio::io_context &io_context, short port);
	~asio_io();

	// 服务器方法
	void ser_accept(simapi &api);
	void ser_receive(simapi &api);
	void ser_send(std::size_t length);
	void ser_send_json(const nlohmann::json& j);



	void do_close();
	bool is_connected() const { return socket_.is_open(); }

private:
	char data_[MAX_LENGTH];

	// TCP 相关成员
	asio::ip::tcp::socket socket_;
	asio::ip::tcp::acceptor acceptor_;
	asio::ip::tcp::resolver resolver;

	// 连接状态
	std::atomic<bool> connected_{ false };
};


#endif  // CDIO_H_