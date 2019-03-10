#include <iostream>
#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/server/TSimpleServer.h>
#include <thrift/transport/TServerSocket.h>
#include <thrift/transport/TSocket.h>
#include <thrift/transport/TTransportUtils.h>
#include <thrift/TToString.h>
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
		std::shared_ptr<TSocket> sock = std::dynamic_pointer_cast<TSocket>(connInfo.transport);
		cout << endl;
		cout << "Incoming connection" << endl;
		cout << "SocketInfo : " << sock->getSocketInfo() << endl;
		cout << "PeerHost   : " << sock->getPeerHost() << endl;
		cout << "PeerAddress: " << sock->getPeerAddress() << endl;
		cout << "PeerPort   : " << sock->getPeerPort() << endl;
		return new ServiceHandler;
	}

	void releaseHandler(ServiceIf* handler) override
	{
		delete handler;
	}
};

int main()
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
