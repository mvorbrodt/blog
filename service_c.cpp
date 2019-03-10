#include <iostream>
#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/transport/TSocket.h>
#include <thrift/transport/TTransportUtils.h>
#include "Service.h"

using namespace std;
using namespace apache::thrift;
using namespace apache::thrift::protocol;
using namespace apache::thrift::transport;
using namespace Service;

int main(int argc, char** argv)
{
	std::shared_ptr<TTransport> socket(new TSocket("localhost", 9090));
	std::shared_ptr<TTransport> transport(new TBufferedTransport(socket));
	std::shared_ptr<TProtocol> protocol(new TBinaryProtocol(transport));
	ServiceClient client(protocol);

	try
	{
		transport->open();

		client.ping();
		client.hello("Martin says hi!");
		client.async_call();

		transport->close();
	}
	catch(TException& tx)
	{
		cout << "ERROR: " << tx.what() << endl;
	}

	return 1;
}
