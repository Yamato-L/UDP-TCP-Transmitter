#include "cdio.h"
#include "simapi.h"

// TCP 服务器构造
asio_io::asio_io(asio::io_context &io_context, short port)
	: socket_(io_context),
	acceptor_(io_context, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), port)),
	resolver(io_context)
{
	std::cout << "TCP Server listening on port " << port << std::endl;
}

asio_io::~asio_io()
{
	do_close();
}

// 服务器接受连接
void asio_io::ser_accept(simapi &api)
{
	acceptor_.async_accept(socket_, [this, &api](std::error_code ec) {
		if (!ec) {
			std::cout << "TCP Client connected!" << std::endl;
			connected_ = true;

			// 开始接收数据
			ser_receive(api);
		}
		else {
			std::cerr << "Accept error: " << ec.message() << std::endl;
			// 继续接受新连接
			ser_accept(api);
		}
	});
}


// 服务器接收数据
void asio_io::ser_receive(simapi &api)
{
	if (!socket_.is_open()) return;

	socket_.async_read_some(asio::buffer(data_, MAX_LENGTH),
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
					ser_send_json(j);
				}

			}
			catch (const nlohmann::json::exception& e) {
				std::cerr << "JSON parse error: " << e.what() << std::endl;
			}

			// 继续接收
			ser_receive(api);
		}
		else {
			if (ec) {
				std::cout << "Client disconnected: " << ec.message() << std::endl;
			}
			do_close();
			// 重新开始接受新连接
			ser_accept(api);
		}
	});
}

// 服务器发送数据
void asio_io::ser_send(std::size_t length)
{
	if (!socket_.is_open()) return;

	asio::async_write(socket_, asio::buffer(data_, length),
		[this](std::error_code ec, std::size_t /*bytes_sent*/) {
		if (ec) {
			std::cerr << "Send error: " << ec.message() << std::endl;
			do_close();
		}
	});
}

//服务器发送json数据
void asio_io::ser_send_json(const nlohmann::json& j) {
	if (!socket_.is_open()) return;

	// 将 JSON 转换为字符串
	std::string json_str = j.dump(); // 或者 j.dump(-1, ' ', false, nlohmann::json::error_handler_t::ignore)

	// 使用 shared_ptr 管理数据生命周期
	auto send_data = std::make_shared<std::string>(std::move(json_str));

	asio::async_write(socket_, asio::buffer(*send_data),
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

// 关闭连接
void asio_io::do_close()
{
	std::error_code ec;
	// 关闭 socket
	if (socket_.is_open()) {
		socket_.shutdown(asio::ip::tcp::socket::shutdown_both, ec);
		socket_.close(ec);
	}
	//if (acceptor_.is_open()) {
	//	acceptor_.close(ec);
	//}
	connected_ = false;
}