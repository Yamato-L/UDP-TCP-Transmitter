#include "cdio.h"
#include "simapi.h"


void Thread_TCP_Server(simapi& api)
{
	// TCP
	asio::io_context io_context;
	asio_io tcp_server(io_context, 60001, asio_io::Protocol::TCP);

	// 开始接受客户端连接
	tcp_server.tcp_accept(api);

	std::thread monitor_thread([&io_context, &api]() {
		// 监控逻辑
		while (true)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(1000));
		}
	});

	io_context.run();

	if (monitor_thread.joinable()) {
		monitor_thread.join();
	}

}

void Thread_UDP_Client(simapi &api)
{
	// UDP
	asio::io_context io_context;
	asio_io udp_client(io_context, "10.66.8.138", 8080, asio_io::Protocol::UDP);

	nlohmann::json j;
	j["Id"] = "777";
	j["Message"] = "xxxx";

	while (true)
	{
		udp_client.cli_send("77777777777777777");
		udp_client.cli_send_json(j);
		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
	}

}

int main(int argc, char* argv[])
{
	while (true)
	{
		simapi api("0");

		//Thread_TCP_Server(api);
		//Thread_UDP_Client(api);

		//// TCP
		std::thread threadServer = std::thread(std::bind(&Thread_TCP_Server, std::ref(api)));
		//// UDP
		std::thread threadClient = std::thread(std::bind(&Thread_UDP_Client, std::ref(api)));
		threadServer.join();
		threadClient.join();

	}
	return 0;
}