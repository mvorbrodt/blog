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

		if(!s_ptr)
		{
			s_ptr = S{ new T(std::forward<A>(a)...) };
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
	~singleton() = default;

private:
	using S = std::unique_ptr<T>;
	inline static S s_ptr = nullptr;
};
