#include <iostream>
#include <atomic>
#include <thread>
#include <vector>
using namespace std;

const unsigned int COUNT = 10;
const unsigned int THREADS = 16;
const unsigned int THREAD_MASK = 0b001;

int main(int argc, char** argv)
{
	atomic_uint flag{0};

	auto proc = [&](int thread, unsigned int thread_mask) {
		for(int i = 0; i < COUNT;)
		{
			flag.fetch_or(thread_mask);
			if(flag.load() == thread_mask)
			{
				cout << "T" << thread << " in critical section, i = " << i << endl;
				flag.fetch_xor(thread_mask);
				++i;
				this_thread::yield();
			}
			else
			{
				flag.fetch_xor(thread_mask);
				this_thread::yield();
			}
		}
	};

	vector<thread> vt;
	for(int i = 0; i < THREADS; ++i)
		vt.emplace_back(proc, i, THREAD_MASK << i);

	for(auto& t : vt)
		t.join();

	return 1;
}
