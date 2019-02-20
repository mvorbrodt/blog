#include <atomic>
#include <thread>
#include "trace.h"
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
		trace("t1 started");
		this_thread::sleep_for(1s);
		flag = true;
		FENCE_RELEASE;
		trace("t1 signals and exits");
	});

	thread t2([&]() {
		trace("t2 started");
		while(flag == false) FENCE_ACQUIRE;
		trace("t2 got signaled and exits");
	});

	t1.join();
	t2.join();

	return 1;
}
