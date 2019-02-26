#include <iostream>
#include "queue.h"
using namespace std;

int main()
{
	simple_queue<int> q;

	cout << q.try_push(1) << endl;
	cout << q.try_push(2) << endl;

	int x;
	cout << q.try_pop(x) << ", " << x << endl;
	cout << q.try_pop(x) << ", " << x << endl;
	cout << q.try_pop(x) << ", " << x << endl;
}
