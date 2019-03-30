#include <iostream>
#include <string>

using namespace std;

int main()
{
	auto capacity = string().capacity();
	auto small = string(capacity, '*');
	auto big = string(capacity + 1, '*');

	cout << "Capacity: " << capacity << endl;
	cout << "Small   : " << small.capacity() << endl;
	cout << "Big     : " << big.capacity() << endl;
}
