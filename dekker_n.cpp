#include <iostream>
#include <atomic>
#include <thread>
#include <vector>
using namespace std;

const int COUNT = 5;
const int THREADS = thread::hardware_concurrency();
const int THREAD_MASK = 0b1;

int main(int argc, char** argv)
{
	atomic_uint flag{0};
	
	auto proc = [&](int t, unsigned int thread_mask) {
		for(int i = 0; i < COUNT;)
		{
			if(flag.fetch_or(thread_mask) == 0)
			{
				cout << "T" << t << " in critical section" << endl;
				++i;
			}
			
			flag.fetch_xor(thread_mask);
		}
	};
	
	vector<thread> vt;
	for(int i = 0; i < THREADS; ++i)
		vt.emplace_back(proc, i, THREAD_MASK << i);
	
	for(auto& t : vt)
		t.join();
	
	return 1;
}
