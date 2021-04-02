#include <iostream>
#include <string>
#include <thread>
#include "event.hpp"
#include "socket.hpp"

int main(int argc, char** argv)
{
	using namespace std;

	if(argc != 3)
	{
		cerr << "USAGE: " << argv[0] << " [host name] [port]" << endl;
		return 1;
	}

	try
	{
		auto host = argv[1];
		auto port = stoul(argv[2]);
		auto event = auto_event(false);
		auto client = client_socket(host, port);

		client.set_data_handler([&](client_socket& cs, socket_buffer_t data)
		{
			auto msg = string((const char*)data.data(), data.size());
			if(!msg.empty()) cout << "< " << msg << endl;
			event.signal();
		});

		thread([&]()
		{
			cout << "[enter] to send, ['q'] to exit, ['die'] to stop server" << endl;

			while(true)
			{
				cout << "> ";
				auto line = string();
				getline(cin, line);

				if(line.empty())
					continue;

				if(line == "q")
				{
					client.close();
					break;
				}

				client.send(line.data(), line.length());
				event.wait();
			}
		}).detach();

		while(client.receive());
	}
	catch(std::exception& ex)
	{
		cout << ex.what() << endl;
	}
}
