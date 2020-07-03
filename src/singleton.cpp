#include <iostream>
#include "singleton.hpp"

using namespace std;

SINGLETON_CLASS(X) {};
SINGLETON_STRUCT(Y) {};
struct Z : public singleton<Z> {};

class S IS_A_SINGLETON(S)//, public X // Compile-time error because singletons cannot be inherited...
{
public:
	S(int x, int y, int z) : _x(x), _y(y), _z(z)
	{
		cout << "S(" << _x << ", " << _y << ", " << _z << ")" << endl;
	}

	~S() { cout << "~S()" << endl; }

	void foo() const
	{
		cout << "foo() x = " << _x << ", y = " << _y << ", z = " << _z << endl;
	}

private:
	int _x = 0, _y = 0, _z = 0;
};

struct T : X, Y {};

int main()
{
	S::Create(1, 2, 3);
	T::X::Create();
	T::Y::Create();
	//S s(1,2,3); // Compile-time error, can't create instances...
	try { S::Create(4, 5, 6); } catch(exception& e) { cout << e.what() << endl; }
	//*S::Instance() = *S::Instance(); // Compile-time error, can't copy singletons...
	S::Instance()->foo();
	cout << "Done!" << endl;
}
