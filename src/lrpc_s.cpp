#include <iostream>
#include <iomanip>
#include <string>
#include <thread>
#include <map>
#include <utility>
#include <functional>
#include <cstdint>
#include "lsf.hpp"
#include "socket.hpp"
#include "lrpc_proto.hpp"

class Server
{
public:
	Server(std::uint16_t port)
	: m_socket{ port }
	{
		m_cmd_handlers[cmd_t(CMD::Add)] = [this](client_socket& self, socket_buffer_t data, const host_info_t& hi)
		{
			auto ut = m_serializer.unpack<PAdd>(data);
			auto& packet = std::get<0>(ut);
			auto& name = (hi.host.empty() ? hi.ip : hi.host);

			std::cout << "Client (" << name << ") say PAdd(" << int(packet.hdr.seq) << ") "
				<< std::setfill(' ') << std::setw(3) << packet.num1 << " + " << std::setw(3) << packet.num2 << std::endl;

			auto reply_packet = PAddRep{ { cmd_t(CMD::AddRep), packet.hdr.seq }, packet.num1 + packet.num2 };
			auto reply_data = m_serializer.pack(reply_packet);

			self.send(reply_data.data(), reply_data.size());
		};

		m_cmd_handlers[cmd_t(CMD::Sub)] = [this](client_socket& self, socket_buffer_t data, const host_info_t& hi)
		{
			auto ut = m_serializer.unpack<PSub>(data);
			auto& packet = std::get<0>(ut);
			auto& name = (hi.host.empty() ? hi.ip : hi.host);

			std::cout << "Client (" << name << ") say PSub(" << int(packet.hdr.seq) << ") "
				<< std::setfill(' ') << std::setw(3) << packet.num1 << " - " << std::setw(3) << packet.num2 << std::endl;

			auto reply_packet = PSubRep{ { cmd_t(CMD::SubRep), packet.hdr.seq }, packet.num1 - packet.num2 };
			auto reply_data = m_serializer.pack(reply_packet);

			self.send(reply_data.data(), reply_data.size());
		};

		m_socket.set_accept_handler([this](server_socket& ss, client_socket cs, host_info_t hi)
		{
			cs.set_data_handler([this, hi = std::move(hi)](client_socket& self, socket_buffer_t data)
			{
				auto ut = m_serializer.unpack<PHead>(data);
				auto& header = std::get<0>(ut);
				auto& handler = m_cmd_handlers.at(header.cmd);

				handler(self, std::move(data), hi);
			});

			std::thread([cs = std::move(cs)]() mutable
			{
				while(cs.receive());
			}).detach();
		});
	}

	void Run() { while(m_socket.accept()); }
	void Stop() { m_socket.close(); }

private:
	server_socket m_socket;
	serializer m_serializer;

	using cmd_handler_t = std::function<void(client_socket&, socket_buffer_t, const host_info_t&)>;
	using cmd_handler_map_t = std::map<cmd_t, cmd_handler_t>;
	cmd_handler_map_t m_cmd_handlers;
};

int main(int argc, char** argv)
{
	using namespace std;

	if(argc != 2)
	{
		cerr << "USAGE: " << argv[0] << " [port]" << endl;
		return 1;
	}

	try
	{
		auto port = uint16_t(stoul(argv[1]));
		auto s = Server(port);

		s.Run();
	}
	catch(exception& ex)
	{
		cerr << ex.what() << endl;
	}
}
