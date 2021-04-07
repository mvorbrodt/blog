#include <iostream>
#include <iomanip>
#include <string>
#include <map>
#include <thread>
#include <memory>
#include <chrono>
#include <stdexcept>
#include <functional>
#include <cstdint>
#include <cstdlib>
#include <ctime>
#include "lsf.hpp"
#include "socket.hpp"
#include "event.hpp"
#include "lrpc_proto.hpp"

class RPCClient
{
public:
	template<typename Rep, typename Period>
	RPCClient(const char* host, std::uint16_t port, std::chrono::duration<Rep, Period> timeout)
	: m_socket{ host, port }, m_timeout{ std::chrono::duration_cast<std::chrono::milliseconds>(timeout) }
	{
		m_reply_handlers[cmd_t(CMD::AddRep)] = [this](client_socket&, socket_buffer_t data)
		{
			auto ut = m_serializer.unpack<PAddRep>(data);
			auto& packet = std::get<0>(ut);
			auto& ctx = m_ctx[packet.hdr.seq];

			ctx.add_reply = packet;
			ctx.reply_event->signal();
		};

		m_reply_handlers[cmd_t(CMD::SubRep)] = [this](client_socket&, socket_buffer_t data)
		{
			auto ut = m_serializer.unpack<PSubRep>(data);
			auto& packet = std::get<0>(ut);
			auto& ctx = m_ctx[packet.hdr.seq];

			ctx.sub_reply = packet;
			ctx.reply_event->signal();
		};

		m_socket.set_data_handler([this](client_socket& self, socket_buffer_t data)
		{
			auto ut = m_serializer.unpack<PHead>(data);
			auto& header = std::get<0>(ut);
			auto& handler = m_reply_handlers.at(header.cmd);

			handler(self, std::move(data));
		});
	}

	int Add(int x, int y)
	{
		auto sn = m_next_seq_num++;
		auto cmd = PAdd{ { cmd_t(CMD::Add), sn }, x, y };
		auto data = m_serializer.pack(cmd);
		auto& ctx = m_ctx[sn];

		m_socket.send(data.data(), data.size());
		auto got_it = ctx.reply_event->wait_for(m_timeout);
		auto reply = ctx.add_reply;
		m_ctx.erase(sn);

		if(!got_it)
			throw std::runtime_error("RPCClient::Add timeout waiting for reply!");

		return reply.res;
	}

	int Sub(int x, int y)
	{
		auto sn = m_next_seq_num++;
		auto cmd = PSub{ { cmd_t(CMD::Sub), sn }, x, y };
		auto data = m_serializer.pack(cmd);
		auto& ctx = m_ctx[sn];

		m_socket.send(data.data(), data.size());
		auto got_it = ctx.reply_event->wait_for(m_timeout);
		auto reply = ctx.sub_reply;
		m_ctx.erase(sn);

		if(!got_it)
			throw std::runtime_error("RPCClient::Sub timeout waiting for reply!");

		return reply.res;
	}

	void Start() { std::thread([this]() { while(m_socket.receive()); }).detach(); }
	void Stop() { m_socket.close(); }

private:
	seq_t m_next_seq_num = 1;
	client_socket m_socket;
	serializer m_serializer;
	std::chrono::milliseconds m_timeout;

	using reply_handler_t = std::function<void(client_socket&, socket_buffer_t)>;
	using reply_handler_map_t = std::map<cmd_t, reply_handler_t>;
	reply_handler_map_t m_reply_handlers;

	struct cmd_ctx_t
	{
		using e_ptr = std::unique_ptr<auto_event>;
		e_ptr reply_event = std::make_unique<auto_event>();

		union
		{
			PAddRep add_reply;
			PSubRep sub_reply;
		};
	};

	using cmd_ctx_map_t = std::map<seq_t, cmd_ctx_t>;
	cmd_ctx_map_t m_ctx;
};

int main(int argc, char** argv)
{
	using namespace std;
	using namespace std::chrono;

	if(argc != 3)
	{
		cerr << "USAGE: " << argv[0] << " [host name] [port]" << endl;
		return 1;
	}

	try
	{
		srand(time(NULL));

		auto host = argv[1];
		auto port = stoul(argv[2]);
		auto c = RPCClient(host, port, 1s);

		c.Start();

		for(;;)//int i = 1; i <= 99; ++i)
		{
			auto num1 = 50 - (rand() % 101);
			auto num2 = 50 - (rand() % 101);
			auto add = c.Add(num1, num2);
			cout << setfill(' ') << setw(3) << num1 << " + " << setw(3) << num2 << " = " << setw(3) << add << std::endl;
			this_thread::sleep_for(250ms);

			auto num3 = 50 - (rand() % 101);
			auto num4 = 50 - (rand() % 101);
			auto sub = c.Sub(num3, num4);
			cout << setfill(' ') << setw(3) << num3 << " - " << setw(3) << num4 << " = " << setw(3) << sub << std::endl;
			this_thread::sleep_for(250ms);
		}
	}
	catch(exception& ex)
	{
		cerr << ex.what() << endl;
	}
}
