#include <iostream>
#include <iomanip>
#include <utility>
#include <atomic>
#include <memory>
#include <string>

template<typename T>
struct ctx_t
{
	ctx_t(T* ptr = nullptr) : obj(ptr)
	{
		std::cout << "ctx_t(T*) count = " << count << " obj = " << std::hex << std::showbase
			<< obj << std::dec << std::noshowbase << std::endl;
	}
	~ctx_t()
	{
		std::cout << "~ctx_t() count = " << count << " obj = " << std::hex << std::showbase
			<< obj << std::dec << std::noshowbase << std::endl;
	}

	std::atomic_uint count = 1; //typedef std::atomic<unsigned int> atomic_uint;
	T* obj;
};

template<typename T>
class sptr
{
public:
	sptr() : ctx{ new ctx_t<T> }
	{
		std::cout << "sptr() count = " << ctx->count << " obj = " << std::hex << std::showbase
			<< ctx->obj << std::dec << std::noshowbase << std::endl;
	}

	sptr(T* ptr) : ctx{ new ctx_t<T>{ ptr } } // if creation of ctx_t<T> throws, ptr will leak
	{
		std::cout << "sptr(T*) count = " << ctx->count << " obj = " << std::hex << std::showbase
			<< ctx->obj << std::dec << std::noshowbase << std::endl;
	}

	sptr(const sptr& o) noexcept : ctx{ o.ctx }
	{
		++ctx->count;
		std::cout << "sptr(const&) count = " << ctx->count << " obj = " << std::hex << std::showbase
			<< ctx->obj << std::dec << std::noshowbase << std::endl;
	}

	sptr(sptr&& o) noexcept : ctx{ std::exchange(o.ctx, nullptr) }
	{
		std::cout << "sptr(&&) count = " << ctx->count << " obj = " << std::hex << std::showbase
			<< ctx->obj << std::dec << std::noshowbase << std::endl;
	}

	~sptr() noexcept
	{
		if(ctx == nullptr)
		{
			std::cout << "~sptr() nothing to do!" << std::endl;
			return;
		}

		std::cout << "~sptr() count = " << ctx->count << " obj = " << std::hex << std::showbase
			<< ctx->obj << std::dec << std::noshowbase << std::endl;

		auto cur = ctx->count.fetch_sub(1);
		if(cur == 1)
		{
			std::cout << "\tlast one, deleting object and context pointers" << std::endl;
			delete ctx->obj;
			delete ctx;
		}
	}

	// implemented purely in terms of copy & move constructors and the destructor
	// proper reference counting will be done as long as constructors/destructor is correct
	// temporary and swap works here as well
	sptr& operator = (sptr o) noexcept
	{
		swap_it(o);
		return *this;
	}

	operator T& () const { return *ctx->obj; } // may want to check if ctx is null and throw if it is

	T* operator -> () const { return ctx->obj; } // may want to check if ctx is null and throw if it is

	friend std::ostream& operator << (std::ostream& os, const sptr& sp)
	{
		if(sp.ctx != nullptr)
			os << *(sp.ctx->obj);
		return os;
	}

private:
	void swap_it(sptr& o) { std::swap(ctx, o.ctx); }

	ctx_t<T>* ctx = nullptr; // sizeof(sptr<T>) == sizeof(T*) because ctx_t holds it all
};

int main()
{
	using namespace std;
	sptr<int> p1{ new int{ 17 } };
	sptr<int> e;
	e = p1;

	cout << p1 << endl;
	sptr<int> p2{ p1 }, p3{ p1 };
	cout << p2 << endl;
	sptr<int> p4{ std::move(p3) }; // after this p3 points to nothing
	cout << p4 << endl;

	p1 = p2;
	p1 = p1;
	p2 = std::move(p4);

	sptr<int> p5{ new int { 11 } };
	sptr<int> p6{ new int { 14 } };
	sptr<int> p7{ new int { 17 } };
	p5 = p6;
	p6 = std::move(p5);
	p5 = std::move(p7);

	sptr<string> sp1{ new string{ "sp1" } }, sp2{ new string{ "sp2" } }, sp3{ new string{ "sp3" } };
	cout << sp1 << ", " << sp2 << ", " << sp3 << std::endl;

	std::shared_ptr<int> pp{ new int }; // performs 2 memory allocations
	auto pp2 = std::make_shared<int>(1); // performs 1 memory allocation of sizeof(ctrl_t) + sizeof(T) bytes
	std::weak_ptr<int> wp{ pp2 }; // does not participate in shared ownership; used to observe state of shared object
	wp.use_count(); // will tell if shared pointer is safe to access if use_count > 0...
}
