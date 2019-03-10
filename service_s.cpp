#include <iostream>
#include <thrift/transport/TSocket.h>
#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/transport/TServerSocket.h>
#include <thrift/transport/TTransportUtils.h>
#include <thrift/server/TSimpleServer.h>
#include "Service.h"

using namespace std;
using namespace apache::thrift;
using namespace apache::thrift::protocol;
using namespace apache::thrift::transport;
using namespace apache::thrift::server;
using namespace Service;

class ServiceHandler : public ServiceIf
{
public:
	ServiceHandler() = default;

	void ping() override { cout << "ping()" << endl; }
	void hello(const string& msg) override { cout << msg << endl; }
	void async_call() override { cout << "async_call()" << endl; }
};

class ServiceCloneFactory : virtual public ServiceIfFactory
{
public:
	ServiceCloneFactory() = default;

	ServiceIf* getHandler(const ::apache::thrift::TConnectionInfo& connInfo) override
	{
		auto sock = std::dynamic_pointer_cast<TSocket>(connInfo.transport);
		cout << "Incoming connection" << endl;
		cout << "\tSocketInfo : " << sock->getSocketInfo() << endl;
		cout << "\tPeerHost   : " << sock->getPeerHost() << endl;
		cout << "\tPeerAddress: " << sock->getPeerAddress() << endl;
		cout << "\tPeerPort   : " << sock->getPeerPort() << endl;
		return new ServiceHandler;
	}

	void releaseHandler(ServiceIf* handler) override
	{
		delete handler;
	}
};

int main(int argc, char** argv)
{
	TSimpleServer server(
		std::make_shared<ServiceProcessorFactory>(std::make_shared<ServiceCloneFactory>()),
		std::make_shared<TServerSocket>(9090),
		std::make_shared<TBufferedTransportFactory>(),
		std::make_shared<TBinaryProtocolFactory>());

	cout << "Starting the server..." << endl;

	server.serve();

	return 1;
}
