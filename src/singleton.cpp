#include <iostream>
#include "singleton.hpp"

using namespace std;

class S : public singleton<S>
{
public:
	void foo() const
	{
		cout << "foo() x = " << _x << ", y = " << _y << ", z = " << _z << endl;
	}

private:
	friend singleton<S>;

	S(int x, int y, int z) : _x(x), _y(y), _z(z)
	{
		cout << "S(" << _x << ", " << _y << ", " << _z << ")" << endl;
	}

	int _x = 0, _y = 0, _z = 0;
};

int main()
{
	S::Create(1, 2, 3);
	try { S::Create(4, 5, 6); } catch(exception& e) { cout << e.what() << endl; }
	S::Instance()->foo();
}
