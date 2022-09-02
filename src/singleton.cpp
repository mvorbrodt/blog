#include <iostream>
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
	S(int x) : _x(x) { cout << "S(" << _x << ")" << endl; }
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
	AS(int x) : _x(x) { cout << "AS(" << _x << ")" << endl; }
	~AS() { cout << "~AS()" << endl; }
	void foo() { cout << "AS::foo() x = " << _x << endl; }
	void bar() const { cout << "AS::bar() x = " << _x << endl; }

private:
	int _x = 0;
};

int main()
{
	try { S::Instance(); }
	catch (exception& e) { cerr << e.what() << endl; }

	S::Create(17);
	//S s(17); // Compile-time error, can't create instances...

	try { S::Create(17); }
	catch(exception& e) { cout << e.what() << endl; }

	//*S::Instance() = *S::Instance(); // Compile-time error, can't copy/move singletons...
	S::Instance()->foo();
	S::Instance()->bar();
	S::Destroy();

	try { S::Destroy(); }
	catch (exception& e) { cerr << e.what() << endl; }

	try { AS::Instance(); }
	catch (exception& e) { cerr << e.what() << endl; }

	AS::Create(20);
	//AS s(20); // Compile-time error, can't create instances...
	try { AS::Create(20); }
	catch(exception& e) { cout << e.what() << endl; }
	//*AS::Instance() = *AS::Instance(); // Compile-time error, can't copy/move singletons...
	AS::Instance()->foo();
	AS::Instance()->bar();
	AS::Destroy();

	try { AS::Destroy(); }
	catch (exception& e) { cerr << e.what() << endl; }

	cout << "Done!" << endl;
}
