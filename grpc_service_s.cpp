#include <iostream>
#include <grpc/grpc.h>
#include <grpcpp/server.h>
#include <grpcpp/server_builder.h>
#include <grpcpp/server_context.h>
#include <grpcpp/security/server_credentials.h>
#include "grpc_service.grpc.pb.h"

using namespace std;
using namespace grpc;

class gRPCServiceImpl final : public gRPCService::Service
{
public:
	Status hello(ServerContext* context, const Input* in, Output* out) override
	{
		cout << in->input() << endl;
		out->set_output("gRPC server says hi!");
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
