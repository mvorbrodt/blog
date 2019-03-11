#include <iostream>
#include <grpc/grpc.h>
#include <grpc++/channel.h>
#include <grpc++/client_context.h>
#include <grpc++/create_channel.h>
#include <grpc++/security/credentials.h>
#include "grpc_service.grpc.pb.h"

using namespace std;
using namespace grpc;

class gRPCClient
{
public:
	gRPCClient(shared_ptr<Channel> channel)
	: stub(gRPCService::NewStub(channel)) {}

	string hello(const std::string& msg)
	{
		Input input;
		Output output;
		ClientContext context;

		input.set_input(msg);

		Status status = stub->hello(&context, input, &output);
		if (status.ok())
		{
			return output.output();
		}
		else
		{
			cout << status.error_code() << ": " << status.error_message() << endl;
			return "RPC failed";
		}
	}

private:
	unique_ptr<gRPCService::Stub> stub;
};

int main(int argc, char** argv)
{
	gRPCClient client(grpc::CreateChannel("localhost:50051", grpc::InsecureChannelCredentials()));
	string reply = client.hello("gRPC client says hi!");
	cout << "Client received: " << reply << endl;

	return 1;
}
