#include <iostream>
#include <grpc/grpc.h>
#include <grpc++/server.h>
#include <grpc++/server_builder.h>
#include <grpc++/server_context.h>
#include <grpc++/security/server_credentials.h>
#include "grpc_service.grpc.pb.h"

using namespace std;
using namespace grpc;

class gRPCServiceImpl final : public gRPCService::Service
{
public:
	Status hello(ServerContext* context, const Input* in, Output* out) override
	{
		cout << in->input_msg() << endl;
		out->set_output_msg("gRPC server says hi!");
		return Status::OK;
	}
};

int main(int argc, char** argv)
{
	gRPCServiceImpl service;
	ServerBuilder builder;
	builder.AddListeningPort("0.0.0.0:50051", grpc::InsecureServerCredentials());
	builder.RegisterService(&service);
	unique_ptr<Server> server(builder.BuildAndStart());
	cout << "Server starting..." << endl;
	server->Wait();

	return 1;
}
