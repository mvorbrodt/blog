#include <iostream>
#include <ios>
#include <string>
#include "bloom.hpp"

using namespace std;

int main()
{
	auto set1 = {"Martin", "Vorbrodt", "C++", "Blog"};
	auto set2 = {"Not", "In", "The", "Set"};

	bloom_filter<string, 5> bloom(128);
	for(auto s : set1) bloom.add(s);

	std::boolalpha(cout);
	cout << "bloom_filter<string, 5> bloom(128);" << endl;
	for(auto s : set1) cout << "\tContains \"" << s << "\"\t: " << bloom.contains(s) << endl;
	for(auto s : set2) cout << "\tContains \"" << s << "\"\t: " << bloom.contains(s) << endl;
	cout << endl;

	fixed_bloom_filter<string, 128, 1> fixed_bloom;
	for(auto s : set1) fixed_bloom.add(s);

	cout << "fixed_bloom_filter<string, 128, 1> fixed_bloom;" << endl;
	for(auto s : set1) cout << "\tContains \"" << s << "\"\t: " << fixed_bloom.contains(s) << endl;
	for(auto s : set2) cout << "\tContains \"" << s << "\"\t: " << fixed_bloom.contains(s) << endl;
}
