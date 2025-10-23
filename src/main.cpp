#include "cdio.h"
#include "simapi.h"


void Thread_TCP_Server(simapi& api)
{
	asio::io_context io_context;
	asio_io io_inst(io_context, 9009);

	// 开始接受客户端连接
	io_inst.ser_accept(api);

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

int main(int argc, char* argv[])
{
	while (true)
	{
		simapi api("0");

		Thread_TCP_Server(api);
	}
	return 0;
}