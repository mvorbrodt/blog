#pragma once

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

		if(!s_instance) s_instance.reset(new T(std::forward<Args>(args)...));
		else throw std::logic_error("This singleton has already been created!");
	}

	static T* Instance() noexcept { return s_instance.get(); }

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
#define SINGLETON_FRIEND(T) friend class singleton<T>

template<typename T>
class abstract_singleton
{
public:
	template<typename... Args>
	static void Create(Args&&... args)
	{
		static std::mutex s_lock;
		std::scoped_lock lock(s_lock);

		struct Q : T
		{
			using T::T;
			void __abstract_singleton__() override {}
		};

		if(!s_instance) s_instance.reset(new Q(std::forward<Args>(args)...));
		else throw std::logic_error("This abstract singleton has already been created!");
	}

	static T* Instance() noexcept { return s_instance.get(); }

protected:
	abstract_singleton() = default;
	abstract_singleton(const abstract_singleton&) = delete;
	abstract_singleton(abstract_singleton&&) = delete;
	abstract_singleton& operator = (const abstract_singleton&) = delete;
	abstract_singleton& operator = (abstract_singleton&&) = delete;
	virtual ~abstract_singleton() = default;

private:
	using storage_t = std::unique_ptr<T>;
	inline static storage_t s_instance = nullptr;

	virtual void __abstract_singleton__() = 0;
};

#define ABSTRACT_SINGLETON(T) : public abstract_singleton<T>
#define ABSTRACT_SINGLETON_CLASS(C) class C ABSTRACT_SINGLETON(C)
#define ABSTRACT_SINGLETON_STRUCT(S) struct S ABSTRACT_SINGLETON(S)
#define ABSTRACT_SINGLETON_FRIEND(T) friend class abstract_singleton<T>
