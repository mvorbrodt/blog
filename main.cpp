#include <iostream>
#include "pool.h"
#include "trace.h"
using namespace std;

int main()
{
	thread_pool tp;
	thread([&]() {
		for(int i = 1; i <= 10; ++i)
			tp.enqueue_work([=]() { trace("work item ", i); return i * i; });
	}).join();
}
