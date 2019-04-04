#include <iostream>
#include <string>
#include "bloom.hpp"

using namespace std;

int main()
{
	string set1[] = {"Martin", "Vorbrodt", "C++", "Blog"};
	string set2[] = {"Not", "In", "The", "Set"};

	BloomFilter<string> bloom(128);

	for(auto& _ : set1)
		bloom.add(_);

	for(auto& _ : set1)
		cout << "Contains \"" << _ << "\" : " << bloom.contains(_) << endl;

	for(auto& _ : set2)
		cout << "Contains \"" << _ << "\" : " << bloom.contains(_) << endl;
}
