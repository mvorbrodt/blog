#include <iostream>

using namespace std;

typedef void(*FuncPtr)(int arg);
FuncPtr callback;

void set_callback(FuncPtr fp) { callback = fp; }
void fire_callback(int arg) { callback(arg); }

int main()
{
	set_callback(+[](int arg) { cout << arg << endl; });
	fire_callback(42);
}
