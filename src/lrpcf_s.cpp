#include <iostream>
#include <string>
#include <thread>
#include <functional>
#include "socket.hpp"

int main(int argc, char** argv)
{
	using namespace std;

	if(argc != 2)
	{
		cerr << "USAGE: " << argv[0] << " [port]" << endl;
		return;
	}

	try
	{
		auto port = uint16_t(stoul(argv[1]));
		auto server = server_socket(port);

		server.set_accept_handler(
			[&](server_socket& ss, client_socket cs, host_info_t info)
			{
				cout << info.host << " (" << info.ip << ") : " << info.port << " connected" << endl;

				cs.set_data_handler([=, sref = ref(server)](client_socket& cs, socket_buffer_t data)
				{
					auto msg = string((const char*)data.data(), data.size());
					if(msg == "die")
					{
						sref.get().close();
					}
					else
					{
						cout << info.host << " says: " << msg << endl;
						cs.send({ rbegin(data), rend(data) });
					}
				});

				thread([=, cs = std::move(cs)]() mutable
				{
					cs.receive();
					cout << info.host << " (" << info.ip << ") : " << info.port << " disconnected" << endl;
				}).detach();
			});

		cout << "[ctrl-c] to exit, listening on port " << port << endl;
		server.accept();
	}
	catch(std::exception& ex)
	{
		cout << ex.what() << endl;
	}
}
