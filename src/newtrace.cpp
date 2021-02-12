#include <iostream>
#include <vector>
#include <map>
#include <set>
#include "newtrace.hpp"

const int N = 100;

const bool DELETE = false;

int main()
{
	char* cp = new char;
	char* ca = new char [N];
	short* sp = new short;
	short* sa = new short [N];
	int* ip = new int;
	int* ia = new int [N];

	if(DELETE)
	{
		delete cp;
		delete [] ca;
		delete sp;
		delete [] sa;
		delete ip;
		delete [] ia;
	}

	dump_leaks();
}
