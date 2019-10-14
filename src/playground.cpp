#include <iostream>
#include <iomanip>
#include <memory>

using namespace std;

int main()
{
	auto p = unique_ptr<int[]>(new int[1]);
}
