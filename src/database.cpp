#include <iostream>
#include <string>
#include <exception>
#include <soci/soci.h>
#include <soci/mysql/soci-mysql.h>

using namespace soci;
using namespace std;

struct Person
{
	int ID;
	string FirstName;
	string LastName;
	int DOB;
	string EMail;
};

namespace soci {
template<>
struct type_conversion<Person>
{
	typedef values base_type;

	static void from_base(const values& v, indicator, Person& p)
	{
		p.ID = v.get<int>("ID");
		p.FirstName = v.get<string>("FirstName");
		p.LastName = v.get<string>("LastName");
		p.DOB = v.get<int>("DOB");
		p.EMail = v.get<string>("EMail");
	}

	static void to_base(const Person& p, values& v, indicator& ind)
	{
		v.set("ID", p.ID);
		v.set("FirstName", p.FirstName);
		v.set("LastName", p.LastName);
		v.set("DOB", p.DOB);
		v.set("EMail", p.EMail);
		ind = i_ok;
	}
};
}

int main()
{
	try
	{
		session sql(mysql, "host=localhost user=root password=''");

		sql << "CREATE DATABASE blog";
		sql << "USE blog";
		sql << "CREATE TABLE people (ID INT, FirstName TEXT, LastName TEXT, DOB INT, EMail TEXT)";

		Person him{1, "Martin", "Vorbrodt", 19800830, "martin@vorbrodt.blog"};
		Person her{2, "Dorota", "Vorbrodt", 19800127, "dorota@vorbrodt.blog"};
		sql << "INSERT INTO people (ID, FirstName, LastName, DOB, EMail) VALUES (:ID, :FirstName, :LastName, :DOB, :EMail)", use(him);
		sql << "INSERT INTO people (ID, FirstName, LastName, DOB, EMail) VALUES (:ID, :FirstName, :LastName, :DOB, :EMail)", use(her);

		int count{};
		sql << "SELECT COUNT(*) FROM people", into(count);
		cout << "Table 'people' has " << count << " row(s)" << endl;

		Person pOut{};
		sql << "SELECT * FROM people WHERE ID = 1", into(pOut);
		cout << pOut.FirstName << ", " << pOut.LastName << ", " << pOut.DOB << ", " << pOut.EMail << endl;

		sql << "DROP TABLE people";
		sql << "DROP DATABASE blog";
	}
	catch (exception& e)
	{
		cerr << e.what() << endl;
	}
}
