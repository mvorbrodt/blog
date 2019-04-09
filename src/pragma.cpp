#include <iostream>

using namespace std;

struct S1
{
	char c;
	short s;
	int i;
	long l;
	float f;
	double d;
};

#pragma pack(show)
#pragma pack(push, 1)
#pragma pack(show)

struct S2
{
	char c;
	short s;
	int i;
	long l;
	float f;
	double d;
};

#pragma pack(pop)

int main()
{
	cout << sizeof(S1) << ", " << sizeof(S2) << endl;
}
