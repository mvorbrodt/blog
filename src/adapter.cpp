#include <iostream>
using namespace std;

class Interface1
{
public:
	virtual void Method1() = 0;
};

class Object1 : public Interface1
{
public:
	virtual void Method1() override {}
};

void Codebase1(Interface1* obj) {}

class Interface2
{
public:
	virtual void Method2() = 0;
};

class Object2 : public Interface2
{
public:
	virtual void Method2() override {}
};

void Codebase2(Interface2* obj) {}

class Adapter : public Interface1
{
public:
	explicit Adapter(Interface2* o) : obj{ o } {}
	virtual void Method1() override { obj->Method2(); }
private:
	Interface2* obj = nullptr;
};

int main()
{
	Object2 obj;
	Adapter adp(&obj);
	Codebase1(&adp);
}
