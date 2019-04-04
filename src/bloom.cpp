#include <iostream>
#include <string>
#include "bloom.hpp"

using namespace std;

int main()
{
	string set1[] = {"Martin", "Vorbrodt", "C++", "Blog"};
	string set2[] = {"Not", "In", "The", "Set"};

	bloom_filter<string> bloom(128);

	for(auto& s : set1)
		bloom.add(s);

	for(auto& s : set1)
		cout << "Contains \"" << s << "\" : " << bloom.contains(s) << endl;

	for(auto& s : set2)
		cout << "Contains \"" << s << "\" : " << bloom.contains(s) << endl;
}
