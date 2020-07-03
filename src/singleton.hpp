#include <mutex>
#include <memory>
#include <utility>
#include <stdexcept>

template<typename T>
class singleton
{
public:
	template<typename... Args>
	static void Create(Args&&... args)
	{
		static std::mutex s_lock;
		std::scoped_lock lock(s_lock);

		if(!s_instance)
		{
			s_instance.reset(new T(std::forward<Args>(args)...));
		}
		else
		{
			throw std::logic_error("This singleton has already been created!");
		}
	}

	static T* Instance()
	{
		return s_instance.get();
	}

protected:
	singleton() = default;
	singleton(const singleton&) = delete;
	singleton(singleton&&) = delete;
	singleton& operator = (const singleton&) = delete;
	singleton& operator = (singleton&&) = delete;
	~singleton() = default;

private:
	using storage_t = std::unique_ptr<T>;
	inline static storage_t s_instance = nullptr;
};

#define SINGLETON(T) final : public singleton<T>
#define SINGLETON_CLASS(C) class C SINGLETON(C)
#define SINGLETON_STRUCT(S) struct S SINGLETON(S)
#define SINGLETON_FRIEND(T) friend class singleton<T>;
