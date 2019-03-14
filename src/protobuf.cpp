#include <iostream>
#include <string>
#include "protobuf.pb.h"

using namespace std;

int main(int argc, char** argv)
{
	GOOGLE_PROTOBUF_VERIFY_VERSION;

	data::Person p;

	p.set_name("Martin Vorbrodt");
	p.set_dob(19800830);
	p.set_email("martin@vorbrodt.blog");

	string binary;
	p.SerializeToString(&binary);

	data::Person p2;
	p2.ParseFromString(binary);

	cout << "Name  = " << p2.name() << endl;
	cout << "DOB   = " << p2.dob() << endl;
	cout << "EMail = " << p2.email() << endl;
	
	google::protobuf::ShutdownProtobufLibrary();
	return 1;
}
