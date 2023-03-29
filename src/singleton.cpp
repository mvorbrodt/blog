#include <iostream>
#include <stdexcept>
#include "singleton.hpp"

using namespace std;

SINGLETON_CLASS(A) {};
SINGLETON_STRUCT(B) {};
struct C final : public singleton<C> {};

ABSTRACT_SINGLETON_CLASS(AA) {};
ABSTRACT_SINGLETON_STRUCT(AB) {};
struct AC : public abstract_singleton<AC> {};

class S SINGLETON(S)
{
public:
	~S() { cout << "~S()" << endl; }
	void foo() { cout << "S::foo() x = " << _x << endl; }
	void bar() const { cout << "S::bar() x = " << _x << endl; }

private:
	// Constructor must be private to prevent creation of instances...
	// ...except by singleton<T> base class, which is our friend...
	SINGLETON_FRIEND(S);
	S(int x = -17) : _x(x) { cout << "S(" << _x << ")" << endl; }
	int _x = 0;
};

class AS ABSTRACT_SINGLETON(AS)
{
public:
	// No friendship needed if constructors are public...
	// ...but you still can't create instances of AS...
	// ...except by abstract_singleton<T> base class...
	// ...which internally erases the abstraction...
	//ABSTRACT_SINGLETON_FRIEND(AS);
	AS(int x = -20) : _x(x) { cout << "AS(" << _x << ")" << endl; }
	~AS() { cout << "~AS()" << endl; }
	void foo() { cout << "AS::foo() x = " << _x << endl; }
	void bar() const { cout << "AS::bar() x = " << _x << endl; }

private:
	int _x = 0;
};

int main()
{
	try { if(S::Instance() == nullptr) throw logic_error("class S not created yet!"); }
	catch (exception& e) { cerr << e.what() << endl; }

	S::Create(17);
	//S s(17); // Compile-time error, can't create instances...

	S::Create(-17); // no-op, already created...

	//*S::Instance() = *S::Instance(); // Compile-time error, can't copy/move singletons...
	S::Instance()->foo();
	S::Instance()->bar();
	S::Destroy();
	S::Destroy(); // no-op, already destroyed...

	try { if(AS::Instance() == nullptr) throw logic_error("class AS not created yet!"); }
	catch (exception& e) { cerr << e.what() << endl; }

	AS::Create(20);
	//AS s(20); // Compile-time error, can't create instances...

	AS::Create(-20); // no-op, already created...

	//*AS::Instance() = *AS::Instance(); // Compile-time error, can't copy/move singletons...
	AS::Instance()->foo();
	AS::Instance()->bar();
	AS::Destroy();
	AS::Destroy(); // no-op, already destroyed...

	cout << "Done!" << endl;
}
