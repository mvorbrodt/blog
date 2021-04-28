#include <iostream>
#include <memory>

using namespace std;

struct M
{
	M(const char* s) : m_s(s) { cout << "M::M(m_s = '" << m_s << "')" << endl; }
	~M() { cout << "M::~M(m_s = '" << m_s << "')" << endl; }

	string m_s;
};

struct S
{
	S() : S(2019) { cout << "S::S() throws..." << endl; throw runtime_error("BOOM!"); }
	S(int i) : S("C++") { cout << "S::S(int i = '" << i << "')" << endl; }
	S(const char* s) : m_vM("by value"), m_pM(make_unique<M>("by pointer"))
		{ cout << "S::S(const char* s = '" << s << "')" << endl; }
	~S() { cout << "S::~S() cleans up..." << endl; }

	M m_vM;
	unique_ptr<M> m_pM = nullptr;
};

int main()
{
	try
	{
		S s;
	}
	catch(exception& e)
	{
		cout << e.what() << endl;
	}
}
