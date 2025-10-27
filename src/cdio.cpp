#include "cdio.h"
#include "simapi.h"

// 服务器构造
asio_io::asio_io(asio::io_context &io_context, long port, Protocol proto)
	: protocol_(proto),
	tcp_socket_(io_context),
	tcp_acceptor_(io_context, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), port)),
	tcp_resolver_(io_context),
	udp_socket_(io_context, asio::ip::udp::endpoint(asio::ip::udp::v4(), port)),
	udp_resolver_(io_context)
{
	if (proto == Protocol::TCP) {
		//tcp_acceptor_.set_option(asio::ip::tcp::acceptor::reuse_address(true));
		std::cout << "TCP Server listening on port " << port << std::endl;
	}
	else {
		std::cout << "UDP Server listening on port " << port << std::endl;
	}
}

// 客户端构造
asio_io::asio_io(asio::io_context &io_context, const char* ip, long port, Protocol proto)
	: protocol_(proto),
	tcp_socket_(io_context),
	tcp_acceptor_(io_context),
	tcp_resolver_(io_context),
	udp_socket_(io_context, asio::ip::udp::endpoint(asio::ip::udp::v4(), 0)), // 客户端使用随机端口
	udp_resolver_(io_context)
{
	if (proto == Protocol::TCP) {
		tcp_endpoints_ = tcp_resolver_.resolve(asio::ip::tcp::v4(), ip, std::to_string(port));
		std::cout << "TCP Client targeting " << ip << ":" << port << std::endl;
	}
	else {
		udp_endpoints_ = udp_resolver_.resolve(asio::ip::udp::v4(), ip, std::to_string(port));
		udp_target_endpoint_ = *udp_endpoints_.begin();
		std::cout << "UDP Client targeting " << ip << ":" << port << std::endl;
	}
}

asio_io::~asio_io()
{
	do_close();
}

// ==================== TCP ====================
// TCP服务器接受连接
void asio_io::tcp_accept(simapi &api)
{
	if (protocol_ != Protocol::TCP) {
		std::cerr << "Not a TCP server instance" << std::endl;
		return;
	}

	tcp_acceptor_.async_accept(tcp_socket_, [this, &api](std::error_code ec) {
		if (!ec) {
			std::cout << "TCP Client connected!" << std::endl;
			connected_ = true;

			// 开始接收数据
			tcp_receive(api);
		}
		else {
			std::cerr << "Accept error: " << ec.message() << std::endl;
			// 继续接受新连接
			tcp_accept(api);
		}
	});
}


// TCP服务器接收数据
void asio_io::tcp_receive(simapi &api)
{
	if (!tcp_socket_.is_open()) return;

	tcp_socket_.async_read_some(asio::buffer(data_, MAX_LENGTH),
		[this, &api](std::error_code ec, std::size_t bytes_recvd) {
		if (!ec && bytes_recvd > 0) {
			try {
				// 处理接收到的数据
				std::string received_data(data_, bytes_recvd);
				nlohmann::json recv_data = nlohmann::json::parse(received_data);

				// 这里可以调用api的处理方法
				// api.handle_control_message(j);
				std::cout << recv_data.dump(4) << std::endl;

				if (!recv_data["courseId"].is_null()) {
					nlohmann::json j;
					j["routeId"] = "25D55AD283AA400AF464C76D713C07AD000001";
					j["sdRouteInfo"] = "xxxx";
					tcp_send_json(j);
				}

			}
			catch (const nlohmann::json::exception& e) {
				std::cerr << "JSON parse error: " << e.what() << std::endl;
			}

			// 继续接收
			tcp_receive(api);
		}
		else {
			if (ec) {
				std::cout << "Client disconnected: " << ec.message() << std::endl;
			}
			do_close();
			// 重新开始接受新连接
			tcp_accept(api);
		}
	});
}

// TCP服务器发送数据
void asio_io::tcp_send(std::size_t length)
{
	if (!tcp_socket_.is_open()) return;

	asio::async_write(tcp_socket_, asio::buffer(data_, length),
		[this](std::error_code ec, std::size_t /*bytes_sent*/) {
		if (ec) {
			std::cerr << "Send error: " << ec.message() << std::endl;
			do_close();
		}
	});
}

// TCP服务器发送json数据
void asio_io::tcp_send_json(const nlohmann::json& j) {
	if (!tcp_socket_.is_open()) return;

	// 将 JSON 转换为字符串
	std::string json_str = j.dump(); // 或者 j.dump(-1, ' ', false, nlohmann::json::error_handler_t::ignore)

	// 使用 shared_ptr 管理数据生命周期
	auto send_data = std::make_shared<std::string>(std::move(json_str));

	asio::async_write(tcp_socket_, asio::buffer(*send_data),
		[this, send_data](std::error_code ec, std::size_t bytes_sent) {
		if (ec) {
			std::cerr << "Send error: " << ec.message() << std::endl;
			do_close();
		}
		else {
			std::cout << "Sent JSON data: " << bytes_sent << " bytes" << std::endl;
		}
	});
}

// ==================== UDP ====================
// UDP 接收
void asio_io::udp_receive(simapi &api)
{
	if (protocol_ != Protocol::UDP) {
		std::cerr << "Not a UDP instance" << std::endl;
		return;
	}

	udp_socket_.async_receive_from(
		asio::buffer(data_, MAX_LENGTH),
		udp_sender_endpoint_,
		[this, &api](std::error_code ec, std::size_t bytes_recvd) {
		if (!ec && bytes_recvd > 0) {
			try {
				nlohmann::json j = nlohmann::json::parse(std::string(data_, bytes_recvd));
				// 处理数据
				std::cout << "UDP received " << bytes_recvd << " bytes" << std::endl;
			}
			catch (const nlohmann::json::exception& e) {
				std::cerr << "UDP JSON parse error: " << e.what() << std::endl;
			}
			memset(data_, 0, MAX_LENGTH);
		}
		udp_receive(api); // 继续接收
	});
}

// UDP 发送
void asio_io::udp_send(std::size_t length)
{
	if (protocol_ != Protocol::UDP) return;

	udp_socket_.async_send_to(
		asio::buffer(data_, length),
		udp_sender_endpoint_,
		[](std::error_code, std::size_t) {});
}

// 通用发送（根据协议类型选择）
void asio_io::cli_send(nlohmann::json &j)
{
	std::string data = j.dump();

	if (protocol_ == Protocol::TCP) {
		if (tcp_socket_.is_open()) {
			asio::write(tcp_socket_, asio::buffer(data));
		}
	}
	else {
		udp_socket_.send_to(asio::buffer(data), udp_target_endpoint_);
	}
}


// 关闭连接
void asio_io::do_close()
{
	std::error_code ec;
	// 关闭 socket
	if (protocol_ == Protocol::TCP) {
		if (tcp_socket_.is_open()) {
			tcp_socket_.shutdown(asio::ip::tcp::socket::shutdown_both, ec);
			tcp_socket_.close(ec);
		}
		//if (tcp_acceptor_.is_open()) {
		//	tcp_acceptor_.close(ec);
		//}
	}
	else {
		if (udp_socket_.is_open()) {
			udp_socket_.close(ec);
		}
	}
	connected_ = false;
}