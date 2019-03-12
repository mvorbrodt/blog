#include <iostream>
#include <thrift/transport/TServerSocket.h>
#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/transport/TTransportUtils.h>
#include <thrift/server/TSimpleServer.h>
#include "ThriftService.h"

using namespace std;
using namespace apache::thrift;
using namespace apache::thrift::protocol;
using namespace apache::thrift::transport;
using namespace apache::thrift::server;
using namespace ThriftService;

class ThriftServiceHandler : public ThriftServiceIf
{
public:
	ThriftServiceHandler() = default;

	void ping() override { cout << "ping()" << endl; }
	void hello(const string& msg) override { cout << msg << endl; }
	void async_call() override { cout << "async_call()" << endl; }
};

int main(int argc, char** argv)
{
	TSimpleServer server(
		std::make_shared<ThriftServiceProcessor>(std::make_shared<ThriftServiceHandler>()),
		std::make_shared<TServerSocket>(9090),
		std::make_shared<TBufferedTransportFactory>(),
		std::make_shared<TBinaryProtocolFactory>());

	cout << "Starting the server..." << endl;

	server.serve();

	return 1;
}
