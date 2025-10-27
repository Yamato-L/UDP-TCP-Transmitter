
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
	enum class Protocol { TCP, UDP };

	// TCP 服务器
	asio_io(asio::io_context &io_context, long port, Protocol proto = Protocol::TCP);
	// TCP/UDP 客户端
	asio_io(asio::io_context &io_context, const char* ip, long port, Protocol proto = Protocol::TCP);

	~asio_io();

	// TCP
	void tcp_accept(simapi &api);
	void tcp_receive(simapi &api);
	void tcp_send(std::size_t length);
	void tcp_send_json(const nlohmann::json& j);

	// UDP
	void udp_receive(simapi &api);
	void udp_send(std::size_t length);
	//void udp_send_to(const std::string& data, const std::string& ip = "", long port = 0);

	// 客户端发送（兼容TCP/UDP）
	void cli_send(const std::string& data);
	void cli_send_json(nlohmann::json &j);

	void do_close();
	bool is_connected() const { return tcp_socket_.is_open(); }

private:
	char data_[MAX_LENGTH];
	Protocol protocol_;

	// TCP 
	asio::ip::tcp::socket tcp_socket_;
	asio::ip::tcp::acceptor tcp_acceptor_;
	asio::ip::tcp::resolver tcp_resolver_;
	asio::ip::tcp::resolver::results_type tcp_endpoints_;

	// UDP 
	asio::ip::udp::socket udp_socket_;
	asio::ip::udp::resolver udp_resolver_;
	asio::ip::udp::resolver::results_type udp_endpoints_;
	asio::ip::udp::endpoint udp_sender_endpoint_;
	asio::ip::udp::endpoint udp_target_endpoint_;

	// 连接状态
	std::atomic<bool> connected_{ false };
};


#endif  // CDIO_H_