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

void eat_it()
{
	try
	{
		throw runtime_error("System error from eat_it()");
	}
	catch(exception& e)
	{
		cout << "Swallowing: " << e.what() << endl;
	}
}

void eat_it_sugar() try
{
	throw runtime_error("System error from eat_it_sugar()");
}
catch(exception& e)
{
	cout << "Swallowing: " << e.what() << endl;
}

int main() try
{
	eat_it();
	eat_it_sugar();
	Q q;
}
catch(exception& e)
{
	cout << "Inside main() caught: " << e.what() << endl;
}
