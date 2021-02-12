#include <iostream>
#include <vector>
#include <map>
#include <set>
#include "newtrace.hpp"

const int N = 100;

enum TEST { GOOD, LEAK, MISMATCH, LEAK_MISMATCH };
TEST test = LEAK_MISMATCH;

struct S { S() { throw 1; } };

int main()
{
	try { new S; } catch(...) { } // not a leak!
	try { new S [N]; } catch(...) { } // same!

	char* cp = new char;
	char* ca = new char [N];
	short* sp = new short;
	short* sa = new short [N];
	int* ip = new int;
	int* ia = new int [N];

	switch(test)
	{
		case GOOD:
			delete cp;
			delete [] ca;
			delete sp;
			delete [] sa;
			delete ip;
			delete [] ia;
			break;
		case LEAK:
			break;
		case MISMATCH:
			delete [] cp;
			delete ca;
			delete [] sp;
			delete sa;
			delete [] ip;
			delete ia;
			break;
		case LEAK_MISMATCH:
			delete [] cp;
			delete ca;
			delete ip;
			delete [] ia;
			break;
	}
}
