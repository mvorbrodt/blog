#include <iostream>
#include <string>
#include <xmlrpc-c/girerr.hpp>
#include <xmlrpc-c/base.hpp>
#include <xmlrpc-c/client_simple.hpp>

using namespace std;

int main(int argc, char** argv)
{
	string serverUrl("http://localhost:8080/RPC2");
	string methodName("hello");

	xmlrpc_c::clientSimple client;
	xmlrpc_c::value result;

	client.call(serverUrl, methodName, "s", &result, "XMLRPC client says hello!");

	auto reply = xmlrpc_c::value_string(result);

	cout << static_cast<string>(reply) << endl;

	return 1;
}
