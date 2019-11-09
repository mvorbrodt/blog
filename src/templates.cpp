#include <iostream>
#include <vector>

using namespace std;

class NTB
{
public:
	virtual ~NTB() {}
	virtual void doWork() = 0;
};

template<typename T>
class TC : public NTB
{
public:
	TC(T t) : m_T(t) {}
	virtual void doWork() override { cout << m_T << endl; }
private:
	T m_T;
};

int main(int argc, char** argv)
{
	vector<NTB*> v;
	v.push_back(new TC<char>('X'));
	v.push_back(new TC<int>(1));
	v.push_back(new TC<float>(3.141f));
	
	for(auto it : v)
		it->doWork();
	
	for(auto it : v)
		delete it;
	
	return 1;
}
