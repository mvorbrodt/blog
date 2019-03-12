#include <iostream>
#include <mutex>
#include <stack>
using namespace std;

mutex cout_lock;
#define trace(x) { scoped_lock<mutex> lock(cout_lock); cout << x << endl; }

template<typename T>
class queue
{
public:
	void push(const T& item)
	{
		spill(m_pop, m_push);
		m_push.push(item);
	}
	
	void pop(T& item)
	{
		spill(m_push, m_pop);
		item = m_pop.top();
		m_pop.pop();
	}
	
	bool empty() const noexcept
	{
		return m_push.empty() && m_pop.empty();
	}
	
private:
	void spill(stack<T>& from, stack<T>& to)
	{
		while(!from.empty())
		{
			to.push(from.top());
			from.pop();
		}
	}
	
	stack<T> m_push;
	stack<T> m_pop;
};

int main(int argc, char** argv)
{
	queue<int> q;
	
	q.push(1);
	q.push(2);
	q.push(3);
	
	while(!q.empty())
	{
		int item;
		q.pop(item);
		trace(item);
	}
	
	return 1;
}
