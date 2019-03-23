#include <iostream>
#include <string>
#include <stdexcept>
#include <xmlrpc-c/base.hpp>
#include <xmlrpc-c/registry.hpp>
#include <xmlrpc-c/server_abyss.hpp>

using namespace std;

class hello : public xmlrpc_c::method
{
public:
	void execute(const xmlrpc_c::paramList& params, xmlrpc_c::value* retval)
	{
		string msg(params.getString(0));
		params.verifyEnd(1);

		cout << msg << endl;

		*retval = xmlrpc_c::value_string("XMLRPC server says hello!");
	}
};

int main(int argc, char** argv)
{
	xmlrpc_c::registry registry;
	registry.addMethod("hello", new hello);

	xmlrpc_c::serverAbyss server(xmlrpc_c::serverAbyss::constrOpt().registryP(&registry).portNumber(8080));
	server.run();

	return 1;
}
