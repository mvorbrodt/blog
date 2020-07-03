#include <mutex>
#include <memory>
#include <utility>
#include <stdexcept>

template<typename T>
class singleton
{
public:
	template<typename... A>
	static void Create(A&&... a)
	{
		static std::mutex s_lock;
		std::scoped_lock lock(s_lock);

		struct Q : public T
		{
			using T::T;
			virtual void __abstract__() override {}
		};

		if(!s_ptr)
		{
			s_ptr = S{ new Q(std::forward<A>(a)...) };
		}
		else
		{
			throw std::logic_error("This singleton has already been created!");
		}
	}

	static T* Instance()
	{
		return s_ptr.get();
	}

protected:
	singleton() = default;
	singleton(const singleton&) = delete;
	singleton(singleton&&) = delete;
	singleton& operator = (const singleton&) = delete;
	singleton& operator = (singleton&&) = delete;
	virtual ~singleton() = default;

private:
	using S = std::unique_ptr<T>;
	inline static S s_ptr = nullptr;

	virtual void __abstract__() = 0;
};

#define IS_A_SINGLETON(T) : public singleton<T>
#define SINGLETON_CLASS(C) class C IS_A_SINGLETON(C)
#define SINGLETON_STRUCT(S) struct S IS_A_SINGLETON(S)
