#include <iostream>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/uuid/uuid_generators.hpp>
using namespace std;
using namespace boost::uuids;

int main(int argc, char** argv)
{
	uuid id = random_generator()();

	cout << id << endl;

	return 1;
}
