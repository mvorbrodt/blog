#include <iostream>
#include <mutex>
#include <thread>
using namespace std;

const int COUNT = 10'000'000;

mutex aLock;
mutex bLock;

struct S
{
	unsigned int a:9;
	unsigned int b:7;
} __attribute__((packed));

int main(int argc, char** argv)
{
	S s{};

	thread t1([&]() {
		scoped_lock lock(aLock);
		for(int i = 0; i < COUNT; ++i)
		{
			s.a = 0;
			s.a = 0b111111111;
		}
	});

	thread t2([&]() {
		scoped_lock lock(bLock);
		for(int i = 0; i < COUNT; ++i)
		{
			s.b = 0;
			s.b = 0b1111111;
		}
	});

	t1.join();
	t2.join();

	cout << "sizeof(S) = " << sizeof(S) << ", " << s.a << ", " << s.b << endl;

	return 1;
}
