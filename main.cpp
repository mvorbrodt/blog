#include <unordered_map>
#include <unordered_set>
#include "trace.h"
using namespace std;

const int COUNT = 1'000'000;

int main(int argc, char** argv)
{
	unordered_map<int, int> m;
	for(int i = 0; i < COUNT; ++i)
		m[i] = i;

	trace("unordered_map");
	trace("initial");
	trace("size         = ", m.size());
	trace("bucket count = ", m.bucket_count());
	trace("load factor  = ", m.load_factor(), "\n");

	m.clear();
	m.reserve(COUNT);
	for(int i = 0; i < COUNT; ++i)
		m[i] = i;

	trace("reserved");
	trace("size         = ", m.size());
	trace("bucket count = ", m.bucket_count());
	trace("load factor  = ", m.load_factor(), "\n");

	m.max_load_factor(10);
	m.rehash(COUNT / 10);

	trace("re-hashed");
	trace("size         = ", m.size());
	trace("bucket count = ", m.bucket_count());
	trace("load factor  = ", m.load_factor(), "\n");

	unordered_set<int> s;
	for(int i = 0; i < COUNT; ++i)
		s.insert(i);

	trace("unordered_set");
	trace("initial");
	trace("size         = ", s.size());
	trace("bucket count = ", s.bucket_count());
	trace("load factor  = ", s.load_factor(), "\n");

	s.clear();
	s.reserve(COUNT);
	for(int i = 0; i < COUNT; ++i)
		s.insert(i);

	trace("reserved");
	trace("size         = ", s.size());
	trace("bucket count = ", s.bucket_count());
	trace("load factor  = ", s.load_factor(), "\n");

	s.max_load_factor(10);
	s.rehash(COUNT / 10);

	trace("re-hashed");
	trace("size         = ", s.size());
	trace("bucket count = ", s.bucket_count());
	trace("load factor  = ", s.load_factor(), "\n");

	return 1;
}
