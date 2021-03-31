#include <iostream>
#include <string>
#include <thread>
#include "socket.hpp"

int main(int argc, char** argv)
{
	using namespace std;

	if(argc != 3)
	{
		cerr << "USAGE: " << argv[0] << " [host name] [port]" << endl;
		return;
	}

	try
	{
		auto host = argv[1];
		auto port = stoul(argv[2]);
		auto client = client_socket(host, port);

		client.set_data_handler([](client_socket& cs, socket_buffer_t data)
		{
			cout << "< " << string((const char*)data.data(), data.size()) << endl;
		});

		thread([&]()
		{
			cout << "[enter] to send, [ctrl-c] to exit" << endl;

			while(true)
			{
				cout << "> ";
				auto line = string();
				getline(cin, line);
				if(line.empty())
					continue;

				client.send(line.data(), line.length());
				this_thread::sleep_for(250ms);
			}
		}).detach();

		client.start();
	}
	catch(std::exception& ex)
	{
		cout << ex.what() << endl;
	}
}
