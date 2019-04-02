#include <iostream>
#include <stdexcept>

using namespace std;

struct P
{
	P() { throw logic_error("Logic error from P::P()"); }
};

struct Q
{
	Q() try : m_P()
	{
		cout << "Never printed!" << endl;
	}
	catch(exception& e)
	{
		cout << "Inside Q::Q() caught: " << e.what() << endl;
		throw runtime_error("Runtime error from Q::Q()");
	}

	P m_P;
};

int main() try
{
	Q q;
}
catch(exception& e)
{
	cout << "Inside main() caught: " << e.what() << endl;
}
