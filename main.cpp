#include <iostream>
#include <atomic>
#include <thread>
using namespace std;

//#define FENCE

#ifdef FENCE
#define FENCE_ACQUIRE atomic_thread_fence(memory_order_acquire)
#define FENCE_RELEASE atomic_thread_fence(memory_order_release)
#else
#define FENCE_ACQUIRE
#define FENCE_RELEASE
#endif

int main(int argc, char** argv)
{
	bool flag = false;

	thread t1([&]() {
		this_thread::sleep_for(100ms);
		cout << "t1 started" << endl;
		flag = true;
		FENCE_RELEASE;
		cout << "t1 signals and exits" << endl;
	});

	thread t2([&]() {
		cout << "t2 started" << endl;
		while(flag == false) FENCE_ACQUIRE;
		cout << "t2 got signaled and exits" << endl;
	});

	t1.join();
	t2.join();

	return 1;
}
