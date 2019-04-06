#include <iostream>

using namespace std;

int main()
{
	struct Old
	{
		int x;
		char padding[16 - sizeof(int)];
	};
	cout << "sizeof(Old): " << sizeof(Old) << endl << endl;

	struct alignas(16) New
	{
		int x;
	};
	cout << "sizeof(New): " << sizeof(New) << endl << endl;

	alignas(16) int x{}, y{};
	alignas(16) int z{};
	ptrdiff_t delta1 = (uint8_t*)&y - (uint8_t*)&x;
	ptrdiff_t delta2 = (uint8_t*)&z - (uint8_t*)&y;
	cout << "Address of 'x'      : " << &x << endl;
	cout << "Address of 'y'      : " << &y << endl;
	cout << "Address of 'z'      : " << &z << endl;
	cout << "Distance 'x' to 'y' : " << delta1 << endl;
	cout << "Distance 'y' to 'z' : " << delta2 << endl << endl;

	struct             Empty   {};
	struct alignas(64) Empty64 {};

	cout << "sizeof(Empty)  : " << sizeof(Empty)   << endl;
	cout << "sizeof(Empty64): " << sizeof(Empty64) << endl << endl;

	struct Full
	{
		alignas(32) char c;
		alignas(16) int x, y;
	};
	cout << "sizeof(Full): " << sizeof(Full) << endl << endl;
}
