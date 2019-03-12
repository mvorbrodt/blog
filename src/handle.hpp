#pragma once

template
<
	typename T,
	template<typename> typename AcquirePolicy,
	template<typename> typename ReleasePolicy
>
class handle
{
public:
	explicit handle(T h) : m_handle(h) { AcquirePolicy<T>::Execute(m_handle); }
	handle(const handle&) = delete;
	handle(handle&&) = default;
	handle& operator = (const handle&) = delete;
	handle& operator = (handle&&) = default;
	~handle() { ReleasePolicy<T>::Execute(m_handle); }

	operator T () const { return m_handle; }

private:
	T m_handle;
};

template<typename T> struct NoOpPolicy { static void Execute(T) noexcept {} };

template<typename T> struct PointerReleasePolicy { static void Execute(T ptr) noexcept { delete ptr; } };
template<typename T> struct ArrayReleasePolicy { static void Execute(T ptr) noexcept { delete[] ptr; } };

template<typename T> using arr_ptr_handle_t = handle<T*, NoOpPolicy, ArrayReleasePolicy>;
template<typename T> using ptr_handle_t = handle<T*, NoOpPolicy, PointerReleasePolicy>;
