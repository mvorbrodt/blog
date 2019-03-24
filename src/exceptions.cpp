#include <iostream>
#include <sstream>
#include <thread>
#include <exception>
#include <stdexcept>

using namespace std;

int main(int, char**)
{
	exception_ptr ep = nullptr;

	thread t([&]()
	{
		try
		{
			stringstream str;
			str << this_thread::get_id();
			throw runtime_error(str.str().c_str());
		}
		catch(...)
		{
			ep = current_exception();
		}
	});
	t.join();

	if(ep != nullptr)
	{
		try
		{
			rethrow_exception(ep);
		}
		catch(exception& e)
		{
			cout << "Thread " << this_thread::get_id() << " caught exception from thread " << e.what() << endl;
		}
	}
}
