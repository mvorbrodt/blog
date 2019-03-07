#include <iostream>
#include <mutex>
#include <map>
#include <utility>
#include <cstdlib>
using namespace std;

mutex cout_lock;
#define trace(x) { scoped_lock<mutex> lock(cout_lock); cout << x << endl; }

const int COUNT = 5;

int main(int argc, char** argv)
{
	srand((unsigned int)time(NULL));
	
	multimap<int, int> m1;
	for(int i = 0; i < COUNT; ++i)
		m1.insert(make_pair(rand() % 10, rand() % 10));
	
	trace("Source (Key -> Value):");
	for(const auto& it : m1)
		trace(it.first << " -> " << it.second);
	
	multimap<int, int> m2;
	for(const auto& it : m1)
		m2.insert(make_pair(it.second, it.first));
	
	trace("Output (Value -> Key):");
	for(const auto& it : m2)
		trace(it.first << " -> " << it.second);
	
	return 1;
}
