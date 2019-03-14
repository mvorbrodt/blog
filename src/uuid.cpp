#include <iostream>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/uuid/uuid_generators.hpp>

using namespace std;
using namespace boost::uuids;

int main(int argc, char** argv)
{
	auto gen = random_generator();
	uuid id = gen();

	cout << id << endl;

	return 1;
}
