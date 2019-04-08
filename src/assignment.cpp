#include <iostream>
#include <utility>

using namespace std;

struct S
{
	S()
	: m_resource(new int)
	{
		cout << "S()" << endl;
		*m_resource = 1;
	}

	S(const S& s)
	: m_resource(new int)
	{
		cout << "S(const S&)" << endl;
		*m_resource = *s.m_resource;
	}

	S(S&&) = delete;

	S& operator = (const S& s)
	{
		cout << "operator = (const S&)" << endl;
		S temp(s); // May throw
		swap(temp); // Will never throw
		return *this;
	}

	S& operator = (S&&) = delete;

	~S()
	{
		cout << "~S()" << endl;
		delete m_resource;
	}

	void swap(S& s) noexcept
	{
		std::swap(m_resource, s.m_resource);
	}

	int* m_resource;
};

int main()
{
	S s1, s2;
	s1 = s2;
}
