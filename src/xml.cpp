#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <boost/archive/xml_oarchive.hpp>
#include <boost/archive/xml_iarchive.hpp>
#include <boost/serialization/vector.hpp>

using namespace std;
using namespace boost::archive;
using namespace boost::serialization;

struct person
{
	string name;
	int dob;
	string email;
};

using people = vector<person>;

BOOST_CLASS_VERSION(person, 1);
BOOST_CLASS_VERSION(people, 1);

template<typename Archive>
void serialize(Archive& ar, person& person, const unsigned int version)
{
	ar & BOOST_SERIALIZATION_NVP(person.name);
	ar & BOOST_SERIALIZATION_NVP(person.dob);
	ar & BOOST_SERIALIZATION_NVP(person.email);
}

int main(int argc, char** argv)
{
	person me{"Martin Vorbrodt", 19800830, "martin@vorbrodt.blog"};
	person her{"Dorota Vorbrodt", 19810127, "dorota@vorbrodt.blog"};
	people us{me, her};
	{
		ofstream ofs("data.xml");
		xml_oarchive oa(ofs, boost::archive::no_header);
		oa << make_nvp("people", us);
	}

	people us_too{};
	{
		ifstream ifs("data.xml");
		xml_iarchive ia(ifs, boost::archive::no_header);
		ia >> make_nvp("people", us_too);
	}

	for(auto& p : us_too)
	{
		cout << "Name  : " << p.name << endl;
		cout << "DOB   : " << p.dob << endl;
		cout << "EMail : " << p.email << endl << endl;
	}

	return 1;
}
