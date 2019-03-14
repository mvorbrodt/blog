#include <iostream>
#include <string>
#include <exception>
#include <soci/soci.h>
#include <soci/mysql/soci-mysql.h>

using namespace soci;
using namespace std;

int main()
{
	try
	{
		session sql(mysql, "host=localhost user=root password=''");

		sql << "CREATE DATABASE blog";
		sql << "USE blog";
		sql << "CREATE TABLE people (FirstName TEXT, LastName TEXT, DOB INT, EMail TEXT)";
		sql << "INSERT INTO people (FirstName, LastName, DOB, EMail) VALUES ('Martin', 'Vorbrodt', 19800830, 'martin@vorbrodt.blog')";

		int count{};
		sql << "SELECT COUNT(*) FROM people", into(count);
		cout << "Table 'people' has " << count << " row(s)" << endl;

		row r;
		sql << "SELECT * FROM people", into(r);
		string FirstName, LastName, EMail;
		int DOB;
		r >> FirstName >> LastName >> DOB >> EMail;
		cout << FirstName << ", " << LastName << ", " << DOB << ", " << EMail << endl;

		sql << "DROP TABLE people";
		sql << "DROP DATABASE blog";
	}
	catch (exception& e)
	{
		cerr << e.what() << endl;
	}
}
