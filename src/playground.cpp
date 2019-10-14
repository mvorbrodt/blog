#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <memory>
#include <exception>

using namespace std;

struct S
{
	S() : S(1) {}
	S(int) { throw runtime_error("S(int)"); }
	~S() { cout << "~S()" << endl; }
};

int main()
{
	auto p = unique_ptr<int[]>(new int[1]);

	try
	{
		S s;
	}
	catch(exception& e)
	{
		cout << e.what() << endl;
	}
}
