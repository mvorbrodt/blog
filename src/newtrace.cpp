#include <iostream>
#include <vector>
#include <map>
#include <set>
#include "newtrace.hpp"

const int N = 100;

const bool D = false; // true = GOOD CODE, false = BAD BAD CODE

int main()
{
	char* cp = new char;
	char* ca = new char [N];
	short* sp = new short;
	short* sa = new short [N];
	int* ip = new int;
	int* ia = new int [N];

	if(D)
	{
		delete cp;
		delete [] ca;
		delete sp;
		delete [] sa;
		delete ip;
		delete [] ia;
	}

	if(auto leaks = get_leaks(); !leaks.empty())
	{
		std::cerr << "\n";
		std::cerr << "**************************\n";
		std::cerr << "*** MEMORY LEAK REPORT ***\n";
		std::cerr << "**************************\n\n";

		for(const auto& entry : leaks)
		{
			std::cerr << "! " << entry.bytes << " bytes -> (" << entry.file << " : " << entry.proc << ", line " << entry.line << ")\n";
		}
		std::cerr << "\n";
	}
}
