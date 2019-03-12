#include <iostream>
#include <iomanip>
#include <new>

struct T
{
	T() { std::cout << "T() @ " << std::hex << std::showbase << this << std::endl; }
	~T() { std::cout << "~T() @ " << std::hex << std::showbase << this << std::endl; }
};

int main(int argc, char** argv)
{
	// new
	// this:
	T* t1 = new T;
	// becomes this:
	T* t2 = (T*)operator new(sizeof(T));
	try
	{
		new (t2) T;
	}
	catch(...)
	{
		operator delete(t2);
		throw;
	}
	
	// delete
	// this:
	delete t1;
	// becomes this:
	t2->~T();
	operator delete(t2);
	
	// new []
	// this:
#define HOW_MANY 3
	T* t3 = new T[HOW_MANY];
	// becomes this:
	T* t4 = (T*)operator new(sizeof(size_t) + HOW_MANY * sizeof(T));
	*((size_t*)t4) = HOW_MANY;
	t4 = (T*)(((char*)t4) + sizeof(size_t));
	for(size_t i = 0; i < HOW_MANY; ++i)
	{
		try
		{
			new (t4 + i) T;
		}
		catch(...)
		{
			for(size_t i2 = 0; i2 < i; ++i2)
				t4[i2].~T();
			t4 = (T*)(((char*)t4) - sizeof(size_t));
			operator delete(t4);
			throw;
		}
	}
	
	// delete []
	// this:
	delete [] t3;
	// becomes:
	size_t how_many = *(size_t*)(((char*)t4) - sizeof(size_t));
	for(size_t i = 0; i < how_many; ++i)
		t4[i].~T();
	t4 = (T*)(((char*)t4) - sizeof(size_t));
	operator delete(t4);
	
	return 1;
}
