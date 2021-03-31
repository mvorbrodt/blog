#include <iostream>
#include <string>
#include <thread>
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
			[](server_socket& ss, client_socket cs, host_info_t info)
			{
				cout << info.host << " (" << info.ip << ") : " << info.port << " connected" << endl;

				cs.set_data_handler([](client_socket& cs, socket_buffer_t data)
				{
					cout << "client says: " << string((const char*)data.data(), data.size()) << endl;
					cs.send(data);
				});

				thread([cs = std::move(cs), info]() mutable
				{
					cs.start();
					cout << info.host << " (" << info.ip << ") : " << info.port << " disconnected" << endl;
				}).detach();
			});

		cout << "starting server on port " << port << " ..." << endl;
		cout << "[ctrl-c] to exit" << endl;
		server.start();
	}
	catch(std::exception& ex)
	{
		cout << ex.what() << endl;
	}
}
