#include <iostream>
#include <iomanip>
#include <string>
#include <chrono>
#include <vector>
#include <numeric>

using namespace std;
using namespace chrono;

const int ELEMS = 10'000'000;

struct Person
{
	Person(const string& n, uint8_t a, uint32_t d)
	: name(n), age(a), dob(d) {}
	
	string name;
	uint8_t age;
	uint32_t dob;
};

using VP = vector<Person>;

void addPerson(VP& v, Person&& p) { v.push_back(move(p)); }

uint64_t averageNameLen(const VP& v)
{
	return accumulate(begin(v), end(v), (uint64_t)0,
					  [](auto sum, auto& p) { return sum + p.name.length(); }) / v.size();
}

uint64_t averageAge(const VP& v)
{
	return accumulate(begin(v), end(v), (uint64_t)0,
					  [](auto sum, auto& p) { return sum + p.age; }) / v.size();
}

uint64_t averageDob(const VP& v)
{
	return accumulate(begin(v), end(v), (uint64_t)0,
					  [](auto sum, auto& p) { return sum + p.dob; }) / v.size();
}

struct Persons
{
	vector<string> names;
	vector<uint8_t> ages;
	vector<uint32_t> dobs;
	
	void addPerson(const string& n, uint8_t a, uint32_t d)
	{
		names.push_back(n);
		ages.push_back(a);
		dobs.push_back(d);
	}
	
	uint64_t averageNameLen() const
	{
		return accumulate(begin(names), end(names), (uint64_t)0,
						  [](auto sum, auto& n) { return sum + n.length(); }) / names.size();
	}
	
	uint64_t averageAge() const
	{
		return accumulate(begin(ages), end(ages), (uint64_t)0) / ages.size();
	}
	
	uint64_t averageDob() const
	{
		return accumulate(begin(dobs), end(dobs), (uint64_t)0) / dobs.size();
	}
};

int main()
{
	VP v1;
	v1.reserve(ELEMS);
	for(int i = 0; i < ELEMS; ++i)
		addPerson(v1, Person(string(string().capacity(), 'N'), i % 0xFF, i % 0xFFFF));
	
	auto start_time = high_resolution_clock::now();
	auto sum = averageNameLen(v1);
	sum += averageAge(v1);
	sum += averageDob(v1);
	auto end_time = high_resolution_clock::now();
	cout  << fixed << setprecision(3);
	cout << "AoS duration " << duration_cast<microseconds>(end_time - start_time).count() / 1000.f << " ms" << endl;
	v1.clear();
	v1.shrink_to_fit();
	
	Persons p;
	p.names.reserve(ELEMS);
	p.ages.reserve(ELEMS);
	p.dobs.reserve(ELEMS);
	for(int i = 0; i < ELEMS; ++i)
		p.addPerson(string(string().capacity(), 'N'), rand() % 0xFF, rand() % 0xFFFF);
	
	start_time = high_resolution_clock::now();
	sum += p.averageNameLen();
	sum += p.averageAge();
	sum += p.averageDob();
	end_time = high_resolution_clock::now();
	cout << "SoA duration " << duration_cast<microseconds>(end_time - start_time).count() / 1000.f << " ms" << endl;
}
