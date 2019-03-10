#include <iostream>
#include <thrift/transport/TSocket.h>
#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/transport/TTransportUtils.h>
#include "Service.h"

using namespace std;
using namespace apache::thrift;
using namespace apache::thrift::protocol;
using namespace apache::thrift::transport;
using namespace Service;

int main(int argc, char** argv)
{
	auto socket = make_shared<TSocket>("localhost", 9090);
	auto transport = make_shared<TBufferedTransport>(socket);
	auto protocol = make_shared<TBinaryProtocol>(transport);

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
