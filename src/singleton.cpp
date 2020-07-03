#include <iostream>
#include "singleton.hpp"

using namespace std;

SINGLETON_CLASS(A) {};
SINGLETON_STRUCT(B) {};
struct C final : public singleton<C> {};

class S SINGLETON(S)//, public A // Compile-time error because singletons cannot be inherited...
{
public:

	~S() { cout << "~S()" << endl; }

	void foo() const
	{
		cout << "foo() x = " << _x << ", y = " << _y << ", z = " << _z << endl;
	}

private:
	SINGLETON_FRIEND(S)

	S(int x, int y, int z) : _x(x), _y(y), _z(z)
	{
		cout << "S(" << _x << ", " << _y << ", " << _z << ")" << endl;
	}

	int _x = 0, _y = 0, _z = 0;
};

int main()
{
	S::Create(1, 2, 3);

	//S s(1,2,3); // Compile-time error, can't create instances...

	try
	{
		S::Create(4, 5, 6);
	}
	catch(exception& e)
	{
		cout << e.what() << endl;
	}

	//*S::Instance() = *S::Instance(); // Compile-time error, can't copy singletons...

	S::Instance()->foo();

	cout << "Done!" << endl;
}
